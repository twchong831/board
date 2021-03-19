#ifndef VL_AS16_H
#define VL_AS16_H
/**
* @file     vl_as16.h
* @brief    카네비컴 라이다 VL-AS16을 파싱하기 위한 헤더 파일
* @details  카네비컴 라이다 VL-AS16의 데이터를 입력받아 각부분을 파싱한다.
* @date     2021-01-12
* @version  0.0.1
*/

#include "../../include/header.h"
class VL_AS16{

public:
    VL_AS16();

    void processor(std::vector<u_char> data, LiDAR_Protocol *datagram);

private:

    void sortData(std::vector<u_char> buf, LiDAR_Protocol *protocol);
    void sortData_SET02(std::vector<u_char> buf, LiDAR_Protocol *protocol);
    void sortData_SET03(std::vector<u_char> buf, LiDAR_Protocol *protocol);

    void sortLength(std::vector<u_char> buf, LiDAR_Protocol *protocol);
    void sortOBJ(std::vector<u_char> buf, LiDAR_Protocol *protocol, size_t startPos);
    void sortEnW(std::vector<u_char> buf, LiDAR_Protocol *protocol);

    int FUNC_HEXtoDEC(u_char up, u_char down);
    int FUNC_HEX2DEX_Length(u_char up, u_char down);
    float FUNC_HEX2DEX_objSize(u_char up, u_char down);

    uint16_t each8to16(u_char up, u_char down);

};

#endif // VL_AS16_H
