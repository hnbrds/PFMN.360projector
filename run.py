import os
import sys

import glob
import subprocess

import hickle
from IPython import embed

IMGPATH_F = "image_paths.txt"
# frame directory  path
SUBSHOTPATH = "../../tmp_sum/360video/frames/"
# projection directory path
PROJPATH = "../../tmp_sum/360video/81patches"
# PROJPATH = "../../tmp_sum/360video/negative"

def main():
    if not os.path.exists(PROJPATH):
        os.makedirs(PROJPATH)

    # segmentation load
    seg = hickle.load('../../tmp_sum/360video/kts_segmentation.hkl')

    with open(IMGPATH_F, 'w') as image_path:
        for root, dirs, files in os.walk(SUBSHOTPATH):
            for dir in dirs:
                if dir.startswith('v='):
                    topic = root.split('/')[-2]
                    video_id = dir[2:]
                    cps = seg[topic][video_id]
                    middle = [(cps[i]+cps[i+1])//2 for i in xrange(cps.size-1)]
                    for frame_num in middle:
                        image_path.write(os.path.join(root, dir, '%06d.jpg' % frame_num) + '\n')
            '''
            for file in files:
                if file.endswith(".jpg") or file.endswith(".jpeg"):
                    image_path.write(os.path.join(root, file) + '\n')
            '''
    proc = subprocess.Popen(["./OgreFltk", IMGPATH_F, SUBSHOTPATH, PROJPATH])
    proc.wait()

    os.remove(IMGPATH_F)

if __name__ == "__main__":
    main()
