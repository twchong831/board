#include "usbvideo.h"

usbVideo::usbVideo()
{
    CLEAR(m_fmt);
    CLEAR(m_req);
    m_devName = "/dev/video0";
    g_width = 1280;
    g_height = 720;
}

void usbVideo::setSize(int h, int w)
{
    packet.width = w;
    packet.height = h;
    packet.blue.resize(w * h);
    packet.green.resize(w * h);
    packet.red.resize(w * h);

    g_width = w;
    g_height = h;
}

void usbVideo::init()
{
    printf("init\n");
    setFormat(m_fd, m_fmt);
    setRequestbuffer(m_fd, m_req);

    //=----
    // struct v4l2_capability caps = {0};
    // xioctl(m_fd, VIDIOC_QUERYCAP, &caps);

    // printf( "Driver Caps:\n"
    //         "  Driver: \"%s\"\n"
    //         "  Card: \"%s\"\n"
    //         "  Bus: \"%s\"\n"
    //         "  Version: %d.%d\n"
    //         "  Capabilities: %08x\n",
    //         caps.driver, caps.card, caps.bus_info,
    //         (caps.version>>16)&&0xff, (caps.version>>24)&&0xff, caps.capabilities);

    // struct v4l2_cropcap cropcap = {0};
    // cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // xioctl(m_fd, VIDIOC_CROPCAP, &cropcap);

    // printf( "Camera Cropping:\n"
    //         "  Bounds: %dx%d+%d+%d\n"
    //         "  Default: %dx%d+%d+%d\n"
    //         "  Aspect: %d/%d\n",
    //         cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
    //         cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
    //         cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);
    // int support_grbg10 = 0;
 
    // struct v4l2_fmtdesc fmtdesc = {0};
    // fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // char fourcc[5] = {0};
    // char c, e;
    // printf("  FMT : CE Desc\n--------------------\n");
    // xioctl(m_fd, VIDIOC_ENUM_FMT, &fmtdesc);
    // {
    //     strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
    //     if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
    //         support_grbg10 = 1;
    //     c = fmtdesc.flags & 1? 'C' : ' ';
    //     e = fmtdesc.flags & 2? 'E' : ' ';
    //     printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
    //     fmtdesc.index++;
    // }
    //======

    m_buffers = (buffer*)calloc(m_req.count, sizeof(*m_buffers));

    printf("req.count ? %d \n", m_req.count);
    for(n_buffer=0; n_buffer < m_req.count; ++n_buffer)
    {
        CLEAR(m_buf);
        m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        m_buf.memory = V4L2_MEMORY_MMAP;
        m_buf.index = n_buffer;

        xioctl(m_fd, VIDIOC_QUERYBUF, &m_buf);

        m_buffers[n_buffer].lenghth = m_buf.length;
        m_buffers[n_buffer].start = v4l2_mmap(NULL, m_buf.length, 
                                            PROT_READ | PROT_WRITE, MAP_SHARED,
                                            m_fd, m_buf.m.offset);
        if(m_buffers[n_buffer].start == MAP_FAILED)
        {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
    }
    printf("n_buffer is %d\n", n_buffer);

    // //re-init?? v4l2 buffer
    for(int i=0; i<n_buffer; ++i)
    {
        CLEAR(m_buf);
        m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        m_buf.memory = V4L2_MEMORY_MMAP;
        m_buf.index = i;
        xioctl(m_fd, VIDIOC_QBUF, &m_buf);
    }

    m_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(m_fd, VIDIOC_STREAMON, &m_type);

    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = 20;

    printf("1 %d\n", m_fd);
    if(select(m_fd+1, &fds, NULL, NULL, &tv) == -1)
    {
        perror("select error");
    }
}

int usbVideo::connect()
{
    printf("connect\n");
    m_fd = v4l2_open(m_devName, O_RDWR | O_NONBLOCK, 0);
    if(m_fd < 0 )
    {
        perror("can not open device");
        exit(EXIT_FAILURE);
    }
    printf("fd : %d\n", m_fd);

    init();
    
    return 0;
}

void usbVideo::close()
{
    m_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(m_fd, VIDIOC_STREAMOFF, &m_type);
    for(int i=0; i<n_buffer; ++i)
    {
        v4l2_munmap(m_buffers[i].start, m_buffers[i].lenghth);
    }
    v4l2_close(m_fd);
}

void usbVideo::setDeviceName(char *name)
{
    m_devName = name;
}

RGBPACKET usbVideo::getData()
{
    clock_t cl_start = clock();
    double cl_result = 0;

    CLEAR(m_buf);
    m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_buf.memory = V4L2_MEMORY_MMAP;
    uint8_t *ptrpos = (uint8_t*)m_buffers[m_buf.index].start;
    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 0] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    xioctl(m_fd, VIDIOC_DQBUF, &m_buf);
    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 1] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    xioctl(m_fd, VIDIOC_QBUF, &m_buf);
    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 2] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    // ---check rgb
    int cnt = 0;
    for(int i=0; i<m_buffers[0].lenghth; i+=3)
    {
        packet.blue[cnt] = ptrpos[i];
        packet.green[cnt] = ptrpos[i+1];
        packet.red[cnt] = ptrpos[i+2];

        // printf(" (%d, %d, %d) ", packet.blue[cnt],packet.green[cnt],packet.red[cnt]);
        cnt++;
    }

    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 3] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    return packet;
}

