#include "VideoFrame.h"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]){
    Mat frame;
    string video_dir = argv[1];
    cout << video_dir << endl;
    Player player = Player(video_dir);

    namedWindow("Sample Video", 1);
    for(;;){
      imshow("Sample Video", player.getNextFrame());
      if(waitKey(30) == 27) break;
    }

    return 0;
}
