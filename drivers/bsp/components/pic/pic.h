#pragma once

#include <vector>
#include <cstdint>

inline const std::vector<uint8_t> &get_pic_buzzer()
{
    static const std::vector<uint8_t> pic_buzzer = {
        0xff, 0xff, 0xff, 0x38, 0xff, 0xff, 0x7f, 0x30, 0xff, 0xff, 0x30, 0x20, 0xff, 0x3f, 0x00, 0x07,
        0xff, 0x0f, 0x00, 0x07, 0xff, 0x87, 0x1f, 0x07, 0xff, 0xc3, 0x3f, 0x20, 0xff, 0xf0, 0x7f, 0x30,
        0x3f, 0xf8, 0x7f, 0x38, 0x01, 0xfe, 0xff, 0x38, 0x00, 0xff, 0xff, 0x38, 0xf8, 0xff, 0xff, 0x38,
        0xf1, 0xff, 0xff, 0x38, 0xe3, 0xff, 0xff, 0x38, 0xc3, 0xff, 0xff, 0x3c, 0xc7, 0xff, 0x7f, 0x3c,
        0x87, 0xff, 0x3f, 0x3e, 0x07, 0xff, 0x1f, 0x3e, 0x23, 0xfe, 0x0f, 0x3f, 0x63, 0xfc, 0x8f, 0x3f,
        0x73, 0xf8, 0xc7, 0x3f, 0xe3, 0xf8, 0xe3, 0x3f, 0xe3, 0xf1, 0xf1, 0x3f, 0xc7, 0xe3, 0xf1, 0x3f,
        0x07, 0xc0, 0xf9, 0x3f, 0x0f, 0x80, 0xf9, 0x3f, 0x3f, 0x1c, 0xf8, 0x3f, 0xff, 0x3f, 0xf8, 0x3f,
        0xff, 0x7f, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f};
    return pic_buzzer;
}

inline const std::vector<uint8_t> &get_pic_realtime_dose()
{
    static const std::vector<uint8_t> pic_realtime_dose = {
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xfc, 0xcf, 0x3f, 0x7f, 0xf8, 0x87, 0x3f,
        0x3f, 0xf8, 0x07, 0x3f, 0x1f, 0xf3, 0x33, 0x3e, 0x9f, 0xf3, 0x73, 0x3e, 0x8f, 0xe7, 0x79, 0x3c,
        0xcf, 0xe7, 0xf9, 0x3c, 0x0f, 0xe0, 0x01, 0x3c, 0x0f, 0x30, 0x03, 0x3c, 0xff, 0x3f, 0xff, 0x3f,
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0x1f, 0xfe, 0x3f, 0xff, 0x0f, 0xfc, 0x3f,
        0xff, 0xcf, 0xfc, 0x3f, 0xff, 0xe7, 0xf9, 0x3f, 0xff, 0xe3, 0xf1, 0x3f, 0xff, 0xf3, 0xf3, 0x3f,
        0xff, 0x03, 0xf0, 0x3f, 0xff, 0x07, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f};
    return pic_realtime_dose;
}

inline const std::vector<uint8_t> &get_pic_clear_cumulative()
{
    static const std::vector<uint8_t> pic_clear_cumulative = {
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
        0xff, 0x1f, 0xfe, 0x3f, 0xff, 0x1f, 0xfe, 0x3f, 0xff, 0x0c, 0xcc, 0x3f, 0x7f, 0x00, 0x80, 0x3f,
        0x3f, 0x00, 0x00, 0x3f, 0x3f, 0xe0, 0x01, 0x3f, 0x7f, 0xf8, 0x87, 0x3f, 0x7f, 0xfc, 0x8f, 0x3f,
        0x3f, 0xfc, 0x0f, 0x3f, 0x0f, 0x3e, 0x1f, 0x3c, 0x0f, 0x1e, 0x1e, 0x3c, 0x0f, 0x1e, 0x1e, 0x3c,
        0x0f, 0x3e, 0x1f, 0x3c, 0x3f, 0xfc, 0x0f, 0x3f, 0x7f, 0xfc, 0x8f, 0x3f, 0x7f, 0xf8, 0x87, 0x3f,
        0x3f, 0xe0, 0x01, 0x3f, 0x3f, 0x00, 0x00, 0x3f, 0x7f, 0x00, 0x80, 0x3f, 0xff, 0x0c, 0xcc, 0x3f,
        0xff, 0x1f, 0xfe, 0x3f, 0xff, 0x1f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
        0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f};
    return pic_clear_cumulative;
}

inline const std::vector<uint8_t> &get_pic_cumulative_dose()
{
    static const std::vector<uint8_t> pic_cumulative_dose = {
        0xff, 0xff, 0xff, 0x3f, 0xff, 0x01, 0xfe, 0x3f, 0x7f, 0xdc, 0xf9, 0x3f, 0xbf, 0xff, 0xe7, 0x3f,
        0xdf, 0xff, 0xcf, 0x3f, 0xef, 0xff, 0x9f, 0x3f, 0xf7, 0xff, 0xbf, 0x3f, 0xfb, 0xff, 0x7f, 0x3f,
        0xfb, 0xdf, 0x7f, 0x3f, 0xf9, 0xdf, 0xff, 0x2e, 0xfd, 0xdf, 0xff, 0x36, 0xfd, 0xdf, 0xff, 0x3b,
        0xfd, 0xdf, 0xff, 0x3d, 0xf9, 0xef, 0x7f, 0x3c, 0xfd, 0xf7, 0x0f, 0x3e, 0xfd, 0xff, 0x67, 0x3e,
        0xfd, 0xff, 0x6b, 0x3f, 0xfb, 0xff, 0x79, 0x3f, 0xfb, 0xff, 0x3c, 0x3f, 0xf7, 0x7f, 0x9e, 0x3f,
        0xe7, 0xbf, 0xcf, 0x3f, 0xcf, 0x0f, 0xe7, 0x3f, 0x9f, 0xcf, 0xf3, 0x3f, 0x7f, 0x8e, 0xf9, 0x3f,
        0xff, 0x37, 0xfc, 0x3f, 0xff, 0x31, 0xfe, 0x3f, 0xff, 0xd3, 0xfe, 0x3f, 0xff, 0xc7, 0xff, 0x3f,
        0xff, 0xef, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f};
    return pic_cumulative_dose;
}
