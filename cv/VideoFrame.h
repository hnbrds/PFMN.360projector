#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

class Player
{
private:
    int cur_frame;
    double frame_rate;
    //std::string video_dir;
    std::string image_dir;
    //cv::VideoCapture* cap;
    cv::Mat cap;
    
public:
    Player();
    Player(std::string const&);
    
    cv::Mat getNextFrame(void);
    int getFrameCount(void);
};
#endif
