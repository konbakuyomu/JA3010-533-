#include <cmath>
#include "astra_logo.h"
#include "variable.h"

namespace astra
{
  /**
   * @brief 绘制启动画面
   * 
   * 启动画面包含多个动画状态，依次显示探头通信自检、高压自检和计数自检的结果。
   * 
   * @note 该函数在一个循环中运行，直到所有动画状态完成并且预热结束。
   */
  void drawSTART()
  {
    auto animation = [](float &_pos, float _posTrg, float _speed) -> void
    {
      if (_pos != _posTrg)
      {
        if (std::fabs(_pos - _posTrg) < 0.15f)
          _pos = _posTrg;
        else
          _pos += (_posTrg - _pos) / ((100 - _speed) / 1.0f);
      }
    };

    static bool onRender = true;
    static int animationState = 0; // 0: 初始下降, 1: 左侧滑出, 2: 右侧滑入, 3: 探头状态显示
    static EventBits_t uxBits = 0;

    // 定义要显示的文本
    static std::string text = "车载式辐射仪";
    static std::string subText = "探头通讯自检...";

    // 初始化文本y坐标（屏幕外）
    static float yTitle = 0 - HAL::getFontHeight() - 1;
    static float ySubText = 0 - HAL::getFontHeight() - 1;
    static float yTitleTrg = (HAL::getSystemConfig().screenHeight - HAL::getFontHeight()) / 2;
    static float ySubTextTrg = HAL::getSystemConfig().screenHeight - getUIConfig().logoTextHeight;

    // 初始化文本x坐标
    HAL::setFont(getUIConfig().logoTitleFont);
    static float xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
    HAL::setFont(getUIConfig().logoCopyRightFont);
    static float xSubText = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
    static float xTitleTrg = xTitle, xSubTextTrg = xSubText;

    // 初始化背景位置（屏幕外）
    static float xBackGround = 0;
    static float yBackGround = 0 - HAL::getSystemConfig().screenHeight - 1;
    static float yBackGroundTrg = 0;

    // 探头状态显示
    static std::vector<std::string> probeLabels = {"探头1", "探头2"};
    static std::vector<std::string> probeStatus(2);
    static std::vector<float> probeX(2, HAL::getSystemConfig().screenWeight + 1);
    static std::vector<float> probeXTrg(2);
    static float probeY[2];

    while (onRender)
    {
      // 初始下降动画
      if (animationState == 0)
      {
        // 等待动画结束，进行探头通信检测(5s),然后主标题和副标题准备从左侧滑出
        if (yBackGround == yBackGroundTrg && yTitle == yTitleTrg && ySubText == ySubTextTrg)
        {
          uint8_t probeID = CAN_PROBE1_ID;
          // 发送自检命令(这一次是为了检测通信)
          key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));
          probeID = CAN_PROBE2_ID;
          key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));
          // 等待自检结果
          uxBits = xEventGroupWaitBits(xInit_EventGroup, ALL_CONNECT_CHECK, pdTRUE, pdTRUE, pdMS_TO_TICKS(5000));

