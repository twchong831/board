#include "processor/carnavicomProcessor.h"
#include <thread>
#include <pthread.h>
#include <sstream>
#include "../interCommu/interCommunication.h"

float HEX2Float(u_char up, u_char down);
void printfData(const std::vector<u_char> data);

void printfData(const std::vector<u_char> data)
{
    //check all data
    printf("[Sensor] : %X%X\n", data[0], data[1]);
    printf("[transmission Mode] : %X\n", data[2]);
    printf("[position] %X%X -> %f, %X%X -> %f, %X%X -> %f\n", 
                                            data[3], data[4], HEX2Float(data[3], data[4]),
                                            data[5], data[6], HEX2Float(data[5], data[6]),
                                            data[7], data[8], HEX2Float(data[7], data[8]));
    printf("[Roll,Pitch, Yaw] %X%X, %X%X, %X%X\n",  data[9], data[10],
                                                    data[11], data[12],
                                                    data[13], data[14]);
    printf("[Roll,Pitch, Yaw] %X%X->%f, %X%X->%f, %X%X->%f\n",  data[9], data[10], HEX2Float(data[9], data[10]),
                                                    data[11], data[12], HEX2Float(data[11], data[12]),
                                                    data[13], data[14], HEX2Float(data[13], data[14]));
    printf("RESERVED : %X\n", data[15]);

    for(size_t i=16; i<data.size()-4; i+=50)
    {
        int angle = static_cast<int>(((data[i] * 256) + data[i+1]));
        printf("[angle] : %X%X -> %d -------------\n", data[i], data[i+1], angle);
        printf("[%d Length] : ", i);
        int j=0;
        for(j=i+2; j<i+32; j+=2)
        {
            printf("[%d] %X%X ", j, data[j], data[j+1]);
        }
        printf("\n[%d intensity] : ", j);
        int k=0;
        for(k=j; k<j+16; k++)
        {
            printf("[%d] %X ", k, data[k]);
        }
        printf("\n");
        // printf("\n%d reserved : %X\n", k, data[k+1]);
    }
}

float HEX2Float(u_char up, u_char down)
{
    float output;
    int16_t sum;

    sum = static_cast<int>(up) * 256 + static_cast<int>(down);
    // printf("[before]check high bit : %X, %X, %X\n", sum, ~sum, (sum & 0xFF00) >> 15 );
    if((sum & 0xFF00) >> 15 == 1)
    {
        // int dum = static_cast<int>(0x100000000 - static_cast<int16_t>(sum));
        sum -= 0x01;
        sum = ~sum;
        output = -static_cast<float>(sum) / 100;
    }
    else
        output = static_cast<float>(sum) / 100;

    return output;
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
                "                   -L/-l : local IP Address Setting\n"
                "                   -T/-t : target(sensor) IP Address Setting\n"
                "                   -M/-m : Mode Setting -- 1 : Length Mode\n"
                "                                           2 : XYZ Mode\n"
                "                   -D/-d : Debug mode\n"
                "                   -H/-h : Command List \n"
                "       ex) lidarAPI -l 192.168.100.82 2000 -c /tmp/IPC_DCU_client -s /tmp/IPC_DCU";
    return output;
}

