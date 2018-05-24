#include "VideoFrame.h"

using namespace std;
using namespace cv;


/*
 Check if file already exists
 input args : char* / string
 output : boolean
 */
bool fexists(const char *filename) {
    std::ifstream ifile(filename);
    return (bool)ifile;
}

bool fexists(const std::string& filename){
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
}


void Player::parse_filepaths(std::string& filepath){
    string::size_type start = filepath.find_last_of("/")+1;
    string::size_type end = filepath.find_last_of(".");
    
    this->filename = filepath.substr(start, end-start);
    
    int rootlen = subshot_root.length();
    this->mid_path = filepath.substr(rootlen, start-rootlen);
}


Player::Player(string const& _image_list_path, string const& _subshot_root)
{
    subshot_root = _subshot_root;
    image_list_stream.open(_image_list_path);
    if(image_list_stream.is_open()){
        if(!image_list_stream.eof()){
            image_list_stream >> read_output;
            parse_filepaths(read_output);
            cout << read_output << endl;
            frame = imread(read_output);
        }
    }
}


cv::Mat Player::getCurrentFrame(void)
{
    return frame;
}


cv::Mat Player::getNextFrame(void)
{
    if(image_list_stream.is_open()){
        if(!image_list_stream.eof()){
            image_list_stream >> read_output;
            parse_filepaths(read_output);
            cout << read_output << endl;
            frame = imread(read_output);
            cur_frame++;
        }
        else{
            cout << "Projection End" << endl;
            exit(0);
        }
    }
    return frame;
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
    
    imwrite(filename, img);
}
