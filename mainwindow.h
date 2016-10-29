#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// I don't know what it is... but it works..
#define _WIN32_WINNT 0x0600

#include <QMainWindow>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost\filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
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

	bool stream_on_flag = false;
	bool stream_save_flag = false;
	bool still_save_flag = false;

	void stream_on();
	void sandbox_create(string sandbox_path);

public:
    Ui::MainWindow *ui;

	Kinect kinect;

	int still_cnt = 0;
	int sequence_cnt = 0;

	string sandbox_dir_path;
	string still_save_dir_path;
	string sequence_save_dir_path;
};

#endif // MAINWINDOW_H
