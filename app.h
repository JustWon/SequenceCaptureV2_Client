#ifndef __APP__
#define __APP__

#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>

#include <vector>
#include <string>

#include <wrl/client.h>
using namespace Microsoft::WRL;
using namespace std;

class Kinect
{
public:
    // Sensor
    ComPtr<IKinectSensor> kinect;

    // Reader
    ComPtr<IDepthFrameReader> depthFrameReader;

    // Depth Buffer
    std::vector<UINT16> depthBuffer;
    int depthWidth;
    int depthHeight;
    unsigned int depthBytesPerPixel;
    cv::Mat depthMat;
	cv::Mat scaleMat;

	int cnt = 0;

public:
    // Constructor
    Kinect();

    // Destructor
    ~Kinect();

    // Processing
    void run();

public:
    // Initialize
    void initialize();

    // Initialize Sensor
    inline void initializeSensor();

    // Initialize Depth
    inline void initializeDepth();

    // Finalize
    void finalize();

    // Update Data
    void update();

    // Update Depth
    inline void updateDepth();

    // Draw Data
    void draw();

    // Draw Depth
    inline void drawDepth();

    // Show Data
    void show();

    // Show Depth
    inline void showDepth();

	void saveDepth(string dir_path, int cnt);
};

#endif // __APP__