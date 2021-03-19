#ifndef carnavicomLidarProcessor_H
#define carnavicomLidarProcessor_H

/**
* @file     carnavicomLidarProcessor.h
* @brief    카네비컴 라이다 데이터를 처리하기 위한 프로세서
* @details  카네비컴 라이다의 데이터 연결, 입력, 파싱, 출력을 처리하기 위한 파일
* @date     2021-01-11
* @version  0.0.1
*/

#include "../../include/header.h"
#include "../UDP/carnavicom_udp.h"
#include "../LiDAR/lidarParser.h"
#include <cmath>

/**
*   @brief LiDAR 데이터 통합 프로세서
*   @details UDP 설정, 데이터 입력, 파싱까지 처리하기 위한 class
*   @author 정태원, twchong@carnavi.com
*   @date 2021.01.11
*   @version 0.0.1
*/

namespace LidarPROCESSOR
{
    enum RESPONSE
    {
        NONE,           //0
        ERROR_NETWORK,  //1
        ERROR_DATA      //2
    };
}

class carnavicomLidarProcessor
{
public:
    carnavicomLidarProcessor();

    void processor();
    int processor_oneTime();

    //lidar--->central processor
    void setLocalIP(std::string IP);
    void setPORT(int port);

    //central processor ----> server
    void setServerIP(std::string IP);
    void setServerPORT(int port);

    //transimisson mode select
    void setTransmissionMode(int mode);
    void sendServer(bool checked, int mode);

    //get(send) lidar data
    size_t getRawDataSize();
    std::vector<u_char> getRawData();
    std::vector<u_char> getData();

    int getTransmissionMode();

    //set lidar IP
    void setLiDAR_IP(std::string lidarIP);

    //set processor name
    void setProcessName(std::string name);
    std::string getProcessName();

    std::string getLidarIP();

    int getLiDARModel();

private:
    //function    

    //variable
    CarnavicomUDP *m_UDP;
    CarnavicomUDP *m_Server_UDP;

    liDAR_Protocol *m_datagram;
    lidarParser *m_dataParser;

    std::string g_localIP;
    int g_PORT;

    std::string g_serverIP;
    int g_serverPORT;
    bool g_setServer;

    std::string g_processName;

    //transmission mode
    int g_transmissionMode;

    //lidar IP
    std::string LidarsIP;

    int testCNT;
};

#endif // carnavicomLidarProcessor_H
