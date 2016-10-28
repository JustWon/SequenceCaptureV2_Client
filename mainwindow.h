#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// I don't know what it is... but it works..
#define _WIN32_WINNT 0x0600

#include <QMainWindow>
#include <iostream>
#include <boost/asio.hpp>
#include "threadtest.h"
#include "app.h"
#include <QDebug>

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	void showEvent(QShowEvent *ev);

private slots:
	void test_thread();
	void stream_on();

private:
    Ui::MainWindow *ui;

	Kinect kinect;

	ThreadTest* m_thread;
	ThreadTest* stream_thread;
};

#endif // MAINWINDOW_H
