/**
* @file     carnavicomLidarProcessor.cpp
* @brief    카네비컴 라이다 데이터를 처리하기 위한 프로세서
* @details  카네비컴 라이다의 데이터 연결, 입력, 파싱, 출력을 처리하기 함수를 정의한 파일
* @date     2021-01-11
* @version  0.0.1
*/
#include "carnavicomProcessor.h"
#include <time.h>
#define CHECK_processingTime    0

carnavicomLidarProcessor::carnavicomLidarProcessor()
{
    m_UDP = new CarnavicomUDP();
    m_Server_UDP = new CarnavicomUDP();
    m_datagram = new liDAR_Protocol();
    m_dataParser = new lidarParser();

    g_localIP = "192.168.100.88";
    g_PORT = 5000;

    g_setServer = false;

    g_serverIP = "192.168.100.90";
    g_serverPORT = 3000;

    g_transmissionMode = CARNAVICOM::TRANSMISSION_MODE::Length_MODE;

    testCNT = 0;
    LidarsIP.clear();
    g_processName = "NONE";
}

/** @brief LiDAR 데이터 통합 처리를 위한 함수(무한)
*  @date 2021.01.11
*  @return void
*  @param NULL
*/
void carnavicomLidarProcessor::processor()
{
    // printf("\n-----Carnavicom LiDAR Processor----\n");

#if CHECK_processingTime
    clock_t cl_start = 0;
    clock_t cl_end = 0;
    double cl_result = 0;
#endif

    m_UDP->InitUDP(g_localIP, g_PORT);

    std::vector<u_char> data;

    if(g_setServer) //usinf FUNC. send Server
    {
        m_Server_UDP->InitUDP(g_serverIP, g_serverPORT);
        // printf("[processor][server UDP init]\n");
    }

    if(m_UDP->connect())
    {
        // printf("connect success\n");
        m_dataParser->setParsingMode(g_transmissionMode);

#if CHECK_processingTime
        cl_start = clock();
#endif
        while(1)
        {
            data = m_UDP->getData();
            // printf("\n from UDP processor input data check size : %d\n", data.size());
            testCNT++;

            if(m_dataParser->setData(data))
            {
                size_t lowSize = getRawDataSize();
                // printf("data over : %d\n", lowSize);
                sendServer(g_setServer, 0);    //usinf FUNC. send Server
#if CHECK_processingTime
                cl_end = clock();
                cl_result = static_cast<double>(cl_end - cl_start);
                // printf("[lidar processor][processing time] %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));
                cl_start = clock();
                // printf("check func cnt :%d\n", testCNT);
                testCNT = 0;
#endif
            }
        }
    }
    else
    {
        printf("connect fail\n");
    }
}

