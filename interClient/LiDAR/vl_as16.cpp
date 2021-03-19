#include "vl_as16.h"

VL_AS16::VL_AS16()
{

}

/** @brief 해당 함수를 통해 vector의 데이터를 프로토콜에 따라 파싱을 진행
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param datagram  : 데이터를 분류할 구조체
*/
void VL_AS16::processor(std::vector<u_char> data, LiDAR_Protocol *datagram)
{
    datagram->clear();

    if(data.size() >= 59208)
    {
        //Dectect Send Protocol 1 -- RAW DATA + OBJ DATA
        // printf("[Processor_AS16][mode1]\n");
        sortData(data, datagram);
    }
    else if(data.size() == 59206)
    {
        //Dectect Send Protocol 1 -- RAW DATA Only
        // printf("[Processor_AS16][mode2]\n");
        sortData_SET02(data, datagram);
    }
    // else if(data.size() < 30000)
    // {
    //     //Dectect Send Protocol 1 -- OBJ DATA Only
    //     printf("[Processor_AS16][mode3]\n");
    //     sortData_SET03(data, datagram);
    // }
}

/** @brief 거리 + obj 정보가 같이 전송될 경우 해당 함수 실행
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*/
void VL_AS16::sortData(std::vector<u_char> buf, LiDAR_Protocol *protocol)
{
    //length and Intensity
    sortLength(buf, protocol);

    //Obj Data Processing
    size_t Obj_Start = static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_DATAPACKET_OBJECTDATA_START_POS);
    sortOBJ(buf, protocol, Obj_Start);

    //Error & Warning
    sortEnW(buf, protocol);

    protocol->PARA_Input_END = true;
}

/** @brief 거리 정보만 전송될 경우 해당 함수 실행
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*/
void VL_AS16::sortData_SET02(std::vector<u_char> buf, LiDAR_Protocol *protocol)
{
    //length and Intensity
    sortLength(buf, protocol);

    //Error & Warning
    sortEnW(buf, protocol);

    protocol->PARA_Input_END = true;
}

/** @brief obj 정보만 전송될 경우 해당 함수 실행
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*/
void VL_AS16::sortData_SET03(std::vector<u_char> buf, LiDAR_Protocol *protocol)
{
    size_t Obj_Start = static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_DATAPACKET_HEADER_SIZE)
            + static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEAD2DATALENGTH_SIZE);
    sortOBJ(buf, protocol, Obj_Start);
    sortEnW(buf, protocol);

    protocol->PARA_Input_END = true;
}