          HAL::setFont(getUIConfig().logoTitleFont);
          xTitleTrg = 0 - HAL::getFontWidth(text) - 1;
          HAL::setFont(getUIConfig().logoCopyRightFont);
          xSubTextTrg = 0 - HAL::getFontWidth(subText) - 1;
          animationState = 1;
        }
      }
      // 主标题和副标题左边滑出后，准备显示探头连接结果
      else if (animationState == 1 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 准备显示探头连接状态
        for (int i = 0; i < 2; ++i)
        {
          uint32_t checkBit = (1 << i);
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "通讯正常" : "通讯异常");
          Gloabal_ProbeStatus[i].connected = (uxBits & checkBit) ? true : false;
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.6) * (HAL::getSystemConfig().screenHeight / 2);
        }
        animationState = 2;
      }
      // 探头状态从右边滑入后，给一个显示的等待时间(3s)，然后探头状态准备从左边滑出
      else if (animationState == 2)
      {
        bool allReached = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allReached = false;
            break;
          }
        }
        if (allReached)
        {
          HAL::delay(3000); // 显示3秒
          for (int i = 0; i < 2; ++i)
          {
            probeXTrg[i] = 0 - HAL::getFontWidth(probeStatus[i]) - 1;
          }
          animationState = 3;
        }
      }
      // 探头状态从左边滑出后，更改副标题为高压自检提示，然后主标题和副标题准备从右侧滑入
      else if (animationState == 3)
      {
        bool allExited = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allExited = false;
            break;
          }
        }
        if (allExited)
        {
          subText = "探头高压自检...";

          HAL::setFont(getUIConfig().logoTitleFont);
          xTitle = HAL::getSystemConfig().screenWeight + 1;
          xTitleTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
          HAL::setFont(getUIConfig().logoCopyRightFont);
          xSubText = HAL::getSystemConfig().screenWeight + 1;
          xSubTextTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
          animationState = 4;
        }
      }
      // 主标题和副标题从右边滑入后，等待探头高压自检完成，然后主标题和副标题准备从左边滑出
      else if (animationState == 4 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 发送自检命令(这一次是为了检测高压)
        uint8_t probeID = CAN_PROBE1_ID;
        key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));
        probeID = CAN_PROBE2_ID;
        key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));

        // 等待高压检查完成或超时
        uxBits = xEventGroupWaitBits(xInit_EventGroup, ALL_HV_CHECK, pdTRUE, pdTRUE, pdMS_TO_TICKS(5000));

        HAL::setFont(getUIConfig().logoTitleFont);
        xTitleTrg = 0 - HAL::getFontWidth(text) - 1;
        HAL::setFont(getUIConfig().logoCopyRightFont);
        xSubTextTrg = 0 - HAL::getFontWidth(subText) - 1;
        animationState = 5;
      }
      // 主标题和副标题从左边滑出后，准备显示高压自检结果
      else if (animationState == 5 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 准备显示探头高压自检结果
        for (int i = 0; i < 2; ++i)
        {
          uint32_t checkBit = (1 << (i + 4)); // HV状态标志位从第4位开始
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "高压正常" : "高压异常");
          Gloabal_ProbeStatus[i].connected = (uxBits & checkBit) ? true : false;
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.6) * (HAL::getSystemConfig().screenHeight / 2);
        }
        animationState = 6;
      }
      // 探头高压自检结果从右边滑入后，给一个显示的等待时间(3s)，然后探头高压自检结果准备从左边滑出
      else if (animationState == 6)
      {
        bool allReached = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allReached = false;
            break;
          }
        }
        if (allReached)
        {
          HAL::delay(3000); // 显示3秒
          for (int i = 0; i < 2; ++i)
          {
            probeXTrg[i] = 0 - HAL::getFontWidth(probeStatus[i]) - 1;
          }
          animationState = 7;
        }
      }
      // 探头状态从左边滑出后，更改副标题为通信自检提示，然后主标题和副标题准备从右侧滑入
      else if (animationState == 7)
      {
        bool allExited = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allExited = false;
            break;
          }
        }
        if (allExited)
        {
          subText = "探测器自检...";
          HAL::setFont(getUIConfig().logoTitleFont);
          xTitle = HAL::getSystemConfig().screenWeight + 1;
          xTitleTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
          HAL::setFont(getUIConfig().logoCopyRightFont);
          xSubText = HAL::getSystemConfig().screenWeight + 1;
          xSubTextTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
          animationState = 8;
        }
      }
      // 主标题和副标题从右边滑入后，等待通信自检完成，然后主标题和副标题准备从左边滑出
      else if (animationState == 8 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 发送自检命令(这一次是为了检测计数)
        uint8_t probeID = CAN_PROBE1_ID;
        key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));
        probeID = CAN_PROBE2_ID;
        key_value_msg("CAN_SelfCheck", &probeID, sizeof(probeID));

        // 等待通信自检完成或超时
        uxBits = xEventGroupWaitBits(xInit_EventGroup, ALL_COUNT_CHECK, pdTRUE, pdTRUE, pdMS_TO_TICKS(5000));

        HAL::setFont(getUIConfig().logoTitleFont);
        xTitleTrg = 0 - HAL::getFontWidth(text) - 1;
        HAL::setFont(getUIConfig().logoCopyRightFont);
        xSubTextTrg = 0 - HAL::getFontWidth(subText) - 1;
        animationState = 9;
      }
      // 主标题和副标题从左边滑出后，准备显示计数自检结果
      else if (animationState == 9 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 准备显示探头计数自检结果
        for (int i = 0; i < 2; ++i)
        {
          uint32_t checkBit = (1 << (i + 8)); // 计数管状态标志位从第8位开始
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "探测器正常" : "探测器异常");
          Gloabal_ProbeStatus[i].connected = (uxBits & checkBit) ? true : false;
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.6) * (HAL::getSystemConfig().screenHeight / 2);
        }
        animationState = 10;
      }
      // 探头计数自检结果从右边滑入后，给一个显示的等待时间(3s)，然后探头计数自检结果准备从左边滑出
      else if (animationState == 10)
      {
        bool allReached = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allReached = false;
            break;
          }
        }
        if (allReached)
        {
          HAL::delay(3000); // 显示3秒
          for (int i = 0; i < 2; ++i)
          {
            probeXTrg[i] = 0 - HAL::getFontWidth(probeStatus[i]) - 1;
          }
          animationState = 11;
        }
      }
      // 探头计数结果从左边滑出后，更改副标题为正在预热，准备从右侧滑入
      else if (animationState == 11)
      {
        bool allExited = true;
        for (int i = 0; i < 2; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allExited = false;
            break;
          }
        }
        if (allExited)
        {
          subText = "正在预热...";
          HAL::setFont(getUIConfig().logoTitleFont);
          xTitle = HAL::getSystemConfig().screenWeight + 1;
          xTitleTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
          HAL::setFont(getUIConfig().logoCopyRightFont);
          xSubText = HAL::getSystemConfig().screenWeight + 1;
          xSubTextTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
          animationState = 12;
        }
      }
      // 等待预热完成，然后退出循环
      else if (animationState == 12 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        // 在预热的时候开启提醒探头上传剂量率和累计剂量命令的软件定时器
        xTimerStart(xDoseRateTimer, 0);
        HAL::delay(10000); // 显示10秒
        onRender = false;
      }

      HAL::canvasClear();

      // 绘制背景
      HAL::setDrawType(0);
      HAL::drawBox(xBackGround, yBackGround, HAL::getSystemConfig().screenWeight, HAL::getSystemConfig().screenHeight);

      // 绘制边框
      HAL::setDrawType(1);
      // HAL::drawVLine(xBackGround, yBackGround, HAL::getSystemConfig().screenHeight);
      // HAL::drawHLine(xBackGround, yBackGround, HAL::getSystemConfig().screenWeight);
      // HAL::drawHLine(xBackGround, yBackGround + HAL::getSystemConfig().screenHeight - 1, HAL::getSystemConfig().screenWeight);
      // HAL::drawVLine(xBackGround + HAL::getSystemConfig().screenWeight - 1, yBackGround, HAL::getSystemConfig().screenHeight);

      // 绘制文本
      HAL::setFont(getUIConfig().logoTitleFont);
      HAL::drawChinese(xTitle, yTitle, text);
      HAL::setFont(getUIConfig().logoCopyRightFont);
      HAL::drawChinese(xSubText, ySubText, subText);

      for (int i = 0; i < 2; ++i)
      {
        HAL::drawChinese(probeX[i], probeY[i], probeStatus[i]);
      }

      // 更新位置
      if (animationState == 0)
      {
        animation(yBackGround, yBackGroundTrg, getUIConfig().logoAnimationSpeed);
        animation(yTitle, yTitleTrg, getUIConfig().logoAnimationSpeed);
        animation(ySubText, ySubTextTrg, getUIConfig().logoAnimationSpeed);
      }
      else
      {
        animation(xTitle, xTitleTrg, getUIConfig().logoAnimationSpeed);
        animation(xSubText, xSubTextTrg, getUIConfig().logoAnimationSpeed);
        if (animationState == 2 || animationState == 3 || animationState == 6 || animationState == 7 || animationState == 10 || animationState == 11)
        {
          for (int i = 0; i < 2; ++i)
          {
            animation(probeX[i], probeXTrg[i], getUIConfig().logoAnimationSpeed);
          }
        }
      }

      HAL::canvasUpdate();
    }

    // 设置字体为主要字体
    HAL::setFont(astra::getUIConfig().mainFont);
  }
}
