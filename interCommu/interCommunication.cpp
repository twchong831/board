#include "interCommunication.h"
/**
* @file     interCommunication.h
* @brief    내부 프로세스 간의 통신을 위한 헤더 파일
* @details  내부 프로세스 간의 통신을 위한 함수 선언
* @date     2021-02-08
* @version  0.0.1
*/
interCommu::interCommu()
{
    memset(&g_server_socket_, 0, 4);
    memset(&g_localAddr_, NULL, sizeof(sockaddr_un));
    memset(&g_senderAddr_, NULL, sizeof(sockaddr_un));
    memset(&g_clntAddr_, NULL, sizeof(sockaddr_un));

    g_clnt_cnt = 0;
}

/** @brief  IPC 서버 초기화(UDP)
*  @date 2021.02.10
*  @return bool : 서버 소켓 생성 여부
*  @param argc : 입력 변수 개수
*  @param argv : 입력 변수 - 서버로 할당하기 위한 파일명 선언 ex) /tmp/IPC_server
*/
bool interCommu::InitServer(const int argc, char** argv)
{
    g_clnt_cnt = argc-1;

    if(g_clnt_cnt > 0)
    {
        for(int i=1; i<argc; i++)
        {
            printf("path check : %s\n", argv[i]);
            initSocket(g_server_socket_[i-1], argv[i]);
            g_localAddr_.sun_family = AF_UNIX;
            strcpy(g_localAddr_.sun_path, argv[i]);

            if(bind(g_server_socket_[i-1], (struct sockaddr*)&g_localAddr_, sizeof(g_localAddr_)) == -1)
            {
                perror("binding Error\n");
                exit(0);
            }
        }
        return true;
    }
    else
    {
        printf("client Information not Get!\n");
        return false;
    }
}

bool interCommu::InitServer(const std::vector<char*> name)
{
    g_clnt_cnt = name.size();

    if(g_clnt_cnt > 0)
    {
        for(size_t i=0; i<name.size(); i++)
        {
            printf("path check : %s\n", name[i]);
            initSocket(g_server_socket_[i], name[i]);
            g_localAddr_.sun_family = AF_UNIX;
            strcpy(g_localAddr_.sun_path, name[i]);

            if(bind(g_server_socket_[i], (struct sockaddr*)&g_localAddr_, sizeof(g_localAddr_)) == -1)
            {
                perror("binding Error\n");
                exit(0);
            }
        }
        return true;
    }
    else
    {
        printf("client Information not Get!\n");
        return false;
    }
}

/** @brief  IPC 클라이언트 초기화(UDP)
*  @date 2021.02.10
*  @return NULL
*  @param socket_ : 할당할 소켓
*  @param path : 클라이언트로 할당하기 위한 파일명 선언 ex) /tmp/IPC_client1 /tmp/IPC_client2
*/
void interCommu::InitClient(int &socket_, char* path)
{
    if(access(path, F_OK) == 0)
    {
        unlink(path);   //해제
    }
    socket_ = socket(AF_UNIX, SOCK_DGRAM, 0);

    if(socket_ == -1)
    {
        perror("server socket is failed\n");
        exit(1);
    }

    g_clntAddr_.sun_family = AF_UNIX;
    printf("check client path %s\n", path);
    strcpy(g_clntAddr_.sun_path, path);

    if(bind(socket_, (struct sockaddr*)&g_clntAddr_, sizeof(g_clntAddr_)) == -1)
    {
        perror("binding Error");
        exit(0);
    }
}

std::vector<u_char> interCommu::getData(const int socket_)
{//not use
}

