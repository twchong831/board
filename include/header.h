#ifndef HEADER_H_H
#define HEADER_H_H

/**
* @file     header.h
* @brief    카네비컴 LiDAR API의 최상위 헤더
* @details  LiDAR 데이터 처리 중 공통으로 필요한 변수 및 struct 생성
* @date     2021-01-11
* @version  0.0.1
*/

#include <iostream>
#include <sstream>
#include <math.h>
#include <time.h>
#include <chrono>

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <sys/un.h>
#include <stdint.h>

#define VL_AS16_DATASIZE    1160
#define MODE_LENGTH_DATASIZE    58020

#define MAX_UDP_SIZE    64000

namespace CARNAVICOM{
    enum TRANSMISSION_MODE{
        RAW_MODE,       //0 -- lidar low data 전체를 전송하는 모드
        Length_MODE,    //1 -- length data만 전송하는 모드
        XYZ_MODE,       //2 -- 좌표로 변환하여 전송하는 모드
        OBJ_MODE,        //3 -- object data만 전송하는 모드
        RGB_MODE
    };
    namespace PROPERTIES{
        enum COLOR{
            COLOR_byLength  = 1,
            COLOR_byCH      = 2,
            COLOR_byALL     = 3,
            COLOR_byVAR     = 4
        };

        enum GRID_PROPERTIES{
            RECT            = 1,
            CIRCLE          = 2,
            RECTnCIRCLE     = 3,
            NONE            = 4
        };
    };
    namespace MODEL{
        enum class LiDAR :int{
            NONE,       //검출하지 못함
            VL_AS16,    //카네비컴 VL-AS16 16채널 모델
            VLP_16,     //Velodyne VLP-16 16채널 모델
            VLP_32      //Velodyne VLP-32 32채널 모델
        };
        enum class RADRA : int{
            NONE,
            SMR         //smart radar system
        };
        enum class CAMERA :int{
            NONE,
            camera
        };
    };

    enum class VL_AS16 : int {
        PROTOCOLFRAME_HEAD2DATALENGTH_SIZE      = 14,
        PROTOCOLFRAME_CheckSUM2TAIL_SIZE        = 4,
        PROTOCOLFRAME_DATAPACKET_HEADER_SIZE    = 20,
        PROTOCOLFRAME_DATAPACKET_RAW_SIZE       = 59160,
        PROTOCOLFRAME_DATAPACKET_OBJECTDATA_START_POS   = 59194,
        PROTOCOLFRAME_DATAPACKET_ERROERnWARNING         = 8,

        PROTOCOLFRAME_HEADER_VALUE_00           = 10,
        PROTOCOLFRAME_HEADER_VALUE_01           = 250,
        PROTOCOLFRAME_TAIL_VALUE_00             = 5,
        PROTOCOLFRAME_TAIL_VALUE_01             = 245
    };

    enum class VL_MEMS : int {
        PROTOCOLFRAME_HEAD2DATALENGTH_SIZE      = 14,
        PROTOCOLFRAME_CheckSUM2TAIL_SIZE        = 4,
        PROTOCOLFRAME_DATAPACKET_HEADER_SIZE    = 20,
        PROTOCOLFRAME_DATAPACKET_RAW_SIZE       = 16425,//10575,
        PROTOCOLFRAME_DATAPACKET_OBJECTDATA_START_POS   = 10609,
        PROTOCOLFRAME_DATAPACKET_ERROERnWARNING         = 8
    };

    const double MAXIMUM_DISPlAY_LENGTH = 200;

    //VL-S3
    const double VL_S3_HorizontalResolution = 0.16;
    const double VL_S3_VertitalResolution = 1;
    const double VL_S3_FoV = 120;
    const int    VL_S3_CH  = 3;

    //VL-AS16
    const double VL_AS16_HorizontalResolution = 0.125;
    const double VL_AS16_VertitalResolution = 0.6;
    const double VL_AS16_FoV = 145;
    const int    VL_AS16_CH  = 16;

    //MEMS
    const double MEMS_HorizontalResolution = 0.8;
    const double MEMS_VertitalResolution = 1.5;
    const double MEMS_FoV = 180;
    const int    MEMS_CH  = 14;

    //Basix Value
    const bool BASIC_XAxis_Inverted = false;
    const bool BASIC_LineChoose = true;

    //input Var.
    const std::string Message_Model_Sel = "MODEL SELECT";
    const std::string LiDAR_Model_S3 = "VL-S3";
    const std::string LiDAR_Model_CH16 = "VL-AS16";
    const std::string Message_IP_Not   = "LiDAR Not Connected";
    const std::string Message_IP_Sel = "IP Address SELECT";
    const std::string IP_Address_basic = "192.168.123.54";

    const std::string LiDAR_Model_VLP16 = "VLP-16";
    const std::string LiDAR_Model_VLP32 = "VLP-32";