/** @brief 거리 데이터를 파싱하기 위한 함수
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*/
void VL_AS16::sortLength(std::vector<u_char> buf, LiDAR_Protocol *protocol)
{
    int start = static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEAD2DATALENGTH_SIZE)
                    + static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_DATAPACKET_HEADER_SIZE);
    int end = static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_DATAPACKET_RAW_SIZE)
                + (static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_HEAD2DATALENGTH_SIZE)
                + static_cast<size_t>(CARNAVICOM::VL_AS16::PROTOCOLFRAME_DATAPACKET_HEADER_SIZE));

    int pos = 0;
    for (int i = start; i < end; i += 51)
    {
#define fixVal 0
#if !fixVal
        protocol->RAWdata_Angle[pos] = FUNC_HEXtoDEC(buf[i+1], buf[i]);
        protocol->RAWdata_RadialDistance00[pos] = FUNC_HEX2DEX_Length(buf[i + 3], buf[i + 2]);
        protocol->RAWdata_RadialDistance01[pos] = FUNC_HEX2DEX_Length(buf[i + 5], buf[i + 4]);
        protocol->RAWdata_RadialDistance02[pos] = FUNC_HEX2DEX_Length(buf[i + 7], buf[i + 6]);
        protocol->RAWdata_RadialDistance03[pos] = FUNC_HEX2DEX_Length(buf[i + 9], buf[i + 8]);
        protocol->RAWdata_RadialDistance04[pos] = FUNC_HEX2DEX_Length(buf[i + 11], buf[i + 10]);
        protocol->RAWdata_RadialDistance05[pos] = FUNC_HEX2DEX_Length(buf[i + 13], buf[i + 12]);
        protocol->RAWdata_RadialDistance06[pos] = FUNC_HEX2DEX_Length(buf[i + 15], buf[i + 14]);
        protocol->RAWdata_RadialDistance07[pos] = FUNC_HEX2DEX_Length(buf[i + 17], buf[i + 16]);
        protocol->RAWdata_RadialDistance08[pos] = FUNC_HEX2DEX_Length(buf[i + 19], buf[i + 18]);
        protocol->RAWdata_RadialDistance09[pos] = FUNC_HEX2DEX_Length(buf[i + 21], buf[i + 20]);
        protocol->RAWdata_RadialDistance10[pos] = FUNC_HEX2DEX_Length(buf[i + 23], buf[i + 22]);
        protocol->RAWdata_RadialDistance11[pos] = FUNC_HEX2DEX_Length(buf[i + 25], buf[i + 24]);
        protocol->RAWdata_RadialDistance12[pos] = FUNC_HEX2DEX_Length(buf[i + 27], buf[i + 26]);
        protocol->RAWdata_RadialDistance13[pos] = FUNC_HEX2DEX_Length(buf[i + 29], buf[i + 28]);
        protocol->RAWdata_RadialDistance14[pos] = FUNC_HEX2DEX_Length(buf[i + 31], buf[i + 30]);
        protocol->RAWdata_RadialDistance15[pos] = FUNC_HEX2DEX_Length(buf[i + 33], buf[i + 32]);
#else
        double length = 2;
        protocol->RAWdata_RadialDistance00.push_back(length);
        protocol->RAWdata_RadialDistance01.push_back(length);
        protocol->RAWdata_RadialDistance02.push_back(length);
        protocol->RAWdata_RadialDistance03.push_back(length);
        protocol->RAWdata_RadialDistance04.push_back(length);
        protocol->RAWdata_RadialDistance05.push_back(length);
        protocol->RAWdata_RadialDistance06.push_back(length);
        protocol->RAWdata_RadialDistance07.push_back(length);
        protocol->RAWdata_RadialDistance08.push_back(length);
        protocol->RAWdata_RadialDistance09.push_back(length);
        protocol->RAWdata_RadialDistance10.push_back(length);
        protocol->RAWdata_RadialDistance11.push_back(length);
        protocol->RAWdata_RadialDistance12.push_back(length);
        protocol->RAWdata_RadialDistance13.push_back(length);
        protocol->RAWdata_RadialDistance14.push_back(length);
        protocol->RAWdata_RadialDistance15.push_back(length);
#endif

        protocol->RAWdata_Intensity00[pos] = static_cast<int>(buf[i + 34]);
        protocol->RAWdata_Intensity01[pos] = static_cast<int>(buf[i + 35]);
        protocol->RAWdata_Intensity02[pos] = static_cast<int>(buf[i + 36]);
        protocol->RAWdata_Intensity03[pos] = static_cast<int>(buf[i + 37]);
        protocol->RAWdata_Intensity04[pos] = static_cast<int>(buf[i + 38]);
        protocol->RAWdata_Intensity05[pos] = static_cast<int>(buf[i + 39]);
        protocol->RAWdata_Intensity06[pos] = static_cast<int>(buf[i + 40]);
        protocol->RAWdata_Intensity07[pos] = static_cast<int>(buf[i + 41]);
        protocol->RAWdata_Intensity08[pos] = static_cast<int>(buf[i + 42]);
        protocol->RAWdata_Intensity09[pos] = static_cast<int>(buf[i + 43]);
        protocol->RAWdata_Intensity10[pos] = static_cast<int>(buf[i + 44]);
        protocol->RAWdata_Intensity11[pos] = static_cast<int>(buf[i + 45]);
        protocol->RAWdata_Intensity12[pos] = static_cast<int>(buf[i + 46]);
        protocol->RAWdata_Intensity13[pos] = static_cast<int>(buf[i + 47]);
        protocol->RAWdata_Intensity14[pos] = static_cast<int>(buf[i + 48]);
        protocol->RAWdata_Intensity15[pos] = static_cast<int>(buf[i + 49]);

        protocol->RAWdata_reserved[pos] = static_cast<int>(buf[i + 50]);

        pos++;
    }
}