/** @brief  LiDAR 데이터 통합 처리를 위한 함수(한번)
*  @date 2021.01.11
*  @return bool
*  @param NULL
*/
int carnavicomLidarProcessor::processor_oneTime()
{
#if CHECK_processingTime
    clock_t cl_start = 0;
    clock_t cl_end = 0;
    double cl_result = 0;
#endif
    bool udpGetLoop = true; 
    // printf("\n-----Carnavicom LiDAR Processor one time----\n");

    m_UDP->InitUDP(g_localIP, g_PORT);
    if(g_setServer) //usinf FUNC. send Server
    {
        m_Server_UDP->InitUDP(g_serverIP, g_serverPORT);
        // printf("[processor][server UDP init]\n");
    }

    std::vector<u_char> data;

    if(m_UDP->connect())
    {
        // printf("connect success\n");
        m_dataParser->setParsingMode(g_transmissionMode);
        
#if CHECK_processingTime
        cl_start = clock();
#endif

        while(udpGetLoop)
        {
            data = m_UDP->getData();
            // printf("\n from UDP processor input data check size : %d\n", data.size());

            if(data.size() >0)
            {
                if(m_dataParser->setData(data))
                {
                    udpGetLoop = false;
                    size_t lowSize = getRawDataSize();
                    // printf("data over : %d\n", lowSize);
#if CHECK_processingTime
                    cl_end = clock();
                    cl_result = static_cast<double>(cl_end - cl_start);
                    printf("[lidar processor][processing time] %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

                    cl_start = clock();
#endif
                }
            }   
        }
        m_UDP->disconnect();

        if(g_setServer) //usinf FUNC. send Server
        {
            sendServer(g_setServer, 1);    //usinf FUNC. send Server
        }

        return LidarPROCESSOR::RESPONSE::NONE;
    }
    else
    {
        printf("connect fail\n");
        return LidarPROCESSOR::RESPONSE::ERROR_NETWORK;
    }
}

/** @brief  UDP 통신을 위한 local IP 지정
*  @date 2021.01.11
*  @return void
*  @param IP :설정할 IP 지정
*/
void carnavicomLidarProcessor::setLocalIP(std::string IP)
{
    g_localIP = IP;
}

/** @brief  UDP 통신을 위한 port 설정
*  @date 2021.01.11
*  @return void
*  @param port : UDP 통신을 위한 포트 설정
*/
void carnavicomLidarProcessor::setPORT(int port)
{
    g_PORT = port;
}

/** @brief  UDP 통신(서버)을 위한 IP 설정
*  @date 2021.01.11
*  @return void
*  @param IP :설정할 IP 지정
*/
void carnavicomLidarProcessor::setServerIP(std::string IP)
{
    g_serverIP = IP;
    g_setServer = true;
}

/** @brief  UDP 통신(서버)을 위한 port 설정
*  @date 2021.01.11
*  @return void
*  @param port : UDP 통신을 위한 포트 설정
*/
void carnavicomLidarProcessor::setServerPORT(int port)
{
    g_serverPORT = port;
}

/** @brief  프로세서에서 수집한 lidar 데이터 송신 방법 설정
*  @date 2021.01.12
*  @return void
*  @param mode : mode 선택
*/
void carnavicomLidarProcessor::setTransmissionMode(int mode)
{
    g_transmissionMode = mode;
}

/** @brief low 데이터 크기 확인 함수
*  @date 2021.01.11
*  @return size_t
*  @param NULL
*/
size_t carnavicomLidarProcessor::getRawDataSize()
{
    return m_dataParser->getRawDataSize();
}

/** @brief low 데이터 획득 함수
*  @date 2021.01.11
*  @return std::vector<u_char>
*  @param NULL
*/
std::vector<u_char> carnavicomLidarProcessor::getRawData()
{
    return m_dataParser->getRawData();
}

std::vector<u_char> carnavicomLidarProcessor::getData()
{
    return m_dataParser->getParsingData();
}

/** @brief 프로세서에서 수집한 lidar 데이터 송신 방법 확인
*  @date 2021.01.12
*  @return int
*  @param NULL
*/
int carnavicomLidarProcessor::getTransmissionMode()
{
    return g_transmissionMode;
}

void carnavicomLidarProcessor::sendServer(bool checked, int mode)
{
    if(checked) //usinf FUNC. send Server
    {
        // printf("[processor][server send on]\n");
        // if(m_Server_UDP->connect())
        {
            std::vector<u_char> data;
            switch (g_transmissionMode)
            {
                case CARNAVICOM::TRANSMISSION_MODE::RAW_MODE :
                    data = getRawData();
                    break;
                case CARNAVICOM::TRANSMISSION_MODE::Length_MODE :
                    // data = g
                    break;
                case CARNAVICOM::TRANSMISSION_MODE::OBJ_MODE :
                    break;
                case CARNAVICOM::TRANSMISSION_MODE::XYZ_MODE :
                    break;
            }
            // printf("[processor][send]: model : %s \n", m_dataParser->getLidarModel().c_str());
            // printf("[processor][send]: transmission mode : %d \n", getTransmissionMode());
            // printf("[processor][send]: check data size : %d \n", data.size());
            m_Server_UDP->sendData(data);

            if(mode == 1)
            {
                m_Server_UDP->disconnect();
            }
        }
    }
}

/** @brief 입력되는 라이다가 여러개일 경우 라이다 위치를 확인하기 위한 IP 세팅
*  @date 2021.01.12
*  @return void
*  @param lidarIP :타겟 라이다의 IP
*/
void carnavicomLidarProcessor::setLiDAR_IP(std::string lidarIP)
{
    m_UDP->setLidarIP(lidarIP);
    LidarsIP = lidarIP;
}

void carnavicomLidarProcessor::setProcessName(std::string name)
{
    g_processName = name;
}

std::string carnavicomLidarProcessor::getProcessName()
{
    return g_processName;
}

/** @brief 데이터를 송신하는 라이다의 IP를 외부로 공유
*  @date 2021.02.10
*  @return std::string : 라이다 IP
*  @param void
*/
std::string carnavicomLidarProcessor::getLidarIP()
{
    return m_UDP->getLidarIP();
}

int carnavicomLidarProcessor::getLiDARModel()
{
    return m_dataParser->getLidarModel_();
}