#define opencv 0

#include "v4l2/usbvideo.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../interCommu/interCommunication.h"
#include "../include/header.h"

#if opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

#define g_width         1280
#define g_height        720

#define opencv  0

void delay(double ms)
{
    double delay=0;
    clock_t start = clock();
    clock_t end = 0;
    do
    {
        end = clock();
        delay = static_cast<double>(end - start);
        // printf("delay : %.3f\n", (double)(delay/CLOCKS_PER_SEC * 1000));
    }while((double)(delay/CLOCKS_PER_SEC * 1000) < ms);
}

bool delay_ms(double ms)
{
    double delay=0;
    clock_t start = clock();
    clock_t end = 0;
    do
    {
        end = clock();
        delay = static_cast<double>(end - start);
        // printf("delay : %.3f\n", (double)(delay/CLOCKS_PER_SEC * 1000));
    }while((double)(delay/CLOCKS_PER_SEC * 1000) < ms);

    return true;
}

int16_t double2Hex(double val)
{
    int buf = static_cast<int>(val * 100);
    return static_cast<int16_t>(buf);
}

std::string warningHelp()
{
    std::string output;
    output =    "Command List :     -S/-s : server file setting\n"
                "                   -C/-c : client file setting\n"
                "                   -p/-P : XYZ position setting ( ex) -p 1 1 1 )\n"
                "                   -R/-r : roll/pitch/yaw setting ( ex) -r 10 0 45 )\n"
                "                   -D/-d : Device Name ( ex) /dev/video0 )"
                "                   -H/-h : Command List \n"
                "                   -wh   : width / height"
                "       ex) lidarAPI -c /tmp/IPC_DCU_client -s /tmp/IPC_DCU";
    return output;
}


