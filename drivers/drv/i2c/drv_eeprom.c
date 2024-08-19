#include "variable.h"

// 定义一个宏来简化成员写入操作
#define WRITE_MEMBER_TO_EEPROM(baseAddr, structPtr, member)          \
    BSP_24CXX_Write(baseAddr + offsetof(typeof(*structPtr), member), \
                    (const uint8_t *)&((structPtr)->member),         \
                    sizeof((structPtr)->member))
#define READ_MEMBER_FROM_EEPROM(baseAddr, structPtr, member)        \
    BSP_24CXX_Read(baseAddr + offsetof(typeof(*structPtr), member), \
                   (uint8_t *)&((structPtr)->member),               \
                   sizeof((structPtr)->member))

// 写入结构体成员到 EEPROM
int32_t writeMemberToEEPROM(uint16_t baseAddr, const ProbeData *sensor, size_t memberOffset, size_t memberSize)
{
    return BSP_24CXX_Write(baseAddr + memberOffset, (const uint8_t *)sensor + memberOffset, memberSize);
}

// 从 EEPROM 读取结构体成员
int32_t readMemberFromEEPROM(uint16_t baseAddr, ProbeData *sensor, size_t memberOffset, size_t memberSize)
{
    return BSP_24CXX_Read(baseAddr + memberOffset, (uint8_t *)sensor + memberOffset, memberSize);
}