/** @brief  IPC 서버가 클라언트로부터 데이터를 UDP로 수신받기 위한 함수
*  @date 2021.02.10
*  @return std::vector<IPC_PACKET> : IPC_PACKET 구조체를 기반으로 하는 벡터로 리턴
*  @param NULL
*/
std::vector<IPC_PACKET> interCommu::getData_fromClnt()
{
    printf("receive data %d \n", g_clnt_cnt);
    std::vector<IPC_PACKET> allBuffer;
    IPC_PACKET buffer;
    u_char data[70000];
    socklen_t clnt_len;
    struct sockaddr_un senderAddr;
    memset(&senderAddr, 0, sizeof(senderAddr));

    //flag camera data input
    bool setCamera = false;

    for(int j=0; j<g_clnt_cnt; j++)
    {
        memset(&data, 0, 70000);
        size_t size = recvfrom(g_server_socket_[j], &data, 70000, 0, (struct sockaddr *)&senderAddr, &clnt_len);
        // printf("[ipc][server][receive] CNT : %d\n", size);
        if(data[0] == 'L')  //lidar Sensor
        {
            printf("LiDAR DAta input from client\n");
            buffer.address = senderAddr;
            buffer.rawData.resize(size);        
            for(int i=0; i<size; i++)
            {
                buffer.rawData[i] = data[i];
            }

            //----rec data IP check
            std::string ip = std::to_string((int)buffer.rawData[buffer.rawData.size()-4])
                            + "." + std::to_string((int)buffer.rawData[buffer.rawData.size()-3])
                            + "." + std::to_string((int)buffer.rawData[buffer.rawData.size()-2])
                            + "." + std::to_string((int)buffer.rawData[buffer.rawData.size()-1]);
            //erase IP  information
            buffer.rawData.pop_back();
            buffer.rawData.pop_back();
            buffer.rawData.pop_back();
            buffer.rawData.pop_back();

            buffer.lidarIP = ip;
            allBuffer.push_back(buffer);
        }
        else if(data[0] == 'C' && data[1] == 'A')
        {
            printf("camera DAta input from client\n");
            buffer.address = senderAddr;
            // uint w_h = data[16];
            // uint w_l = data[17];
            // uint h_h = data[18];
            // uint h_l = data[19];
            // int imagebufferWidth = w_h * 256 + w_l;
            // int imagebufferHeight = h_h * 256 + h_l;
            // buffer.rawData.resize(19 + (imagebufferWidth * imagebufferHeight * 3));

            setCamera = true;
            g_cameraBufferNum = 0;
            printf("camera input %d \n", size);
        }

        if(setCamera)
        {
            buffer.rawData.clear();
            buffer.rawData.resize(921620);
            if(data[0] == 'C' && data[1] == 'A')
            {
                for(int i=0; i<size; i++)
                {
                    buffer.rawData[g_cameraBufferNum] = (data[i]);
                    g_cameraBufferNum++;
                }
                // printf("check data size[midium] : %d\n", buffer.rawData.size());
            }

            while(setCamera)
            {
                memset(&data, 0, 70000);
                size_t ssize = recvfrom(g_server_socket_[j], &data, 70000, 0, (struct sockaddr *)&senderAddr, &clnt_len);
                // printf("camera input continue %d\n", ssize);
                for(int i=0; i<ssize; i++)
                {
                    buffer.rawData[g_cameraBufferNum] = (data[i]);
                    g_cameraBufferNum++;
                }

                // printf("check data size[midium] : %d\n", buffer.rawData.size());

                // if(ssize < MAX_UDP_SIZE)
                if(ssize == 25620)
                {
                    setCamera = false;
                    // printf("check hex value %d %d\n", buffer.rawData[0], buffer.rawData[19]);
                    // printf("check data size : %d\n", buffer.rawData.size());
                }
            }
            allBuffer.push_back(buffer);
        }

    }
    return allBuffer;
}

