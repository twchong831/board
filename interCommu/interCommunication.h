#ifndef INTERCOMMUNICATION_H
#define INTERCOMMUNICATION_H

#include "../include/header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

/**
* @file     interCommunication.h
* @brief    내부 프로세스 간의 UDP 통신을 위한 헤더 파일
* @details  내부 프로세스 간의 UDP 통신을 위한 함수 선언
* @date     2021-02-08
* @version  0.0.1
*/

class interCommu
{
public:
    interCommu();

    bool InitServer(const int argc, char** argv);
    bool InitServer(const std::vector<char*> name);
    void InitClient(int &socket_, char* path);

    std::vector<u_char> getData(const int socket_);
    std::vector<IPC_PACKET> getData_fromClnt();
    void sendData(const int socket_, std::vector<u_char> data, char* path);
    void sendData_array(const int socket_, u_char data[], int cnt, char* path);
    void sendData_Camera(const int socket_, u_char data[], int cnt, char* path);

    void closeServer();

private:
    //function
    void initSocket(int &socket_, const char* file);
    
    //variable.
    int g_server_socket_[4];
    int g_clnt_socket;
    int g_clnt_cnt;
    struct sockaddr_un g_localAddr_;
    struct sockaddr_un g_clntAddr_;
    struct sockaddr_un g_senderAddr_;

    //camera
    int g_cameraBufferNum;
};

#endif // INTERCOMMUNICATION_H