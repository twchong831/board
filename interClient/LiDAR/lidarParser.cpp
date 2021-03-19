/**
* @file     lidarParser.cpp
* @brief    카네비컴 라이다의 데이터를 파싱하기 위한 파일
* @details  카네비컴 라이다의 데이터를 입력하여 이를 각 파트별로 파싱하기 위한 함수 정의
* @date     2021-01-11
* @version  0.0.1
*/
#include "lidarParser.h"

lidarParser::lidarParser()
{
    protocolDatagram = new LiDAR_Protocol();
    g_LiDARModel = CARNAVICOM::LiDAR_Model_CH16;

    m_detect_Start = false;
    m_detect_End = false;

    g_parsingMode = CARNAVICOM::TRANSMISSION_MODE::Length_MODE;

    g_checkModel = true;

    m_vlas16_Processor = new VL_AS16();
}

/** @brief 파싱하기 위한 데이터를 set
*  @date 2021.01.12
*  @return bool
*  @param data : 전송된 데이터 입력
*/
bool lidarParser::setData(std::vector<u_char> data)
{
    if(g_checkModel)
    {
        protocolDatagram->LiDAR_Model = classificationModel(data);  //라이다 종류 확인
        g_checkModel = false;
    }
    switch (protocolDatagram->LiDAR_Model)
    {
    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VL_AS16) :
        /* code */
        return accumulateData_VLAS16(data);

    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VLP_16) :
        /* code */
        break;
    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VLP_32) :
        /* code */
        break;
    default:
        break;
    }
}

/** @brief  LiDAR 모델 정보를 송신
*  @date 2021.01.12
*  @return std::string 라이다 제조사/모델
*  @param NULL
*/
std::string lidarParser::getLidarModel()
{
    switch (protocolDatagram->LiDAR_Model)
    {
    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VL_AS16) :
        /* code */
        return "CARNAVICOM / VL-AS16";

    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VLP_16) :
        /* code */
        return "VELODYNE / VLP-16";
    case static_cast<int>(CARNAVICOM::MODEL::LiDAR::VLP_32) :
        /* code */
        return "VELODYNE / VLP-32";
    default:
        return "Not CONNETED";
    }
}

int lidarParser::getLidarModel_()
{
    return protocolDatagram->LiDAR_Model;
}

/** @brief  LiDAR datagram 획득
*  @date 2021.01.12
*  @return void
*  @param datagram : protocolDatagram 데이터 반환
*/
void lidarParser::getLiDARdatagram(LiDAR_Protocol *datagram)
{
    datagram = protocolDatagram;
}

/** @brief  LiDAR 모델 확인
*  @date 2021.01.12
*  @return int 라이다 모델
*  @param data : input 데이터 입력
*/
int lidarParser::classificationModel(std::vector<u_char> data)
{
    printf("[parser][classification MODEL]\n");
    if((static_cast<int>(data[0]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEADER_VALUE_00))
        && (static_cast<int>(data[1]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEADER_VALUE_01)))
    {
        return static_cast<int>(CARNAVICOM::MODEL::LiDAR::VL_AS16);
    }
    else if(static_cast<int>(data[data.size() - 2]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_TAIL_VALUE_00)
        && static_cast<int>(data[data.size() - 1]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_TAIL_VALUE_01))
    {
        return static_cast<int>(CARNAVICOM::MODEL::LiDAR::VL_AS16);
    }
}

/** @brief  데이터 파싱 진행
*  @date 2021.01.12
*  @return void
*  @param data : input 데이터 파싱 진행
*/
void lidarParser::parsing_VLAS16(std::vector<u_char> data)
{
    g_lengthBuffer.clear();
    g_xyzBuffer.clear();
    parsingRawData(data, protocolDatagram);

    switch(g_parsingMode)
    {
        case CARNAVICOM::TRANSMISSION_MODE::RAW_MODE :
            //nothing
            break;
        case CARNAVICOM::TRANSMISSION_MODE::Length_MODE :
            // printf("parsing Mode is Length mode\n");
            parseLengthData(protocolDatagram);
            break;
        case CARNAVICOM::TRANSMISSION_MODE::OBJ_MODE :
            break;
        case CARNAVICOM::TRANSMISSION_MODE::XYZ_MODE :
            parseXyzData(protocolDatagram);
            break;
    }
}

/** @brief  LiDAR 입력 데이터 누적
*  @date 2021.01.12
*  @return bool : 누적이 완료되면 true 반환
*  @param data : 매 프레임 input 데이터
*/
bool lidarParser::accumulateData_VLAS16(std::vector<u_char> data)
{
    if((static_cast<int>(data[0]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEADER_VALUE_00))
        && (static_cast<int>(data[1]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEADER_VALUE_01)))
    {
        printf("*****detect HEADER\n");
        for(int i=0; i<data.size(); i++)
        {
            g_lidarBuffer.push_back(data[i]);
        }
        m_detect_Start  = true;
    }

    if(static_cast<int>(data[data.size() - 2]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_TAIL_VALUE_00)
        && static_cast<int>(data[data.size() - 1]) == static_cast<int>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_TAIL_VALUE_01)
        && m_detect_Start)
    {
        printf("detect END********\n");
        for(int i=0; i<data.size(); i++)
        {
            g_lidarBuffer.push_back(data[i]);
        }
        m_detect_End = true;
    }

    if(m_detect_Start && m_detect_End )
    {
        //data output Sque.
        m_detect_Start = false;
        m_detect_End = false;
        
        //parsing
        parsing_VLAS16(g_lidarBuffer);

        previous_lidarBuffer.clear();
        previous_lidarBuffer = g_lidarBuffer;
        g_lidarBuffer.clear();

        printf("[parser][parsing complete]\n");
        return true;
    }
    else
    {
        printf("check head & tail %d, %d\n", m_detect_Start, m_detect_End);
        return false;
    }
}

