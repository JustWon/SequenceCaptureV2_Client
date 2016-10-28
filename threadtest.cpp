#include "threadtest.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;


ThreadTest::ThreadTest(QObject *parent) :
QThread(parent)
{
}

void ThreadTest::run()
{
	while (1)
	{
		emit tick(0);
		msleep(30);
	}
}