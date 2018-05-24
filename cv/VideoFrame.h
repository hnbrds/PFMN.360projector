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
    
    std::string read_output;
    // file stream for reading image_list
    std::ifstream image_list_stream;
    cv::Mat frame;

public:
    // File paths
    std::string image_list_path;
    // filename : (e.g. 123.jpg => 123)
    std::string filename;
    // subshot directory root : (e.g. ../subshot/)
    std::string subshot_root;
    // directory path without subshot : (e.g. wedding/v=123123/)
    std::string mid_path;
    // function for parsing paths
    void parse_filepaths(std::string&);

    Player(std::string const&, std::string const&);
    cv::Mat getCurrentFrame(void);
    cv::Mat getNextFrame(void);
    int getFrameCount(void);
    void outputFrame(const char *filename, float *buffer, int width, int height);
};
#endif
