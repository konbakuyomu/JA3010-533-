//
// Created by Fir on 2024/2/11.
//

#pragma once
#ifndef ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
#define ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
#include "../hal.h"
#include "u8g2.h"

class HALDreamCore : public HAL
{	
private:
  void _hc32_hal_init();

  void _ssd1325_init();
  void _key_init();
  void _buzzer_init();
  void _u8g2_init();

public:
  HALDreamCore() = default;

protected:
  // 当你声明 u8g2_t canvasBuffer {}; 时，程序会在内存中分配一块空间来存储这个结构体
  // 这个结构体中包含了指向实际图形数据的指针，以及控制显示的各种参数。
  // 虽然 canvasBuffer 看起来只是一个变量，但它实际上代表了一个完整的图形缓冲系统。
  // 当你调用绘图函数时，这些函数会修改 canvasBuffer 所管理的内存区域。
  // 这个内存区域就是我们比喻中的"画布"，每个像素对应内存中的一个或多个位。
  u8g2_t canvasBuffer{};
  static unsigned char _u8x8_byte_hw_spi_callback(u8x8_t *_u8x8, unsigned char _msg, unsigned char _argInt, void *_argPtr);

  static unsigned char _u8x8_gpio_and_delay_callback(U8X8_UNUSED u8x8_t *_u8x8,
                                                     U8X8_UNUSED unsigned char _msg,
                                                     U8X8_UNUSED unsigned char _argInt,
                                                     U8X8_UNUSED void *_argPtr);

public:
  inline void init() override
  {
    _hc32_hal_init();

    // _ssd1325_init(); // u8g2 初始化中已经初始化了，不需要再手动初始化
    _key_init();
    _buzzer_init();
    _u8g2_init();
  }

protected:
  void _ssd1325_transmit_cmd(unsigned char _cmd);
  void _ssd1325_transmit_data(unsigned char _data, unsigned char _mode);
  static void _ssd1325_reset(bool _state);
  void _ssd1325_set_cursor(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
  void _ssd1325_fill(unsigned char _data);

public:
  void _screenOn() override;
  void _screenOff() override;

public:
  void *_getCanvasBuffer() override;
  unsigned char _getBufferTileHeight() override;
  unsigned char _getBufferTileWidth() override;
  void _canvasUpdate() override;
  void _canvasClear() override;
  void _setFont(const unsigned char *_font) override;
  unsigned char _getFontWidth(std::string &_text) override;
  unsigned char _getFontHeight() override;
  void _setDrawType(unsigned char _type) override;
  void _drawPixel(float _x, float _y) override;
  void _drawEnglish(float _x, float _y, const std::string &_text) override;
  void _drawChinese(float _x, float _y, const std::string &_text) override;
  void _drawVDottedLine(float _x, float _y, float _h) override;
  void _drawHDottedLine(float _x, float _y, float _l) override;
  void _drawVLine(float _x, float _y, float _h) override;
  void _drawHLine(float _x, float _y, float _l) override;
  void _drawBMP(float _x, float _y, float _w, float _h, const unsigned char *_bitMap) override;
  void _drawBox(float _x, float _y, float _w, float _h) override;
  void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
  void _drawFrame(float _x, float _y, float _w, float _h) override;
  void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;

public:
  void _delay(unsigned long _mill) override;
  unsigned long _millis() override;
  unsigned long _getTick() override;
  unsigned long _getRandomSeed() override;

public:
  void _beep(float _freq) override;
  void _beepStop() override;
  void _setBeepVol(unsigned char _vol) override;

public:
  bool _getKey(key::KEY_INDEX _keyIndex) override;

public:
  void _updateConfig() override;
};

#endif // ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