/** @brief  IPC 클라이언트가 서버로 데이터를 송신하기 위한 함수
*  @date 2021.02.10
*  @return NULL
*  @param socket_ : 할당한 소켓
*  @param data : 전송할 데이터
*  @param path : 서버의 파일명 입력 ex) /tmp/IPC_server
*/
void interCommu::sendData(const int socket_, std::vector<u_char> data, char* path)
{
    struct sockaddr_un servAddr;
    memset(&servAddr, NULL, sizeof(servAddr));
    servAddr.sun_family = AF_UNIX;
    // printf("check client socket_ %d\n", socket_);
    // printf("check client path %s\n", path);
    strcpy(servAddr.sun_path, path);

    // printf("---check IPC send data------\n");
    // printf("----size : %d------------\n", data.size());

    u_char sendbuffer[data.size()];
    size_t dataSize = data.size();
    for(int i=0; i<dataSize; i++)
    {
        sendbuffer[i] = data[i];
    }

    printf("--sendto : addr : %s\n", servAddr.sun_path);

    if(sendto(socket_, &sendbuffer, dataSize, 0, 
        (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
    {        
        perror("send error sendto ");
        sleep(2);
    }
    else
    {
        // printf("send success\n");
    }
}

/** @brief  IPC 클라이언트가 서버로 데이터를 송신하기 위한 함수
*  @date 2021.03.15
*  @return NULL
*  @param socket_ : 할당한 소켓
*  @param data  : 전송할 데이터
*  @param cnt   : 전송할 데이터의 크기
*  @param path  : 서버의 파일명 입력 ex) /tmp/IPC_server
*/
void interCommu::sendData_array(const int socket_, u_char data[], int cnt, char* path)
{
    struct sockaddr_un servAddr;
    memset(&servAddr, NULL, sizeof(servAddr));
    servAddr.sun_family = AF_UNIX;
    strcpy(servAddr.sun_path, path);

    // printf("--sendto : addr : %s\n", servAddr.sun_path);
    printf("[ipc] check size : %d\n", cnt);
    printf("[ipc]check server : %s\n", path);
    if(cnt <= MAX_UDP_SIZE) //UDP 최대 전송 패킷 수를 넘지 않을 경우
    {
        u_char sendbuffer[cnt];
        memset(sendbuffer, 0, cnt);

        for(int i=0; i<cnt; i++)
        {
            sendbuffer[i] = data[i];
        }
        int res = sendto(socket_, &sendbuffer, cnt, 0, 
            (struct sockaddr*)&servAddr, sizeof(servAddr));
        printf("[ipc][udp response] : %d\n" , res);
        if(res == -1)
        {        
            perror("send error sendto ");
            sleep(1);
        }
        else
        {
            // printf("IPC %s send success\n", servAddr.sun_path);
        }
    }
    else    //UDP 최대 전송 패킷 수를 넘을 경우 -- 카메라
    {
        int n = 0;
        int remainSize = cnt;
        // u_char* dumBuff = (u_char*)malloc(MAX_UDP_SIZE);
        u_char sendbuffer[MAX_UDP_SIZE];
        memset(sendbuffer, 0, MAX_UDP_SIZE);
        
        for(int i=0; i<cnt; i++)
        {
            sendbuffer[n] = data[i];
            n++;
            if(n == MAX_UDP_SIZE && remainSize > MAX_UDP_SIZE)
            {                
                if(sendto(socket_, &sendbuffer, MAX_UDP_SIZE, 0, (
                    struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
                {
                    perror("send error");
                }
                remainSize -= n; 
                // printf("remain data %d, %d\n", i, remainSize);
                n = 0;
            }
            else
            {
                if(remainSize == n)
                {
                    // printf("all data send %d\n", remainSize);
                    if(sendto(socket_, &sendbuffer, n, 0, 
                        (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
                    {
                        perror("send error");
                    }
                }                
            }
        }
    }
}

void interCommu::sendData_Camera(const int socket_, u_char data[], int cnt, char* path)
{
    struct sockaddr_un servAddr;
    memset(&servAddr, NULL, sizeof(servAddr));
    servAddr.sun_family = AF_UNIX;
    strcpy(servAddr.sun_path, path);
    
    u_char sendBuffer_[15][MAX_UDP_SIZE] = {0,};
    int remainSize = cnt - MAX_UDP_SIZE *14;
    for(int i=0; i<MAX_UDP_SIZE; i++)
    {
        sendBuffer_[0][i] = data[i];
        sendBuffer_[1][i] = data[i + MAX_UDP_SIZE];
        sendBuffer_[2][i] = data[i + MAX_UDP_SIZE *2];
        sendBuffer_[3][i] = data[i + MAX_UDP_SIZE *3];
        sendBuffer_[4][i] = data[i + MAX_UDP_SIZE *4];
        sendBuffer_[5][i] = data[i + MAX_UDP_SIZE *5];
        sendBuffer_[6][i] = data[i + MAX_UDP_SIZE *6];
        sendBuffer_[7][i] = data[i + MAX_UDP_SIZE *7];
        sendBuffer_[8][i] = data[i + MAX_UDP_SIZE *8];
        sendBuffer_[9][i] = data[i + MAX_UDP_SIZE *9];
        sendBuffer_[10][i] = data[i + MAX_UDP_SIZE *10];
        sendBuffer_[11][i] = data[i + MAX_UDP_SIZE *11];
        sendBuffer_[12][i] = data[i + MAX_UDP_SIZE *12];
        sendBuffer_[13][i] = data[i + MAX_UDP_SIZE *13];

        if(i < remainSize)
            sendBuffer_[14][i] = data[i + MAX_UDP_SIZE *14];//remain
    }

    for(int i=0; i<14; i++)
    {
        int res = sendto(socket_, sendBuffer_[i], MAX_UDP_SIZE, 0, (struct sockaddr*)& servAddr, sizeof(servAddr));
        if( res == -1)
        {
            perror("IPC UDP send Error");            
        }
        // printf("data udp send %d : %d \n", i, res);
    }
            
    int res = sendto(socket_, sendBuffer_[14], remainSize, 0, (struct sockaddr*)& servAddr, sizeof(servAddr));
            
    if( res == -1)        
    {
        perror("IPC UDP send Error");
    }
    // printf("data udp send 14 : %d \n", res);
}

/** @brief  IPC UDP로 소켓을 초기화하기 위한 함수
*  @date 2021.02.10
*  @return NULL
*  @param socket_ : 할당할 소켓
*  @param file : IPC UDP 사용 시 파일이 사용 중일 경우, 이를 unlink
*/
void interCommu::initSocket(int &socket_, const char* file)
{
    socket_ = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(socket_ == -1)
    {
        printf("server socket is failed\n");
        exit(1);
    }

    unlink(file); //해제
}

/** @brief  IPC 소켓을 닫기 위한 함수
*  @date 2021.02.10
*  @return NULL
*  @param NULL
*/
void interCommu::closeServer()
{
    for(int i=0; i<g_clnt_cnt; i++)
    {
        close(g_server_socket_[i]);
    }
}