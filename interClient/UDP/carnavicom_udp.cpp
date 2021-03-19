/**
* @file     carnavicom_udp.cpp
* @brief    UDP 연결을 위한 파일
* @details  UDP 연결을 위한 함수를 정의
* @date     2021-01-11
* @version  0.0.1
*/
#include "carnavicom_udp.h"

CarnavicomUDP::CarnavicomUDP()
{
    g_UDP_Multicast = false;
    g_localIP = "192.168.100.99";
    g_LiDAR_IP_01 = "192.168.100.1";
    g_UDP_PORT = 5000;

    memset(g_intput_buffer, 0, BUFFER_SIZE);

    g_checkLiDAR_IP = false;
    g_inputLidar_IP = "192.168.100.92";
}

/** @brief UDP 멀티캐스트 설정 여부 선택
*  @date 2021.01.11
*  @return void
*  @param multicast_IP  : 설정할 멀티캐스트 IP
*  @param checked       : 멀티캐스트 설정 여부(true, false)
*/
void CarnavicomUDP::setMulticast(std::string multicast_IP, bool checked)
{
    g_UDP_Multicast = checked;
    if(checked)
    {
        g_multicst_IP = multicast_IP;
    }
}

/** @brief UDP 초기화 함수
*  @date 2021.01.11
*  @return void
*  @param IP    : 로컬(컴퓨터) IP 지정
*  @param port  : UDP 통신을 위한 port 설정
*/
void CarnavicomUDP::InitUDP(std::string IP, int port)
{
    g_localIP = IP;
    g_UDP_PORT = port;

    memset(&g_udpAddr, 0, sizeof(struct sockaddr_in));

    g_udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    if(g_udpSocket == -1)
    {
        perror("UDP Socket Failed");
        exit(1);
    }

    //unicast
    if(!g_UDP_Multicast)
    {
        g_udpAddr.sin_family = AF_INET;
        g_udpAddr.sin_addr.s_addr = inet_addr(IP.c_str());
        g_udpAddr.sin_port = htons(port);
    }
    else    //multicast
    {
        g_udpAddr.sin_family = AF_INET;
        g_udpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        g_udpAddr.sin_port = htons(port);

        multicastAddress.imr_multiaddr.s_addr = inet_addr(g_multicst_IP.c_str());
        multicastAddress.imr_interface.s_addr = inet_addr(IP.c_str());

        setsockopt(g_udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&multicastAddress, sizeof(multicastAddress));
    }
}

/** @brief UDP 연결 함수
*  @date 2021.01.11
*  @return bool
*  @param NULL
*/
bool CarnavicomUDP::connect()
{
    if(bind(g_udpSocket, (struct sockaddr*)&g_udpAddr, sizeof(g_udpAddr)) == -1)
    {
        perror("[ERROR][UDP] Binding error\n");
        return false;
    }
    else
    {
        printf("[UDP]CONNECTED\n");
        return true;
    }
}

/** @brief UDP 연결 끊기 함수
*  @date 2021.01.11
*  @return  int
*  @param   NULL
*/
int CarnavicomUDP::disconnect()
{
    printf("[UDP][DISCONNECT]\n");
    return close(g_udpSocket);
}

/** @brief UDP 데이터 수신 함수
*  @date 2021.01.11
*  @return  std::vector<u_char>
*  @param   NULL
*/
std::vector<u_char> CarnavicomUDP::getData()
{
    memset(&g_SenderAddr, 0, sizeof(struct sockaddr_in));
    socklen_t lidarAddress_length = sizeof(g_SenderAddr);

    std::vector<u_char> output;

    // int size = recvfrom(g_udpSocket, output->data(), 300000, 0, (struct sockaddr*)&g_SenderAddr, &lidarAddress_length);
    int size = recvfrom(g_udpSocket, g_intput_buffer, BUFFER_SIZE, 0, (struct sockaddr*)&g_SenderAddr, &lidarAddress_length);
    if(size > 0)
    {
        if(g_checkLiDAR_IP)
        {
            if(strcmp(g_inputLidar_IP.c_str(), inet_ntoa(g_SenderAddr.sin_addr)) == 0)
            {
                for(int i=0; i<size; i++)
                {
                    output.push_back(g_intput_buffer[i]);
                }
            }
            else
            {
                printf("[UDP][input] : input IP is %s\n", inet_ntoa(g_SenderAddr.sin_addr));
                printf("[UDP][input] : set LiDAR IP is %s\n", g_inputLidar_IP.c_str());
                output.clear();
            }
        }
        else
        {            
            for(int i=0; i<size; i++)
            {
                output.push_back(g_intput_buffer[i]);
            }
        }
    }
    else
    {
        output.clear();
    }

    g_getLidar_IP = inet_ntoa(g_SenderAddr.sin_addr);
    
    return output;
}

