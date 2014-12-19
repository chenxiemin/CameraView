#include <iostream>
#include <string>
#include <stdlib.h>

#include "sdl-player.h"

using namespace std;


int main(int argc, char **argv)
{
    int width = 640;
    int height = 360;
    
    string url = "rtsp://192.168.0.129/12 rtsp://192.168.0.128/12 rtsp://192.168.0.130/12 rtsp://192.168.0.131/12";
    // string url = "rtsp://127.0.0.1/test.264 rtsp://127.0.0.1/test10.264";
    // string url = "rtsp://192.168.0.11/live-high";
    if (argc >= 2)
        width = atoi(argv[1]);
    if (argc >= 3)
        height = atoi(argv[2]);
    if (argc >= 4) {
        url = "";
        for (int i = 3; i < argc; i++) {
            url += argv[i];
            url += " ";
        }
    }
	SdlMain(width, height, url.c_str());

	return 0;
}
