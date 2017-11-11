#include "VideoFrame.h"

using namespace std;
using namespace cv;

void Player::parsefile(std::string& filepath){
    string::size_type start = filepath.find_last_of("/")+1;
    string::size_type end = filepath.find_last_of(".");
    //printf("%d %d\n", start, end);
    
    this->filename = filepath.substr(start, end-start);
    //cout << this->filename << endl;
    this->fileroot = filepath.substr(27, start-27);
    //this->fileroot = "asdf";
    //cout << this->fileroot << endl;
    
}

bool fexists(const char *filename) {
    std::ifstream ifile(filename);
    return (bool)ifile;
}

bool fexists(const std::string& filename){
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
}

/*
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
*/

Player::Player()
{
    image_list.open(image_dir);
    if(image_list.is_open()){
        if(!image_list.eof()){
            image_list >> read_output;
            parsefile(read_output);
            cout << read_output << endl;
            cap = imread(read_output);
        }
    }
}

Player::Player(string const& _image_dir)
{
    image_dir = _image_dir;
    cap = imread(image_dir);
}

cv::Mat Player::getCurrentFrame(void)
{
    return cap;
}

cv::Mat Player::getNextFrame(void)
{
    if(image_list.is_open()){
        
        if(!image_list.eof()){
            image_list >> read_output;
            parsefile(read_output);
            cout << read_output << endl;
            cap = imread(read_output);
        }
    }
    return cap;
}

int Player::getFrameCount(void)
{
    return cur_frame;
}

void Player::outputFrame(const char *filename, float *buffer, int width, int height)
{
    cout << filename << endl;
    Mat tmp = cv::Mat(height, width, CV_32FC4, buffer);
    cvtColor(tmp, tmp, CV_BGR2RGB);
    flip(tmp, tmp, 0);
    
    Mat img;
    tmp.convertTo(img, CV_8UC4, 255.0);
    
    //namedWindow("Output Image", CV_WINDOW_AUTOSIZE);
    //imshow("Output Image", img);
    imwrite(filename, img);
}