/** @brief UDP 데이터 송신 함수
*  @date 2021.01.11
*  @return  void
*  @param   send_data : UDP로 전송할 데이터
*/
void CarnavicomUDP::sendData(std::vector<u_char> send_data)
{
    int res;
    if(send_data.size() <= MAX_UDP_SIZE)
    {
        if(sendto(g_udpSocket, send_data.data(), send_data.size(), 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr)) == -1)
        {
            perror("UDP send Error");
        }
    }
    else
    {
#if 1
        int n = 0;
        int remainSize = send_data.size();
        // u_char* dumBuff = (u_char*)malloc(MAX_UDP_SIZE);
        u_char sendbuffer[MAX_UDP_SIZE];
        memset(sendbuffer, 0, MAX_UDP_SIZE);
        
        for(int i=0; i<send_data.size(); i++)
        {
            sendbuffer[n] = send_data[i];
            n++;
            if(n == MAX_UDP_SIZE && remainSize > MAX_UDP_SIZE)
            {                
                if(sendto(g_udpSocket, sendbuffer, MAX_UDP_SIZE, 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr)) == -1)
                {
                    perror("UDP send Error");
                }
                remainSize -= n; 
                // printf("[UDP][outer Server]remain data %d, %d\n", i, remainSize);
                n = 0;
            }
            else
            {
                if(remainSize == n)
                {
                    // printf("all data send %d\n", remainSize);
                    if(sendto(g_udpSocket, sendbuffer, remainSize, 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr)) == -1)
                    {
                        perror("UDP send Error");
                    }
                }                
            }
        }
#else
        u_char sendBuffer_[15][MAX_UDP_SIZE] = {0,};
        int remainSize = send_data.size() - MAX_UDP_SIZE *14;
        for(int i=0; i<MAX_UDP_SIZE; i++)
        {
            sendBuffer_[0][i] = send_data[i];
            sendBuffer_[1][i] = send_data[i + MAX_UDP_SIZE];
            sendBuffer_[2][i] = send_data[i + MAX_UDP_SIZE *2];
            sendBuffer_[3][i] = send_data[i + MAX_UDP_SIZE *3];
            sendBuffer_[4][i] = send_data[i + MAX_UDP_SIZE *4];
            sendBuffer_[5][i] = send_data[i + MAX_UDP_SIZE *5];
            sendBuffer_[6][i] = send_data[i + MAX_UDP_SIZE *6];
            sendBuffer_[7][i] = send_data[i + MAX_UDP_SIZE *7];
            sendBuffer_[8][i] = send_data[i + MAX_UDP_SIZE *8];
            sendBuffer_[9][i] = send_data[i + MAX_UDP_SIZE *9];
            sendBuffer_[10][i] = send_data[i + MAX_UDP_SIZE *10];
            sendBuffer_[11][i] = send_data[i + MAX_UDP_SIZE *11];
            sendBuffer_[12][i] = send_data[i + MAX_UDP_SIZE *12];
            sendBuffer_[13][i] = send_data[i + MAX_UDP_SIZE *13];

            if(i < remainSize)
                sendBuffer_[14][i] = send_data[i + MAX_UDP_SIZE *14];//remain
        }

        for(int i=0; i<14; i++)
        {
            int res = sendto(g_udpSocket, sendBuffer_[i], MAX_UDP_SIZE, 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr));
            if( res == -1)
            {
                perror("UDP send Error");
            }
            // printf("data udp send %d : %d \n", i, res);
        }
            
        int res = sendto(g_udpSocket, sendBuffer_[14], remainSize, 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr));
            
        if( res == -1)        
        {
            perror("UDP send Error");
        }
        // printf("data udp send 14 : %d \n", res);
#endif
    }

    printf("[udp][send][end]");
}

void CarnavicomUDP::sendData(u_char data[], int size)
{
    {
        if(sendto(g_udpSocket, data, size, 0, (struct sockaddr*)& g_udpAddr, sizeof(g_udpAddr)) == -1)
        {
            perror("UDP send Error");
        }
    }
}

/** @brief 입력되는 라이다가 여러개일 경우 라이다 위치를 확인하기 위한 IP 세팅
*  @date 2021.01.12
*  @return void
*  @param lidarIP :타겟 라이다의 IP
*/
void CarnavicomUDP::setLidarIP(std::string lidarIP)
{
    g_inputLidar_IP = lidarIP;
    g_checkLiDAR_IP = true;
}

std::string CarnavicomUDP::getLidarIP()
{
    return g_getLidar_IP;
}