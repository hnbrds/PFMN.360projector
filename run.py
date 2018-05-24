import os
import sys

import glob
import subprocess

IMGPATH_F = "image_paths.txt"
SUBSHOTPATH = "../subshots/"
PROJPATH = "../81patches"

def main():
    if not os.path.exists(PROJPATH):
        os.makedirs(PROJPATH)

    with open(IMGPATH_F, 'w') as image_path:
        for root, dirs, files in os.walk(SUBSHOTPATH):
            for file in files:
                if file.endswith(".jpg") or file.endswith(".jpeg"):
                    image_path.write(os.path.join(root, file) + '\n')

    proc = subprocess.Popen(["./OgreFltk", IMGPATH_F, SUBSHOTPATH, PROJPATH])
    proc.wait()

    os.remove(IMGPATH_F)

if __name__ == "__main__":
    main()
