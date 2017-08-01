#include "VideoFrame.h"

using namespace std;
using namespace cv;

int main(int argc, char * argv[])
{
    Mat frame;
    // Init videocapture
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;
        
    //Mat edges;
    namedWindow("Sample video", 1);
    while(true){
        Mat frame;
        cap >> frame;
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        imshow("Sample video", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