    //command response list
    enum LiDAR_RESPONSE{
        COMM_RESP_OK            = 0,
        COMM_RESP_TRANS_TIMEOUT = 65535,
        COMM_RESP_TRANS_TOO_BIG = 65534,
        COMM_RESP_TRANS_CHKSUL  = 65533,
        COMM_RESP_TRANS_MSGEND  = 65532,
        COMM_RESP_TRANS_ERR     = 65520,
        COMM_RESP_CMD_RSV       = 65519,
        COMM_RESP_CMD_BAD       = 65518,
        COMM_RESP_CMD_ERR       = 65504,
        COMM_RESP_BAD_UPARA     = 65489,
        COMM_RESP_BAD_DATALEN   = 65490,
        COMM_RESP_BAD_DATA      = 65492,
        COMM_RESP_ERROR         = 65488,
        COMM_RESP_REPORT        = 1
    };
}

typedef struct liDAR_Protocol {
    int         LiDAR_Model;
    double      PARA_Vertical_Resolution;
    double      PARA_Horizontal_Resolution;
    int         PARA_Start_Angle;
    int         PARA_End_Angle;
    bool        PARA_Input_END;
    /*  ************************
     *          S3 : 1
     *          S16 : 2
     * ***************************/
    std::string LiDAR_IP;
    uint8_t     HEADER_SerialID[4]; //[0~3]
    uint16_t    HEADER_StartAngle;  //[4~5]
    uint16_t    HEADER_EndAngle;    //[6~7]
    uint16_t    HEADER_DataLength;  //[8~9]
    uint8_t     HEADER_DataType;    //[10]
    uint8_t     HEADER_NTPTime[8];  //[11~18]
    uint8_t     HEADER_reserved1;   //[19]

    int		RAWdata_Angle[VL_AS16_DATASIZE];					//[20-21], 145 degree, now H angle			┐

    int		RAWdata_RadialDistance00[VL_AS16_DATASIZE];		//[22-25], length data, all channel Lenght	│	-- Data 1160EA[

    int		RAWdata_RadialDistance01[VL_AS16_DATASIZE];		//[26-29], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance02[VL_AS16_DATASIZE];		//[30-34], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance03[VL_AS16_DATASIZE];		//[35-38], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance04[VL_AS16_DATASIZE];		//[39-42], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance05[VL_AS16_DATASIZE];		//[43-46], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance06[VL_AS16_DATASIZE];		//[47-50], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance07[VL_AS16_DATASIZE];		//[51-54], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance08[VL_AS16_DATASIZE];		//[55-58], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance09[VL_AS16_DATASIZE];		//[59-62], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance10[VL_AS16_DATASIZE];		//[62-65], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance11[VL_AS16_DATASIZE];		//[65-68], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance12[VL_AS16_DATASIZE];		//[69-72], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance13[VL_AS16_DATASIZE];		//[73-76], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance14[VL_AS16_DATASIZE];		//[77-80], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_RadialDistance15[VL_AS16_DATASIZE];		//[81-84], length data, all channel Lenght	│	-- Data 1160EA

    int		RAWdata_Intensity00[VL_AS16_DATASIZE];			//[85], intensity - now not use??		│

    int		RAWdata_Intensity01[VL_AS16_DATASIZE];			//[86], intensity - now not use??		│

    int		RAWdata_Intensity02[VL_AS16_DATASIZE];			//[87], intensity - now not use??		│

    int		RAWdata_Intensity03[VL_AS16_DATASIZE];			//[88], intensity - now not use??		│

    int		RAWdata_Intensity04[VL_AS16_DATASIZE];			//[89], intensity - now not use??		│

    int		RAWdata_Intensity05[VL_AS16_DATASIZE];			//[90], intensity - now not use??		│

    int		RAWdata_Intensity06[VL_AS16_DATASIZE];			//[91], intensity - now not use??		│

    int		RAWdata_Intensity07[VL_AS16_DATASIZE];			//[92], intensity - now not use??		│

    int		RAWdata_Intensity08[VL_AS16_DATASIZE];			//[93], intensity - now not use??		│

    int		RAWdata_Intensity09[VL_AS16_DATASIZE];			//[94], intensity - now not use??		│

    int		RAWdata_Intensity10[VL_AS16_DATASIZE];			//[95], intensity - now not use??		│

    int		RAWdata_Intensity11[VL_AS16_DATASIZE];			//[96], intensity - now not use??		│

    int		RAWdata_Intensity12[VL_AS16_DATASIZE];			//[97], intensity - now not use??		│

    int		RAWdata_Intensity13[VL_AS16_DATASIZE];			//[98], intensity - now not use??		│

    int		RAWdata_Intensity14[VL_AS16_DATASIZE];			//[99], intensity - now not use??		│

    int		RAWdata_Intensity15[VL_AS16_DATASIZE];			//[100], intensity - now not use??		│

    int		RAWdata_reserved[VL_AS16_DATASIZE];				//[101]	??								┘

    int                     OBJ_CNT;

    std::vector<uint8_t>	OBJ_ID;

    std::vector<float>      OBJ_xMax;

    std::vector<float>      OBJ_yMax;  //rewrite -> 2byte

    std::vector<float>      OBJ_zMax;

    std::vector<float>      OBJ_xMin;

    std::vector<float>      OBJ_yMin;      //rewrite -> 2byte

    std::vector<float>      OBJ_zMin;

    std::vector<uint8_t>	OBJ_Classification;

    std::vector<uint8_t>	OBJ_Status;

    std::vector<uint8_t>	OBJ_Relative_Velocity;

    std::vector<uint8_t>	OBJ_Relative_Acceleration;

    std::vector<uint16_t>	OBJ_Relative_Angle;

    std::vector<uint16_t>	OBJ_Relative_YawRate;

    std::vector<uint8_t>	OBJ_Current_Age;

    std::vector<uint8_t>	OBJ_Prediction_Age;

    int8_t			ErrorNWarning_Internal_Temp;
    uint8_t			ErrorNWarning_Motor_Status;
    uint8_t			ErrorNWarning_APD_Voltage;
    uint8_t			ErrorNWarning_PnT_Status;
    uint8_t			ErrorNWarning_Reserved1;
    uint8_t			ErrorNWarning_CANID00;
    uint8_t			ErrorNWarning_CANID01;
    uint8_t			ErrorNWarning_AliveCNT;

    //MEMS
    bool MEMS_SLOP;
    int  MEMS_x;
    int  MEMS_y;

    liDAR_Protocol() {
        LiDAR_Model = 2;
        PARA_Vertical_Resolution = CARNAVICOM::VL_AS16_VertitalResolution;
        PARA_Horizontal_Resolution = CARNAVICOM::VL_AS16_HorizontalResolution;
        PARA_Start_Angle    = 0;
        PARA_End_Angle      = 1159;
        PARA_Input_END      = false;
        /*  ************************
         *          S3 : 1
         *          S16 : 2
         * ***************************/
        LiDAR_IP = "192.168.1.80";
        HEADER_SerialID[0] = 1; //[0~3]
        HEADER_SerialID[1] = 2; //[0~3]
        HEADER_SerialID[2] = 3; //[0~3]
        HEADER_SerialID[3] = 4; //[0~3]
        HEADER_StartAngle = 0;  //[4~5]
        HEADER_EndAngle = 1159;    //[6~7]
        HEADER_DataLength = 18544;  //[8~9]
        HEADER_DataType = 1;    //[10]
        for(int i=0; i<8; i++)
        {
            HEADER_NTPTime[i] = 0;
        }
        HEADER_reserved1 = 0;   //[19]
        OBJ_CNT = 0;

        ErrorNWarning_Internal_Temp=0;
        ErrorNWarning_Motor_Status=0;
        ErrorNWarning_APD_Voltage=0;
        ErrorNWarning_PnT_Status=0;
        ErrorNWarning_Reserved1=0;
        ErrorNWarning_CANID00=0;
        ErrorNWarning_CANID01=0;
        ErrorNWarning_AliveCNT=0;

        //MEMS
        MEMS_SLOP = false;
        MEMS_x = 0;
        MEMS_y = 0;
    }

    void clear()
    {
        memset(RAWdata_Angle, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity00, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity01, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity02, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity03, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity04, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity05, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity06, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity07, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity08, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity09, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity10, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity11, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity12, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity13, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity14, 0, VL_AS16_DATASIZE);
        memset(RAWdata_Intensity15, 0, VL_AS16_DATASIZE);

        memset(RAWdata_RadialDistance00, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance01, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance02, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance03, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance04, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance05, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance06, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance07, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance08, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance09, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance10, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance11, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance12, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance13, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance14, 0, VL_AS16_DATASIZE);
        memset(RAWdata_RadialDistance15, 0, VL_AS16_DATASIZE);
        
        memset(RAWdata_reserved, 0, VL_AS16_DATASIZE);

        OBJ_ID.clear();
        OBJ_xMax.clear();
        OBJ_yMax.clear();
        OBJ_zMax.clear();
        OBJ_xMin.clear();
        OBJ_yMin.clear();
        OBJ_zMin.clear();

        OBJ_Classification.clear();
        OBJ_Status.clear();
        OBJ_Relative_Velocity.clear();
        OBJ_Relative_Acceleration.clear();
        OBJ_Relative_Angle.clear();
        OBJ_Relative_YawRate.clear();
        OBJ_Current_Age.clear();
        OBJ_Prediction_Age.clear();

        PARA_Input_END = false;
    }

    int size()
    {
        int size = 1160 * 16;
        return size;
    }

}LiDAR_Protocol;

typedef struct pointXYZ{
    float x;
    float y;
    float z;

    pointXYZ()
    {
        //initialize
        x = 0;
        y = 0;
        z = 0;
    }

    float length(){
        return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
    }
    void clear(){
        x = 0;
        y = 0;
        z = 0;
    }
}PointXYZ;

typedef struct IPC_PACKET_{
    struct sockaddr_un address;
    std::vector<u_char> rawData;
    std::string lidarIP;
    
    IPC_PACKET_()
    {
        memset(&address, 0, sizeof(address));
        rawData.clear();
    }
}IPC_PACKET;

#define RESERVED    0

#endif // HEADER_H_H
