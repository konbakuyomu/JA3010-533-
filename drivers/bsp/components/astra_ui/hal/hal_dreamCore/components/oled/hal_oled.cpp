//
// Created by Fir on 2024/2/11.
//
#include "variable.h"
#include "../../hal_dreamCore.h"
#include <cmath>

/**
 * @brief 复位SSD1306 OLED显示器
 *
 * @param _state 复位状态
 * @return void
 */
void HALDreamCore::_st7567_reset(bool _state)
{
  if (_state)
    SPI_RST_SET();
  else
    SPI_RST_RESET();
}

/**
 * @brief 点亮屏幕
 * @param none
 * @param none
 */
void HALDreamCore::_screenOn()
{
  u8g2_SetPowerSave(&canvasBuffer, 0);
}

/**
 * @brief 关闭屏幕
 * @param none
 * @param none
 */
void HALDreamCore::_screenOff()
{
  u8g2_SetPowerSave(&canvasBuffer, 1);
}

unsigned char HALDreamCore::_u8x8_byte_hw_spi_callback(u8x8_t *_u8x8, unsigned char _msg, unsigned char _argInt, void *_argPtr)
{ // NOLINT
  switch (_msg)
  {
  case U8X8_MSG_BYTE_SEND: /*通过SPI发送arg_int个字节数据*/
  {
    // 开启SPI的DMA传输
    DMA_SPI_Send(_argPtr, _argInt);
    // SPI_Trans(SPI_UNIT, _argPtr, _argInt, 1000); // 非DMA传输
    break;
  }
  case U8X8_MSG_BYTE_INIT: /*初始化函数*/
    break;
  case U8X8_MSG_BYTE_SET_DC: /*设置DC引脚,表明发送的是数据还是命令*/
  {
    (_argInt != 0) ? SPI_DC_DATA() : SPI_DC_CMD();
    break;
  }
  case U8X8_MSG_BYTE_START_TRANSFER:
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    break;
  default:
    return 0;
  }
  return 1;
}

unsigned char HALDreamCore::_u8x8_gpio_and_delay_callback(__attribute__((unused)) u8x8_t *_u8x8,
                                                          __attribute__((unused)) unsigned char _msg,
                                                          __attribute__((unused)) unsigned char _argInt,
                                                          __attribute__((unused)) void *_argPtr)
{ // NOLINT
  switch (_msg)
  {
  case U8X8_MSG_GPIO_AND_DELAY_INIT: /*delay和GPIO的初始化，在main中已经初始化完成了*/
    break;
  case U8X8_MSG_DELAY_MILLI: /*延时函数*/
    vTaskDelay(_argInt / portTICK_PERIOD_MS);
    break;
  case U8X8_MSG_GPIO_CS: /*片选信号*/
    break;
  case U8X8_MSG_GPIO_DC:
    break;
  case U8X8_MSG_GPIO_RESET:
    _st7567_reset(_argInt);
    break;
  default:
    break;
  }
  return 1;
}

void HALDreamCore::_u8g2_init()
{
  u8g2_Setup_st7567_pi_132x64_f(&canvasBuffer,
                                U8G2_R0, // 不旋转
                                _u8x8_byte_hw_spi_callback,
                                _u8x8_gpio_and_delay_callback);
  u8g2_InitDisplay(&canvasBuffer);     // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
  u8g2_SetPowerSave(&canvasBuffer, 0); // 打开显示器
  u8g2_ClearBuffer(&canvasBuffer);

  u8g2_SetFontMode(&canvasBuffer, 1);           /*字体模式选择*/
  u8g2_SetFontDirection(&canvasBuffer, 0);      /*字体方向选择*/
  u8g2_SetFont(&canvasBuffer, u8g2_my_font_16); /*字库选择*/

  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void *HALDreamCore::_getCanvasBuffer()
{
  return u8g2_GetBufferPtr(&canvasBuffer);
}

unsigned char HALDreamCore::_getBufferTileHeight()
{
  return u8g2_GetBufferTileHeight(&canvasBuffer);
}

unsigned char HALDreamCore::_getBufferTileWidth()
{
  return u8g2_GetBufferTileWidth(&canvasBuffer);
}

void HALDreamCore::_canvasUpdate()
{
  u8g2_SendBuffer(&canvasBuffer);
}

// 清除画布，使用 u8g2 库的 u8g2_ClearBuffer 函数
void HALDreamCore::_canvasClear()
{
  u8g2_ClearBuffer(&canvasBuffer);
}

void HALDreamCore::_setFont(const unsigned char *_font)
{
  u8g2_SetFontMode(&canvasBuffer, 1);      /*字体模式选择*/
  u8g2_SetFontDirection(&canvasBuffer, 0); /*字体方向选择*/
  u8g2_SetFont(&canvasBuffer, _font);
}

unsigned char HALDreamCore::_getFontWidth(std::string &_text)
{
  return u8g2_GetUTF8Width(&canvasBuffer, _text.c_str());
}

unsigned char HALDreamCore::_getFontHeight()
{
  return u8g2_GetMaxCharHeight(&canvasBuffer);
}

void HALDreamCore::_setDrawType(unsigned char _type)
{
  u8g2_SetDrawColor(&canvasBuffer, _type);
}

void HALDreamCore::_drawPixel(float _x, float _y)
{
  u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y));
}

void HALDreamCore::_drawEnglish(float _x, float _y, const std::string &_text)
{
  u8g2_DrawStr(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void HALDreamCore::_drawEnglish_str(float _x, float _y, const char *str)
{
  u8g2_DrawStr(&canvasBuffer, _x, _y, str);
}

void HALDreamCore::_drawChinese(float _x, float _y, const std::string &_text)
{
  u8g2_DrawUTF8(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void HALDreamCore::_drawVDottedLine(float _x, float _y, float _h)
{
  for (unsigned char i = 0; i < (unsigned char)std::round(_h); i++)
  {
    if (i % 8 == 0 || (i - 1) % 8 == 0 || (i - 2) % 8 == 0)
      continue;
    u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y) + i);
  }
}

void HALDreamCore::_drawHDottedLine(float _x, float _y, float _l)
{
  for (unsigned char i = 0; i < _l; i++)
  {
    if (i % 8 == 0 || (i - 1) % 8 == 0 || (i - 2) % 8 == 0)
      continue;
    u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x) + i, (int16_t)std::round(_y));
  }
}

void HALDreamCore::_drawVLine(float _x, float _y, float _h)
{
  u8g2_DrawVLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_h));
}

void HALDreamCore::_drawHLine(float _x, float _y, float _l)
{
  u8g2_DrawHLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_l));
}

void HALDreamCore::_drawBMP(float _x, float _y, float _w, float _h, const unsigned char *_bitMap)
{
  u8g2_DrawXBMP(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), _bitMap);
}

void HALDreamCore::_drawBox(float _x, float _y, float _w, float _h)
{
  u8g2_DrawBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

void HALDreamCore::_drawRBox(float _x, float _y, float _w, float _h, float _r)
{
  u8g2_DrawRBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}

void HALDreamCore::_drawFrame(float _x, float _y, float _w, float _h)
{
  u8g2_DrawFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

void HALDreamCore::_drawRFrame(float _x, float _y, float _w, float _h, float _r)
{
  u8g2_DrawRFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}
