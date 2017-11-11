import fnmatch
import os
import sys

data_dir = '.'

def main():
    if len(sys.argv) >= 2:
        data_dir = sys.argv[1]

    with open("image_list.txt", 'a') as output:
        for root, dirnames, filenames in os.walk(data_dir):
            for filename in fnmatch.filter(filenames, "*.jp*g"):
                if(filename[0] == '.'):
                    continue
                file = os.path.join(root, filename) + "\n"
                #print(file)
                output.write(file)

if __name__ == '__main__':
    main()