C_SIZE usbVideo::getDataPtr()
{
    C_SIZE pos;
    clock_t cl_start = clock();
    double cl_result = 0;

    CLEAR(m_buf);
    m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_buf.memory = V4L2_MEMORY_MMAP;
    xioctl(m_fd, VIDIOC_DQBUF, &m_buf);
    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 1] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    xioctl(m_fd, VIDIOC_QBUF, &m_buf);
    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 2] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    pos.bufferPtr = (uint8_t*)m_buffers[m_buf.index].start;
    pos.size = m_buffers[m_buf.index].lenghth;
    pos.height = m_fmt.fmt.pix.height;
    pos.width = m_fmt.fmt.pix.width;
    // uint8_t *ptrpos = (uint8_t*)m_buffers[m_buf.index].start;
    
    // size = m_buffers[0].lenghth;

    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time 3] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

    return pos;
}

void usbVideo::start()
{
    clock_t cl_start = clock();
    double cl_result = 0;

    CLEAR(m_buf);
    m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_buf.memory = V4L2_MEMORY_MMAP;
    m_dataPtr = (uint8_t*)m_buffers[m_buf.index].start;
    xioctl(m_fd, VIDIOC_DQBUF, &m_buf);
    // int cnt = 0;
    // for(int i=0; i<m_buffers[0].lenghth; i+=3)
    // {
    //     packet.blue[cnt] = m_dataPtr[i];
    //     packet.green[cnt] = m_dataPtr[i+1];
    //     packet.red[cnt] = m_dataPtr[i+2];

    //     // printf(" (%d, %d, %d) ", packet.blue[cnt],packet.green[cnt],packet.red[cnt]);
    //     cnt++;
    // }
    xioctl(m_fd, VIDIOC_QBUF, &m_buf);

    cl_result = static_cast<double>(clock() - cl_start);
    printf("[usbvideo][dataget time] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

}
uint8_t* usbVideo::getData_thread()
{
    return m_dataPtr;
}

RGBPACKET usbVideo::getDatapacket_thread()
{
    return packet;
}

void usbVideo::xioctl(int fh, int request, void *arg)
{
    int r;
    int cnt=0;

    do
    {
        /* code */
        r = v4l2_ioctl(fh, request, arg);
        // if(cnt == 0 || r == 0)
            // printf("r : %d %d %d\n", r, errno, cnt);
        cnt++;
    } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));
    
    if(r == -1)
    {
        fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void usbVideo::setFormat(int fd, v4l2_format &fmt)
{
    CLEAR(fmt);
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = g_width;
    fmt.fmt.pix.height      = g_height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    xioctl(fd, VIDIOC_S_FMT, &fmt);
}

void usbVideo::setRequestbuffer(int fd, v4l2_requestbuffers &req)
{
    //init request buffer
    CLEAR(req);
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    xioctl(fd, VIDIOC_REQBUFS, &req);
}
