#include "../interCommu/interCommunication.h"
#include "../interClient/UDP/carnavicom_udp.h"
#include <vector>

#define CAMERA_REMAIN_DATA_SIZA     25630
#define CAMERA_INPUT_POSITION   MAX_UDP_SIZE - 6

std::string warningHelp()
{
    std::string output;
    output =    "Command List :     -S/-s : internal server file setting ( ex) -s /tmp/server)\n"
                "                   -O/-o : outer server UDP setting ( ex) -o 192.168.100.80 5002 )\n"
                "                   -D/-d : Debug mode\n"
                "                   -H/-h : Command List \n";
    return output;
}

bool checkClientOverlap(std::vector<IPC_PACKET> packet)
{
    bool output = false;

    //클라이언트 문자열을 비교하여 중복여부 탐색
    int size = static_cast<int>(packet.size());
    char* clientName[size];
    std::vector<int> overlapCheck;

    if(packet.size() > 1)
    {
        for(size_t i=0; i<packet.size(); i++)
        {
            clientName[i] = packet[i].address.sun_path;
            // printf("check client name %d :%s\n", i, clientName[i]);
        }

        //compare Client name
        for(int i=0; i<size-1; i++)
        {
            for(int j=i+1; j<size; j++)
            {
                if(!strcmp(clientName[i], clientName[j]))
                {
                    //같으면 1 
                    // printf("%s %s\n", clientName[i], clientName[j]);
                    overlapCheck.push_back(1);
                }
                else
                {
                    //다르면 0
                    /* code */
                    overlapCheck.push_back(0);
                }

            }
        }

        int sum = 0;
        for(size_t i=0; i<overlapCheck.size(); i++)
        {
            sum += overlapCheck[i];
        }

        if(sum == 0)
        {
            // printf("sum == 0\n");
            output = false;
        }
        else
        {
            /* code */
            // printf("sum > 0\n");
            output = true;
        }
    }

    return output;
}

