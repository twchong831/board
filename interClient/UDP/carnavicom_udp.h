#ifndef CARNAVICOMUDP_H
#define CARNAVICOMUDP_H

/**
* @file     carnavicom_udp.h
* @brief    UDP 연결을 위한 파일
* @details  UDP 연결을 위한 함수를 선언
* @date     2021-01-11
* @version  0.0.1
*/

#include "../../include/header.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 40000

/**
*   @brief LiDAR 센서와의 UDP 통신
*   @details LiDAR 센서와의 UDP 통신을 위한 초기화 및 데이터 입력 확인
*   @author 정태원, twchong@carnavi.com
*   @date 2021.01.11
*   @version 0.0.1
*/
class CarnavicomUDP
{
public:
    CarnavicomUDP();

    //from LidAR sensor
    void setMulticast(std::string multicast_IP, bool checked=false);
    void InitUDP(std::string IP, int port);

    bool connect();
    int disconnect();
    std::vector<u_char> getData();

    void sendData(std::vector<u_char> send_data);

    void sendData(u_char data[], int size);

    void setLidarIP(std::string lidarIP);
    std::string getLidarIP();

private:
    //lidar -> pc
    struct sockaddr_in g_udpAddr;
    struct sockaddr_in g_SenderAddr;
    int g_udpSocket;

    struct ip_mreq multicastAddress;

    std::string g_localIP;
    std::string g_multicst_IP;
    std::string g_LiDAR_IP_01;
    int g_UDP_PORT;
    bool g_UDP_Multicast;

    u_char g_intput_buffer[BUFFER_SIZE];

    //lidar's IP
    std::string g_inputLidar_IP;
    bool g_checkLiDAR_IP;

    std::string g_getLidar_IP;
};

#endif // CARNAVICOMUDP_H