int main(int argc, char* argv[])
{
    usbVideo *m_video;
    m_video = new usbVideo;

    interCommu *m_clinet;
    m_clinet = new interCommu;
    
    RGBPACKET packet;
    C_SIZE videoBuffer;

    //var. argv
    bool setArgvXYZ = false;
    bool setArgvRolling = false;
    bool setArgvServer = false;
    bool setArgvClient = false;
    bool setArgvSize = false;
    bool setArgvDevice = false;

    char* argvServerName = "null";
    char* argvClientName = "null";
    char* argvDeviceName = "/dev/video0";

    int fd = 0;
    clock_t cl_start = 0;
    clock_t cl_end = 0;
    double cl_result = 0;
    double avrTime = 0;

    int heigth = 480;
    int width = 640;

    //sensor position
    double pos_xyz[3] = {0, 0, 0};   //x,y,z
    double pos_rpy[3] = {0, 0, 0};   //roll, pitch, yaw

    //check argv -- 인자 확인
    if(argc == 0)
    {
        printf("%s\n", warningHelp().c_str());
    }
    else
    {
        for(int i=0; i<argc; i++)
        {
            if(!strcmp(argv[i], "-S") || !strcmp(argv[i], "-s"))
            {
                printf("set Server\n");
                setArgvServer = true;
                argvServerName = argv[i+1];
            }
            else if(!strcmp(argv[i], "-C") || !strcmp(argv[i], "-c"))
            {
                printf("set client\n");
                setArgvClient = true;
                argvClientName = argv[i+1];
            }
            else if(!strcmp(argv[i], "-P") || !strcmp(argv[i], "-p"))
            {
                setArgvXYZ = true;
                pos_xyz[0] = atof(argv[i+1]);
                pos_xyz[1] = atof(argv[i+2]);
                pos_xyz[2] = atof(argv[i+3]);
            }
            else if(!strcmp(argv[i], "-R") || !strcmp(argv[i], "-r"))
            {
                setArgvRolling = true;
                pos_rpy[0] = atof(argv[i+1]);
                pos_rpy[1] = atof(argv[i+2]);
                pos_rpy[2] = atof(argv[i+3]);
            }
            else if(!strcmp(argv[i], "-D") || !strcmp(argv[i], "-d"))
            {
                setArgvDevice = true;
                argvDeviceName = argv[i+1];
            }
            else if(!strcmp(argv[i], "-wh"))
            {
                setArgvSize = true;
                heigth = atoi(argv[i+2]);
                width = atoi(argv[i+1]);
            }
            else if(!strcmp(argv[i], "-H") || !strcmp(argv[i], "-h"))
            {
                printf("%s\n", warningHelp().c_str());
            }
        }
    }

    cl_start = clock();

    m_video->setSize(heigth, width);
    if(setArgvDevice)
    {
        m_video->setDeviceName(argvDeviceName);
    }
    m_video->connect();

    printf("main fd : %d\n", fd);

    int size_koS = 2;
    int size_mode = 1;
    int size_pos = 6;
    int size_roll = 6;
    int size_reserved1 = 1;
    int size_imageSize = 4;
    int size_raw = heigth * width * 3;
    
    int total_size = size_koS + size_mode + size_pos + size_roll
                    + size_reserved1 + size_imageSize + size_raw;
    // memset(total_Data, 0, total_size);

    u_char *total_Data = (u_char*)malloc(sizeof(u_char) * total_size);

    //IPC
    int ipc_clntSocket;
    if(setArgvServer && setArgvClient)
    {
        printf("dara send\n");
        m_clinet->InitClient(ipc_clntSocket, argvClientName);
    }

#if opencv
    cv::Mat m_frame(heigth, width,CV_8UC3, cv::Scalar(255,255,255));
#endif

    sleep(1);

    //input process
    while(1)    //infinite loop
    {
        
        printf("get data\n");
        {        
            // packet = m_video->getData();
            // printf("convert Mat\n");
            // // -------output opencv
            // int cnt = 0;
            // for(int i=0; i<packet.height; i++)
            // {
            //     for(int j=0; j<packet.width; j++)
            //     {
            //         m_frame.at<cv::Vec3b>(i,j)[0] = packet.blue[cnt];
            //         m_frame.at<cv::Vec3b>(i,j)[1] = packet.green[cnt];
            //         m_frame.at<cv::Vec3b>(i,j)[2] = packet.red[cnt];
            //         cnt++;
            //     }
            // }
        }
        {
            videoBuffer = m_video->getDataPtr();
            int cnt = 0;
            printf("convert Mat\n");
            total_Data[0] = 'C';
            total_Data[1] = 'A';
            total_Data[2] = CARNAVICOM::TRANSMISSION_MODE::RGB_MODE;  //rgb
            //pos X,Y,Z
            int16_t pos_x = double2Hex(pos_xyz[0]);
            int16_t pos_y = double2Hex(pos_xyz[1]);
            int16_t pos_z = double2Hex(pos_xyz[2]);
            total_Data[3] = ((pos_x >> 8) & 0xFF);
            total_Data[4] = (pos_x & 0xFF);
            total_Data[5] = ((pos_y >> 8) & 0xFF);
            total_Data[6] = (pos_y & 0xFF);
            total_Data[7] = ((pos_z >> 8) & 0xFF);
            total_Data[8] = (pos_z & 0xFF);

            //4. position roll, pitch, yaw
            total_Data[9] = ((static_cast<int16_t>(pos_rpy[0]*100) >> 8) & 0xFF);
            total_Data[10] = ((static_cast<int16_t>(pos_rpy[0]*100)) & 0xFF);
            total_Data[11] = ((static_cast<int16_t>(pos_rpy[1]*100) >> 8) & 0xFF);
            total_Data[12] = ((static_cast<int16_t>(pos_rpy[1]*100)) & 0xFF);
            total_Data[13] = ((static_cast<int16_t>(pos_rpy[2]*100) >> 8) & 0xFF);
            total_Data[14] = ((static_cast<int16_t>(pos_rpy[2]*100)) & 0xFF);

            total_Data[15] = RESERVED;
            //image size
            //width
            int16_t widthHex = static_cast<int16_t>(width);
            total_Data[16] = ((widthHex >> 8) & 0xFF);
            total_Data[17] = widthHex & 0xFF;
            //height
            int16_t heigthHex = static_cast<int16_t>(heigth);
            total_Data[18] = ((heigthHex >> 8) & 0xFF);
            total_Data[19] = heigthHex & 0xFF;
            for(int i=0; i<videoBuffer.size; i++)
            {
                total_Data[20 + i] = videoBuffer.bufferPtr[i];
            }
            printf("check hex value : %d %d %d\n", total_Data[0], total_Data[19], total_Data[total_size-1]);
#if opencv
            cnt = 0;
            for(int i=0; i<videoBuffer.height; i++)
            {
                for(int j=0; j<videoBuffer.width; j++)
                {
                    m_frame.at<cv::Vec3b>(i,j)[0] = videoBuffer.bufferPtr[cnt];
                    m_frame.at<cv::Vec3b>(i,j)[1] = videoBuffer.bufferPtr[cnt+1];
                    m_frame.at<cv::Vec3b>(i,j)[2] = videoBuffer.bufferPtr[cnt+2];
                    cnt+=3;
                }
                // printf("cnt : %d\n", cnt);
            }
#endif
        }
        //check total data
        // for(int i=0; i<18; i++)
        // {
        //     printf("[%d] : %d\n", i, total_Data[i]);
        // }

#if opencv
        printf("check pixel %d,%d count :%d\n",m_frame.rows, m_frame.cols, m_frame.rows * m_frame.cols);
        cv::namedWindow("window",CV_WINDOW_AUTOSIZE);
        cv::imshow("window", m_frame);
        cv::waitKey(1);
#endif
        //--------------------------------------------------

        //----IPC--------
        if(setArgvServer && setArgvClient)
        {
            printf("send data\n");
            m_clinet->sendData_Camera(ipc_clntSocket, total_Data, total_size, argvServerName);
        }
        //---------------
        cl_result = static_cast<double>(clock() - cl_start);

        if(avrTime != 0)
        {
            avrTime = (avrTime + cl_result)/2;
        }
        else
            avrTime = cl_result;
        printf("[main][image get processing time] : %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));
        printf("[main][average processing time] : %.3f[ms]\n", (double)(avrTime/CLOCKS_PER_SEC * 1000));
        cl_start = clock();
        // usleep(30000);
        // sleep(1);
        // cv::waitKey(30);
        // delay(10);
        double de_time = static_cast<double>(clock() - cl_start);
        printf("[main][delay time] : %.3f[ms]\n", (double)(de_time/CLOCKS_PER_SEC * 1000));
    }
    sleep(20);

    m_video->close();

    return 0;
}