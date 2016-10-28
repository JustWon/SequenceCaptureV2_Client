#include "mainwindow.h"
#include "ui_mainwindow.h"

const char SERVER_IP[] = "172.26.27.104";
const unsigned short PORT_NUMBER = 31400;

boost::asio::io_service io_service;
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
boost::system::error_code connect_error;
boost::asio::ip::tcp::socket socket_(io_service);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QImage Mat2QImage_color(const cv::Mat3b &src) {
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
	for (int y = 0; y < src.rows; ++y) {
		const cv::Vec3b *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
			destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
		}
	}
	return dest;
}
QImage Mat2QImage_depth(const cv::Mat_<double> &src)
{
	double scale = 1;
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
	for (int y = 0; y < src.rows; ++y) {
		const double *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
			unsigned int color = srcrow[x] * scale;
			destrow[x] = qRgba(color, color, color, 255);
		}
	}
	return dest;
}
void MainWindow::stream_on()
{
	kinect.update();
	kinect.drawDepth();
	kinect.showDepth();
	
	Mat mat_img = kinect.scaleMat;
	QImage q_image;
	
	cv::resize(mat_img, mat_img, cv::Size(ui->label_KinectDepth->width(), ui->label_KinectDepth->height()));
	q_image = Mat2QImage_color(mat_img);
	ui->label_KinectDepth->setPixmap(QPixmap::fromImage(q_image));

}
void MainWindow::test_thread()
{
	std::array<char, 128> buf;
	buf.assign(0);
	boost::system::error_code error;
	size_t len = socket_.read_some(boost::asio::buffer(buf), error);
	std::cout << "서버로부터 받은 메시지: " << &buf[0] << std::endl;
	std::string str(std::begin(buf), std::end(buf));
	
	if (!strcmp(str.c_str(), "stream_on")) {
		qDebug() << "stream_on";
		stream_thread = new ThreadTest(this);
		connect(stream_thread, SIGNAL(tick(int)), this, SLOT(stream_on(void)));
		stream_thread->start();
	}
	else if (!strcmp(str.c_str(), "stream_off")) {
		qDebug() << "stream_off";
		stream_thread->terminate();
	}
}

void MainWindow::showEvent(QShowEvent* event) {
	QWidget::showEvent(event);

	socket_.connect(endpoint, connect_error);
	if (connect_error) {
		std::cout << "연결 실패. error No: " << connect_error.value() << ", Message: " << connect_error.message() << std::endl;
	}
	else {
		std::cout << "서버에 연결 성공" << std::endl;
	}

	m_thread = new ThreadTest(this);
	connect(m_thread, SIGNAL(tick(int)), this, SLOT(test_thread(void)));
	m_thread->start();
}