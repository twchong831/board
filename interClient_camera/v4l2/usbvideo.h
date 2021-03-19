#ifndef USBVIDEO_H
#define USBVIDEO_H
/**
* @file     usbvideo.h
* @brief    카네비컴 usb 카메라를 파싱하기 위한 헤디
* @details  카네비컴 usb 카메라를 v4l2를 기반으로 입력 받아 이를 처리
* @date     2021-03-09
* @version  0.0.1
*/

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <libv4l2.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer
{
    void *start;
    size_t lenghth;
};

typedef struct rgbpacket
{
    /* data */
    std::vector<int> red;
    std::vector<int> green;
    std::vector<int> blue;
    int height;
    int width;
    rgbpacket()
    {
        red.resize(921600);
        green.resize(921600);
        blue.resize(921600);
        height = 1280;
        width = 720;
    }
    void clear()
    {
        red.clear();
        green.clear();
        blue.clear();
        if(height != 0 && width != 0)
        {
            red.resize(height * width);
            green.resize(height * width);
            blue.resize(height * width);
        }
    }
}RGBPACKET;

typedef struct size_
{
    int width;
    int height;
    int size;
    uint8_t* bufferPtr;
    size_()
    {
        width = 0;
        height = 0;
        size = 0;
        bufferPtr = 0;
    }

}C_SIZE;


class usbVideo{

public:
    usbVideo();

    void setSize(int h, int w);
    void init();

    int connect();
    void close();
    void setDeviceName(char *name);

    RGBPACKET getData();
    C_SIZE getDataPtr();

    void start();
    uint8_t* getData_thread();
    RGBPACKET getDatapacket_thread();
    
private:
    //func.
    void xioctl(int fh, int request, void *arg);
    void setFormat(int fd, v4l2_format &fmt);
    void setRequestbuffer(int fd, v4l2_requestbuffers &req);

    //var.
    RGBPACKET packet;
    int m_fd;
    char *m_devName;
    
    struct v4l2_format m_fmt;
    struct v4l2_buffer m_buf;
    struct v4l2_requestbuffers  m_req;

    struct buffer *m_buffers;

    enum v4l2_buf_type  m_type;

    u_int n_buffer;

    uint8_t *m_dataPtr;

    int g_width;
    int g_height;
};

#endif // USBVIDEO_H