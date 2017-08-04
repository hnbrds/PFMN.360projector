# 360VR Projector

Equirectangular video to sphere projector

## Environment
- macOS Sierra 10.12.5
- XCode 8.3.2

## Requirements
- OpenGL 3
- GLFW 3
- OpenCV 3.2.0 C++ framework

## How to

### CV module test
``g++ test.cpp VideoFrame.cpp -o test `pkg-config --cflags --libs opencv` ``
<br> run `./test`

### VR Player
`cmake .`
<br> `make`
<br> run ``./Ogrefltk``

### Projection module
none