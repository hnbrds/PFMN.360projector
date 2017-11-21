#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

class Player
{
private:
    int cur_frame = 0;
    double frame_rate;
    std::string image_dir = "./sampled_list_parade.txt";
    std::string read_output;
    std::ifstream image_list;
    cv::VideoCapture* cap;
    cv::Mat frame;

public:
    std::string video_dir;
    std::string filename;
    std::string fileroot;
    void parsefile(std::string&);

    Player();
    Player(std::string const&);
    cv::Mat getCurrentFrame(void);
    cv::Mat getNextFrame(void);
    int getFrameCount(void);
    void outputFrame(const char *filename, float *buffer, int width, int height);
};
#endif