/** @brief  LiDAR low 데이터 사이즈 확인
*  @date 2021.01.12
*  @return size_t 누적된 low 데이터 사이즈
*  @param NULL
*/
size_t lidarParser::getRawDataSize()
{
    if(!m_detect_Start && !m_detect_End && previous_lidarBuffer.size() != 0)
    {
        return previous_lidarBuffer.size();
    }
    else
    {
        return 0;
    }
}

/** @brief  LiDAR low 데이터 획득
*  @date 2021.01.12
*  @return std::vector<u_char> 헤드부터 테일까지 축적한 데이터 획득
*  @param NULL
*/
std::vector<u_char> lidarParser::getRawData()
{
    if(!m_detect_Start && !m_detect_End && previous_lidarBuffer.size() != 0)
    {
        return previous_lidarBuffer;
    }
}

/** @brief  데이터 파싱 모드 선택
*  @date 2021.01.12
*  @return void
*  @param mode :파싱 모드 선택
*/
void lidarParser::setParsingMode(int mode)
{
    g_parsingMode = mode;
}

std::vector<u_char> lidarParser::getParsingData()
{
    if(!m_detect_Start && !m_detect_End && previous_lidarBuffer.size() != 0)
    {
        printf("[parser][command get data][mode : %d]\n", g_parsingMode);
        switch(g_parsingMode)
        {
        case CARNAVICOM::TRANSMISSION_MODE::RAW_MODE : 
            return previous_lidarBuffer;
        case CARNAVICOM::TRANSMISSION_MODE::Length_MODE :
            return g_lengthBuffer;
        case CARNAVICOM::TRANSMISSION_MODE::XYZ_MODE : 
            return g_xyzBuffer;
        // case CARNAVICOM::TRANSMISSION_MODE::OBJ_MODE :
        //     return getOBJData();
        }
    }
}

void lidarParser::parsingRawData(std::vector<u_char> data, LiDAR_Protocol *datagram)
{
    m_vlas16_Processor->processor(data, datagram);
}

void lidarParser::parseLengthData(LiDAR_Protocol *datagram)
{
    g_lengthBuffer.clear();
    //parsing 순서
    //각도 - 채널별 거리 - intensity
    int16_t buffer_ = 0;
    for(int i=0; i<VL_AS16_DATASIZE; i++)
    {
        //각도 값 입력
        g_lengthBuffer.push_back((static_cast<int16_t>(datagram->RAWdata_Angle[i]) >> 8) & 0xFF);
        g_lengthBuffer.push_back((static_cast<int16_t>(datagram->RAWdata_Angle[i])) & 0xFF);
        // printf("check angel : %f, %X, %X\n", datagram->RAWdata_Angle[i], g_lengthBuffer[g_lengthBuffer.size()-2], g_lengthBuffer[g_lengthBuffer.size()-1]);
        //거리 값 입력
        int j=0;
        while( j < 16)
        {
            // printf("check j is ? %d\n", j);
            switch (j)
            {
            case 0:
                /* code */
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance00[i]);
                break;
            case 1:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance01[i]);
                break;
            case 2:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance02[i]);
                break;
            case 3:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance03[i]);
                break;
            case 4:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance04[i]);
                break;
            case 5:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance05[i]);
                break;
            case 6:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance06[i]);
                break;
            case 7:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance07[i]);
                break;
            case 8:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance08[i]);
                break;
            case 9:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance09[i]);
                break;
            case 10:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance10[i]);
                break;
            case 11:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance11[i]);
                break;
            case 12:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance12[i]);
                break;
            case 13:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance13[i]);
                break;
            case 14:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance14[i]);
                break;
            case 15:
                buffer_ = convertLength2Byte(datagram->RAWdata_RadialDistance15[i]);
                break;
            }

            g_lengthBuffer.push_back((buffer_ >> 8) & 0xFF);
            g_lengthBuffer.push_back((buffer_) & 0xFF);
            j++;
        }
        //intensity 값 입력
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity00[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity01[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity02[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity03[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity04[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity05[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity06[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity07[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity08[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity09[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity10[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity11[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity12[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity13[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity14[i]));
        g_lengthBuffer.push_back(static_cast<u_char>(datagram->RAWdata_Intensity15[i]));

        //reserved 값 입력 0x00
        // g_lengthBuffer.push_back(RESERVED);
    }
    // printf("Parsing data size :%d\n", g_lengthBuffer.size());
}

void lidarParser::parseXyzData(LiDAR_Protocol *datagram)
{

}

int16_t lidarParser::convertLength2Byte(double length)
{
    int16_t value = 0x00000000;
    double buffer = length;
    value = static_cast<int16_t>(buffer);
    // printf("check length to 2byte : %f\n", buffer);
    // printf("check length hex : %X \n", (int16_t)buffer);
    // printf("check length hex 4byte: %X \n", value);
    return value;
}