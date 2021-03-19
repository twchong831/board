#ifndef PARSER_H
#define PARSER_H

/**
* @file     lidarParser.h
* @brief    카네비컴 라이다의 데이터를 파싱하기 위한 파일
* @details  카네비컴 라이다의 데이터를 입력하여 이를 각 파트별로 파싱
* @date     2021-01-11
* @version  0.0.1
*/

#include "../../include/header.h"
#include "vl_as16.h"
#include <cmath>
/**
*   @brief LiDAR 데이터 파싱
*   @details 입력된 데이터를 정렬하고 각부를 parsing하기 위한 class
*   @author 정태원, twchong@carnavi.com
*   @date 2021.01.11
*   @version 0.0.1
*/
class lidarParser
{
public:
    lidarParser();

    bool setData(std::vector<u_char> data);
    std::string getLidarModel();
    int getLidarModel_();

    void getLiDARdatagram(LiDAR_Protocol *datagram);

    size_t getRawDataSize();
    std::vector<u_char> getRawData();

    void setParsingMode(int mode);
    std::vector<u_char> getParsingData();

private:
    //function
    int classificationModel(std::vector<u_char> data);
    void parsing_VLAS16(std::vector<u_char> data);

    //accumulate
    bool accumulateData_VLAS16(std::vector<u_char> data);

    //parsing -- method
    void parsingRawData(std::vector<u_char> data, LiDAR_Protocol *datagram);

    void parseLengthData(LiDAR_Protocol *datagram);
    void parseXyzData(LiDAR_Protocol *datagram);

    //length to 2bte
    int16_t convertLength2Byte(double length);

    //variable
    std::string g_LiDARModel;
    LiDAR_Protocol *protocolDatagram;

    //lidar data buffer
    std::vector<u_char> g_lidarBuffer;
    std::vector<u_char> previous_lidarBuffer;
    std::vector<u_char> g_lengthBuffer;
    std::vector<u_char> g_xyzBuffer;

    //detect lidar protocol start & end
    bool m_detect_Start;
    bool m_detect_End;

    //data Parsing mode
    int g_parsingMode;

    bool g_checkModel;

    //each lidar processor
    VL_AS16 *m_vlas16_Processor;
};

#endif // PARSER_H