int main(int argc, char* argv[])
{
    //내부 통신용
    interCommu *m_server;
    m_server = new interCommu;

    //외부 통신용 
    CarnavicomUDP *m_sender2Out;
    m_sender2Out = new CarnavicomUDP();

    bool setArgvServer = false;
    std::vector<char*> argvServerName;
    argvServerName.clear();

    bool setArgvDebug = false;

    bool setOuputSender = false;
    char* senderIP = "192.168.100.80";
    int senderPort = 5002;

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
                setArgvServer = true;
                argvServerName.push_back(argv[i+1]);
            }
            else if(!strcmp(argv[i], "-O") || !strcmp(argv[i], "-o"))
            {
                setOuputSender = true;
                senderIP = argv[i+1];
                senderPort = atoi(argv[i+2]);
            }
            else if(!strcmp(argv[i], "-D") || !strcmp(argv[i], "-d"))
            {
                setArgvDebug = true;
            }
            else if(!strcmp(argv[i], "-H") || !strcmp(argv[i], "-h"))
            {
                printf("%s\n", warningHelp().c_str());
            }
        }
    }

    if(!setArgvServer)
    {
        perror("Not define IPC Server");
        return 0;
    }

    // m_server->InitServer(argc, argv);
    m_server->InitServer(argvServerName);

    if(setOuputSender)
    {
        printf("Init UDP sender : %s, %d\n", senderIP,senderPort);
        m_sender2Out->InitUDP(senderIP, senderPort);
    }

    sleep(3);
    clock_t cl_start = 0;
    clock_t cl_end = 0;
    double cl_result = 0;
    cl_start = clock();

    std::vector<IPC_PACKET> ipdData;
    std::vector<u_char> allData;
    u_char outputData[15][MAX_UDP_SIZE];
    int16_t checksum;
    int16_t dataSize;

    int Num_otherProtocol = 10;

    while(1)
    {
        dataSize = 0;

        ipdData = m_server->getData_fromClnt();
        if(!checkClientOverlap(ipdData))
        {
            printf("-----[SERVER]----get DATA from Client----\n");
            // int Num_header = 2;
            // int Num_packet = 2;
            // int Num_Size = 2;
            // int Num_checksum = 2;
            // int Num_tail = 2;
        
            for(int i=0; i<ipdData.size(); i++)
            {
                if(ipdData[i].rawData[0] == 'C' && ipdData[i].rawData[1] == 'A')
                {
                    printf("camera data processing\n");
                    dataSize = ipdData[i].rawData.size();
                    for(int j=0; j<15; j++)
                    {
                        memset(outputData[j], 0, MAX_UDP_SIZE);
                    }

                    int remainSize = ipdData[i].rawData.size() + 10 - MAX_UDP_SIZE *14;
                    for(int j=0; j<MAX_UDP_SIZE; j++)
                    {

                        switch(j)
                        {
                        case 0:
                            outputData[0][j] = 0xFA;
                            break;
                        case 1:
                            outputData[0][j] = 0x0A;
                            break;
                        case 2:
                            outputData[0][j] = (static_cast<u_char>(ipdData.size()));
                            break;
                        case 3:
                            outputData[0][j] = (static_cast<u_char>(i));
                            break;
                        case 4:
                            outputData[0][j] = ((dataSize >> 8) & 0xFF);
                            break;
                        case 5:
                            outputData[0][j] = (dataSize & 0xFF);
                            break;
                        default :
                            outputData[0][j] = ipdData[i].rawData[j-6];
                            break;
                        }
                        outputData[1][j] = ipdData[i].rawData[j -6  + MAX_UDP_SIZE];
                        outputData[2][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *2];
                        outputData[3][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *3];
                        outputData[4][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *4];
                        outputData[5][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *5];
                        outputData[6][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *6];
                        outputData[7][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *7];
                        outputData[8][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *8];
                        outputData[9][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *9];
                        outputData[10][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *10];
                        outputData[11][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *11];
                        outputData[12][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *12];
                        outputData[13][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *13];
                        // printf("size 1: %d\n", j);
                        // printf("data all size %d \n", ipdData[i].rawData.size());
                        // printf("remain data? %d %d\n",remainSize, j -6 + MAX_UDP_SIZE *14);

                        if(j < remainSize-4)
                            outputData[14][j] = ipdData[i].rawData[j -6 + MAX_UDP_SIZE *14];//remain
                        // printf("size 2: %d\n", j);
                    }
                    // printf("---1---\n");

                    outputData[14][remainSize - 4] = ((dataSize >> 8) & 0xFF);
                    outputData[14][remainSize - 3] = (dataSize  & 0xFF);
                    outputData[14][remainSize - 2] = 0xF5;
                    outputData[14][remainSize - 1] = 0x05;

                    printf("check size remain : %d\n", remainSize);

                    for(int j=0; j<14; j++)
                    {
                        m_sender2Out->sendData(outputData[j], MAX_UDP_SIZE);
                    }
                    m_sender2Out->sendData(outputData[14], remainSize);

                    cl_end = clock();
                    cl_result = static_cast<double>(cl_end - cl_start);
                    printf("**********************\n");
                    printf("*-+*-+*-+[server camera array process Time] %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));
                }
                else
                {
                    allData.clear();
                    allData.resize(Num_otherProtocol + ipdData[i].rawData.size());

                    //1. header input
                    allData[0] = 0xFA;
                    allData[1] = 0x0A;
                    //2. num. of packet 
                    // allData.push_back(0x00);
                    allData[2] = (static_cast<u_char>(ipdData.size()));
                    allData[3] = (static_cast<u_char>(i));

                    //3. data size input
                        dataSize = ipdData[i].rawData.size();
                    checksum = dataSize;
                    allData[4] = ((dataSize >> 8) & 0xFF);
                    allData[5] = (dataSize & 0xFF);

                    printf("[%d]Address Information : %d,  %s\n", i, sizeof(ipdData[i].address.sun_path), ipdData[i].address.sun_path);
                    // printf("[%d]data size : %d\n", i, ipdData[i].rawData.size());
                    //----IP check------
                    printf("[%d]lidar ip address : %s\n", i, ipdData[i].lidarIP.c_str());

                    //4. data packet input
                    for(size_t j=0; j<ipdData[i].rawData.size(); j++)
                    {
                        allData[6+j] = ipdData[i].rawData[j];
                    }

                    //5. checksum & tail input
                    allData[allData.size() - 4] = ((checksum >> 8) & 0xFF);
                    allData[allData.size() - 3] = (checksum & 0xFF);
                    allData[allData.size() - 2] = (0xF5);
                    allData[allData.size() - 1] = (0x05);
                    // printf("data check %d \n", allData.size());

                    if(setOuputSender)
                    {
                        printf("-----***-----***----[data send] : %d -----***-----***\n", allData.size());
                        m_sender2Out->sendData(allData);
                    }
                }
            }   
        }
        else
        {
            printf("******************************************\n");
            printf("******************************************\n");
            printf("********[client Name is Overlap!!]********\n");
            printf("******************************************\n");
            printf("******************************************\n");
        }

        // printf("====output buffer========\n");
        // for(int i=0; i<allData.size(); i++)
        // {
        //     printf("%X ", allData[i]);
        // }
        // printf("\n====data end======\n");
        

        cl_end = clock();
        cl_result = static_cast<double>(cl_end - cl_start);
        printf("**********************\n");
        printf("*-+*-+*-+[server Total Time] %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));
        cl_start = clock();
    }

    m_server->closeServer();
    return 0;
}