/** @brief obj 데이터를 파싱하기 위한 함수
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*  @param startPos  : obj데이터의 시작 지점
*/
void VL_AS16::sortOBJ(std::vector<u_char> buf, LiDAR_Protocol *protocol, size_t startPos)
{
    size_t endPoint = buf.size() -4;
    size_t Obj_END;

    int objCntCheck=0;
    protocol->OBJ_CNT = static_cast<int>(buf[startPos]);

    if(protocol->OBJ_CNT%2 == 0)
    {
        Obj_END = endPoint-1 - 8; // entire Data size - error&warning Size
    }
    else
    {
        Obj_END = endPoint - 8; // entire Data size - error&warning Size
    }

    if(protocol->OBJ_CNT > 0 && protocol->OBJ_CNT < 100)
    {
        for(size_t i=startPos+1; i<Obj_END; i+=23)
        {
            if(objCntCheck < protocol->OBJ_CNT)
            {
                protocol->OBJ_ID.push_back(buf[i]);
                protocol->OBJ_xMax.push_back((FUNC_HEX2DEX_objSize(buf[i+2], buf[i+1])));
                protocol->OBJ_yMax.push_back((FUNC_HEX2DEX_objSize(buf[i+4], buf[i+3])));
                protocol->OBJ_zMax.push_back((FUNC_HEX2DEX_objSize(buf[i+6], buf[i+5])));

                protocol->OBJ_xMin.push_back((FUNC_HEX2DEX_objSize(buf[i+8], buf[i+7])));
                protocol->OBJ_yMin.push_back((FUNC_HEX2DEX_objSize(buf[i+10], buf[i+9])));
                protocol->OBJ_zMin.push_back((FUNC_HEX2DEX_objSize(buf[i+12], buf[i+11])));

                protocol->OBJ_Classification.push_back(buf[i+13]);
                protocol->OBJ_Status.push_back(buf[i+14]);
                protocol->OBJ_Relative_Velocity.push_back(buf[i+15]);
                protocol->OBJ_Relative_Acceleration.push_back(buf[i+16]);
                protocol->OBJ_Relative_Angle.push_back(each8to16(buf[i+18], buf[i+17]));
                protocol->OBJ_Relative_YawRate.push_back(each8to16(buf[i+20], buf[i+19]));
                protocol->OBJ_Current_Age.push_back(buf[i+21]);
                protocol->OBJ_Prediction_Age.push_back(buf[i+22]);
            }
            objCntCheck++;
        }
    }
}

/** @brief error&warning 데이터를 파싱하기 위한 함수
*  @date 2021.01.12
*  @return void
*  @param data      : 전송된 데이터 입력(head부터 tail까지)
*  @param protocol  : 데이터를 분류할 구조체
*/
void VL_AS16::sortEnW(std::vector<u_char> buf, LiDAR_Protocol *protocol)
{
    size_t pos;
    if(protocol->OBJ_CNT%2 == 0)
    {
        pos = buf.size()-4 -1;
    }
    else
    {
        pos = buf.size()-4;
    }

    protocol->ErrorNWarning_Internal_Temp = buf[pos-8];
    protocol->ErrorNWarning_Motor_Status = buf[pos-7];
    protocol->ErrorNWarning_APD_Voltage = buf[pos-6];
    protocol->ErrorNWarning_PnT_Status = buf[pos-5];
    protocol->ErrorNWarning_Reserved1 = buf[pos-4];
    protocol->ErrorNWarning_CANID00 = buf[pos-3];
    protocol->ErrorNWarning_CANID01 = buf[pos-2];
    protocol->ErrorNWarning_AliveCNT = buf[pos-1];
}

/** @brief 2byte 데이터를 하나의 10진수 변환
*  @date 2021.01.12
*  @return int 
*  @param up    : 상위 바이트
*  @param down  : 하위 바이트
*/
int VL_AS16::FUNC_HEXtoDEC(u_char up, u_char down)
{
    int output;
    output = static_cast<int>(up) * 256 + static_cast<int>(down);
    return output;
}

/** @brief 2byte 데이터를 하나의 10진수로 변환 후 거리 값으로 변환(cm)
*  @date 2021.01.12
*  @return int  : 거리 값 [cm]
*  @param up    : 상위 바이트
*  @param down  : 하위 바이트
*/
int VL_AS16::FUNC_HEX2DEX_Length(u_char up, u_char down)
{
    int output;
    output = static_cast<int>(up) * 256 + static_cast<int>(down);
    return output;
}

/** @brief 2byte 데이터를 좌표 값으로 변환하기 위한 함수(+,-)
*  @date 2021.01.12
*  @return void
*  @param up    : 상위 바이트
*  @param down  : 하위 바이트
*/
float VL_AS16::FUNC_HEX2DEX_objSize(u_char up, u_char down)
{
    float output;
    int sum;

    sum = static_cast<int>(up) * 256 + static_cast<int>(down);
    if(sum > 15000)
    {
        int dum = static_cast<int>(0x100000000 - static_cast<int16_t>(sum));
        output = -static_cast<float>(dum) / 100;
    }
    else
        output = static_cast<float>(sum) / 100;

    return output;
}

/** @brief 2byte 데이터를 하나의 16진수로 합치기 위한 함수
*  @date 2021.01.12
*  @return void
*  @param up    : 상위 바이트
*  @param down  : 하위 바이트
*/
uint16_t VL_AS16::each8to16(u_char up, u_char down)
{
    uint16_t output;
    output = static_cast<uint16_t>(static_cast<int>(up) * 256 + static_cast<int>(down));
    return output;
}