#include "VideoFrame.h"

using namespace std;
using namespace cv;

Player::Player(string const& _video_dir)
{
    video_dir = _video_dir;
    cap = new VideoCapture(video_dir);
    if(!cap->isOpened())
    {
        // ERROR msg
    }
    
    frame_rate = cap->get(CV_CAP_PROP_FPS);
}

cv::Mat Player::getNextFrame(void)
{
    Mat frame;
    *cap >> frame;
    cur_frame++;
    return frame;
}

int Player::getFrameCount(void)
{
    return cur_frame;
}

/*
int main(int argc, char * argv[])
{
    Mat frame;
    // Init videocapture
    VideoCapture cap("../video/sample1.mp4");
    if(!cap.isOpened())
        return -1;
    
    double fps = cap.get(CV_CAP_PROP_FPS);
    cout << fps << endl;
    
    //Mat edges;
    namedWindow("Sample video", 1);
    for(;;)
    {
        Mat frame;
        cap >> frame;
        imshow("Sample video", frame);
        if(waitKey(30) == 27) break;
    }
    return 0;
}
*/
