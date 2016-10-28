#include "mainwindow.h"
#include "ui_mainwindow.h"

const char SERVER_IP[] = "172.26.27.104";
const unsigned short PORT_NUMBER = 31400;

class TCP_Client
{
public:
	TCP_Client(boost::asio::io_service& io_service)
		: m_io_service(io_service),
		m_Socket(io_service),
		m_nSeqNumber(0)
	{}

	void Connect(boost::asio::ip::tcp::endpoint& endpoint)
	{
		m_Socket.async_connect(endpoint,
			boost::bind(&TCP_Client::handle_connect,
				this,
				boost::asio::placeholders::error)
		);
	}

	MainWindow* p_mainwindow;

private:

	void PostReceive()
	{
		memset(&m_ReceiveBuffer, '\0', sizeof(m_ReceiveBuffer));

		m_Socket.async_read_some(boost::asio::buffer(m_ReceiveBuffer),
			boost::bind(&TCP_Client::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		);

	}


	void handle_connect(const boost::system::error_code& error)
	{
		if (error)
		{
			std::cout << "connect failed : " << error.message() << std::endl;
		}
		else
		{
			std::cout << "connected" << std::endl;

			PostReceive();
		}
	}

	void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
	{
	}

	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (error)
		{
			if (error == boost::asio::error::eof)
			{
				std::cout << "서버와 연결이 끊어졌습니다" << std::endl;
			}
			else
			{
				std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
			}
		}
		else
		{
			const std::string strRecvMessage = m_ReceiveBuffer.data();
			std::cout << "서버에서 받은 메시지: " << strRecvMessage << ", 받은 크기: " << bytes_transferred << std::endl;

			static boost::thread thread;
			if (!strcmp(strRecvMessage.c_str(), "stream_on"))
			{
				qDebug() << "stream on if";
				p_mainwindow->stream_on_flag = true;
				thread = boost::thread(boost::bind(&MainWindow::stream_on, p_mainwindow));
			}
			else if (!strcmp(strRecvMessage.c_str(), "stream_off"))
			{
				qDebug() << "stream off elses";
				p_mainwindow->stream_on_flag = false;
			}
			else if (!strcmp(strRecvMessage.c_str(), "still_capture"))
			{
				qDebug() << "still capture";
				
				p_mainwindow->kinect.update();
				p_mainwindow->kinect.drawDepth();
				p_mainwindow->kinect.saveDepth();
			}

			PostReceive();
		}
	}



	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::socket m_Socket;

	int m_nSeqNumber;
	std::array<char, 128> m_ReceiveBuffer;
	std::string m_WriteMessage;
};

boost::asio::io_service io_service;
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
TCP_Client client(io_service);


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
	while (stream_on_flag)
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

	// stream off
	Mat mat_img = Mat::zeros(640, 480, CV_32FC1);
	QImage q_image;
	cv::resize(mat_img, mat_img, cv::Size(ui->label_KinectDepth->width(), ui->label_KinectDepth->height()));
	q_image = Mat2QImage_depth(mat_img);
	ui->label_KinectDepth->setPixmap(QPixmap::fromImage(q_image));
}

void MainWindow::showEvent(QShowEvent* event) {
	QWidget::showEvent(event);

	client.p_mainwindow = this;

	client.Connect(endpoint);
	boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));
}