/**
* @mainpage main.cpp
* @brief    LiDAR 프로세서 동작을 확인하기 위한 main
* @details  carnavicomLidarProcessor를 통한 데이터 처리를 확인
*/
int main(int argc, char* argv[])
{
    carnavicomLidarProcessor *m_carnaviLidar;
    m_carnaviLidar = new carnavicomLidarProcessor;

    interCommu *m_clinet;
    m_clinet = new interCommu;
    
    //var. argv
    bool setArgvXYZ = false;
    bool setArgvRolling = false;
    bool setArgvServer = false;
    bool setArgvClient = false;
    bool setArgvLocal = false;
    bool setArgvTarget = false;
    bool setArgvDebug = false;
    bool setArgvMode = false;
    //sensor position
    double pos_xyz[3] = {-1, 1, 0.5};   //x,y,z
    double pos_rpy[3] = {10, -10, 30};   //roll, pitch, yaw

    char* argvServerName = "null";
    char* argvClientName = "null";
    char* argvLocalIP = "null";
    char* argvTargetIP = "null";

    int argvPort = 5000;

    int argvMode = 1;

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
                argvServerName = argv[i+1];
            }
            else if(!strcmp(argv[i], "-C") || !strcmp(argv[i], "-c"))
            {
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
            else if(!strcmp(argv[i], "-L") || !strcmp(argv[i], "-l"))
            {
                setArgvLocal = true;
                argvLocalIP = argv[i+1];
                argvPort = atoi(argv[i+2]);
            }
            else if(!strcmp(argv[i], "-T") || !strcmp(argv[i], "-t"))
            {
                setArgvTarget = true;
                argvTargetIP = argv[i+1];
            }
            else if(!strcmp(argv[i], "-M") || !strcmp(argv[i], "-m"))
            {
                setArgvMode = true;
                argvMode = atoi(argv[i+1]);
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

    if(!setArgvLocal)
    {
        printf("Please set Setting\n");
        printf("%s\n", warningHelp().c_str());
        return 1;
    }

    printf("----check ARGV----------\n");
    printf("server : %s\n", argvServerName);
    printf("client : %s\n", argvClientName);
    printf("XYZ : %f %f %f\n", pos_xyz[0], pos_xyz[1], pos_xyz[2]);
    printf("rolling : %f %f %f\n", pos_rpy[0], pos_rpy[1], pos_rpy[2]);
    printf("local IP : %s\n", argvLocalIP);
    printf("local IP port : %d\n", argvPort);
    printf("target IP : %s\n", argvTargetIP);

    m_carnaviLidar->setProcessName("pr_1");
    //set UDP information
    // m_carnaviLidar->setLiDAR_IP("192.168.100.103");
    printf("set Lidar UDP information -----------\n");
    printf("IP : %s \n", argvLocalIP);
    printf("port : %d\n", argvPort);
    
    //set local IP
    {
        m_carnaviLidar->setLocalIP(argvLocalIP);
        m_carnaviLidar->setPORT(argvPort);
    }
    //set parsing mode
    if(setArgvMode && (argvMode == 1 || argvMode == 2) == false )
    {
        printf("mode error\n");
        printf("%s\n", warningHelp().c_str());
    }
    m_carnaviLidar->setTransmissionMode(argvMode);

    //variable -- lidar data
    std::vector<u_char> lidarRawData;
    u_char total_Data[MODE_LENGTH_DATASIZE];
    memset(total_Data, 0, MODE_LENGTH_DATASIZE);

        clock_t cl_start = 0;
        clock_t cl_end = 0;
        double cl_result = 0;

    //IPC 세팅 clinet ->> server
    int ipc_clntSocket;
    if(setArgvServer && setArgvClient)
    {
        m_clinet->InitClient(ipc_clntSocket, argvClientName);
    }
    
    //2. kind of Data
    u_char tranmissionMode = 0;
    switch(m_carnaviLidar->getTransmissionMode())
    {
    case CARNAVICOM::TRANSMISSION_MODE::Length_MODE :
        tranmissionMode = 0;
        break;
    case CARNAVICOM::TRANSMISSION_MODE::XYZ_MODE :
        tranmissionMode = 2;
        break;
    case CARNAVICOM::TRANSMISSION_MODE::RAW_MODE :
        tranmissionMode = 1;
        break;
    }

    sleep(1);

    while(1)
    {
        cl_start = clock();

        if(m_carnaviLidar->processor_oneTime() == LidarPROCESSOR::RESPONSE::NONE)
        {
            // lidarRawData = m_carnaviLidar->getRawData();
            lidarRawData = m_carnaviLidar->getData();
        }

        cl_end = clock();
        cl_result = static_cast<double>(cl_end - cl_start);
        // printf("*****Client process******\n***********\n");
        // printf("*-+*-+*-+[lidar data get Time] %.3f[ms]\n", (double)(cl_result/CLOCKS_PER_SEC * 1000));

        if(lidarRawData.size()>0)
        {
            //1. kind of Sensor -- lidar
            total_Data[0] = 'L';
            total_Data[1] = m_carnaviLidar->getLiDARModel();
            
            //2. kind of Data
            total_Data[2] = (tranmissionMode);

            //need sensor -- lidar spec.?

            //3. position X,Y,Z
            int16_t pos_x = double2Hex(pos_xyz[0]);
            int16_t pos_y = double2Hex(pos_xyz[1]);
            int16_t pos_z = double2Hex(pos_xyz[2]);
            total_Data[3] = ((pos_x >> 8) & 0xFF);
            total_Data[4] = (pos_x & 0xFF);
            // printf("check pos x convert result : %X %X : %X%X\n", pos_x, (pos_x) >> 8, (static_cast<int16_t>(pos_x) >> 8) & 0xFF,(static_cast<int16_t>(pos_x)) & 0xFF);
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

            //5. reserved input
            total_Data[15] = RESERVED;

            // printf("check data size : %d\n", total_Data.size());

            //6. data get and Parsing
            // printf("****[client main][mode] : %d [size] : %d\n", m_carnaviLidar->getTransmissionMode(), lidarRawData.size());
            for(int i=0; i<lidarRawData.size(); i++)
            {
                total_Data[16+i] = (lidarRawData[i]);
            }

            //ps. 각 라이다 IP 확인 프로세스
            //ps. 각 라이다의 IP는 데이터 벡터의 맨 뒤에 배치함
            std::string lidarIP = m_carnaviLidar->getLidarIP();
            // printf("---check lidar ip : %s\n", lidarIP.c_str());
            std::istringstream dumStr(lidarIP);
            std::string ips;
            int numIP;
            int ipPos = 0;
            while(getline(dumStr, ips, '.'))
            {
                numIP = atoi(ips.c_str());
                total_Data[MODE_LENGTH_DATASIZE - 4 + ipPos] = ((u_char)(numIP));
                ipPos++;
            }
            // IP 확인
            // printf("check raw data IP : %d.%d.%d.%d\n", total_Data[MODE_LENGTH_DATASIZE-4],
            //                                             total_Data[MODE_LENGTH_DATASIZE-3],
            //                                             total_Data[MODE_LENGTH_DATASIZE-2],
            //                                             total_Data[MODE_LENGTH_DATASIZE-1]);
            //data check
            if(setArgvDebug)
            {
                // printfData(total_Data);
            }

            //---IPC send data
            if(setArgvServer && setArgvClient)
            {
                m_clinet->sendData_array(ipc_clntSocket, total_Data, MODE_LENGTH_DATASIZE, argvServerName);
            }

            //시간 측정-----------------------------------------------
            cl_end = clock();
            cl_result = static_cast<double>(cl_end - cl_start);
            printf("***[lidar %s Total Time] %.3f[ms]****\n", argvLocalIP, (double)(cl_result/CLOCKS_PER_SEC * 1000));
            // printf("check data size : %d\n", lidarRawData.size());
            // printf("check data size(+ip) : %d\n", total_Data.size());
            printf("***********\n***********\n");
            //-------------------------------------------------------
            lidarRawData.clear();
        }
    }

    delete m_carnaviLidar;
    return 0;
}