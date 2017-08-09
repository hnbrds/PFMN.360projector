#include "VideoFrame.h"

using namespace std;
using namespace cv;

Player::Player(string const& _video_dir)
{
    video_dir = _video_dir;
    cap = new VideoCapture(video_dir);
    if(!cap->isOpened())
        cout << "ERROR : not opened" << endl;
    
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
