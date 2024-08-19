#include <cmath>
#include "astra_logo.h"
#include "variable.h"

namespace astra
{

  /**
   *   *     *      *         *
   *    *     powered by *    *
   *       * Astra UI *
   *  *       *     *     *
   *     *  *     *           *
   */

  // 绘制logo的主函数，_time参数控制动画持续时间
  void drawLogo(uint16_t _time)
  {
    // 定义动画函数，用于平滑过渡元素位置
    auto animation = [](float &_pos, float _posTrg, float _speed) -> void
    {
      if (_pos != _posTrg)
      {
        // 如果当前位置与目标位置的差距小于 0.15，直接将当前位置设置为目标位置，避免微小抖动
        if (std::fabs(_pos - _posTrg) < 0.15f)
          _pos = _posTrg;
        else
          // (_posTrg - _pos) 计算当前位置到目标位置的距离。
          //((100 - _speed) / 1.0f) 创建一个基于速度的除数。速度越高，除数越小。
          // 整个表达式计算出一个增量，这个增量会随着接近目标而变小
          _pos += (_posTrg - _pos) / ((100 - _speed) / 1.0f);
      }
    };

    // 控制渲染循环的标志
    static bool onRender = true;
    // 标记是否已初始化
    static bool isInit = false;
    // 计时器，用于控制动画进程
    static uint16_t time = 0;

    // 主渲染循环
    while (onRender)
    {
      time++; // 增加计时器

      // 存储星星的y坐标、目标y坐标和x坐标
      static std::vector<float> yStars, yStarsTrg, xStars;

      // 定义要显示的文本
      static std::string text = "astra UI";
      static std::string copyRight = "powered by";

      // 计算文本的x坐标（居中显示）
      HAL::setFont(getUIConfig().logoTitleFont);
      static float xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
      HAL::setFont(getUIConfig().logoCopyRightFont);
      static float xCopyRight = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(copyRight)) / 2;
      HAL::setFont(getUIConfig().mainFont);

      // 初始化文本y坐标（屏幕外）
      // yTitle 和 yCopyRight是设置为负值（0减去文本高度再减1）意味着初始时标题在屏幕上方不可见的位置
      // 这样可以在动画开始时，让标题从屏幕上方滑入
      // yTitleTrg 和 yCopyRightTrg 是目标位置
      static float yTitle = 0 - getUIConfig().logoTextHeight - 1;
      static float yCopyRight = 0 - getUIConfig().logoCopyRightHeight - 1;
      static float yTitleTrg = 0;
      static float yCopyRightTrg = 0;

      // 初始化背景位置（屏幕外）
      static float xBackGround = 0;
      static float yBackGround = 0 - HAL::getSystemConfig().screenHeight - 1;
      static float yBackGroundTrg = -1;

      // 动画进行中
      if (time < _time)
      {
        // 初始化星星位置（仅执行一次）
        if (!isInit)
        {
          // 清除旧数据
          yStars.clear();
          yStarsTrg.clear();
          xStars.clear();

          // 为每个星星设置随机位置
          for (unsigned char i = 0; i < getUIConfig().logoStarNum; i++)
          {
            srand(HAL::getRandomSeed() * 7); // 设置随机种子

            // 设置初始y坐标（屏幕外）
            yStars.push_back(0 - getUIConfig().logoStarLength - 1);

            // 设置目标y坐标（随机）
            yStarsTrg.push_back(1 + rand() % (uint16_t)(HAL::getSystemConfig().screenHeight - 2 * getUIConfig().logoStarLength - 2 + 1));
            // 设置x坐标（随机）
            xStars.push_back(1 + rand() % (uint16_t)(HAL::getSystemConfig().screenWeight - 2 * getUIConfig().logoStarLength - 2 + 1));
          }
          isInit = true;
        }

        // 设置文本目标位置
        // 将标题文本垂直居中显示在屏幕上
        yTitleTrg = HAL::getSystemConfig().screenHeight / 2 - getUIConfig().logoTextHeight / 2; // 标题居中
        // 这行代码将版权文本放置在标题文本的上方
        yCopyRightTrg = yTitleTrg - getUIConfig().logoCopyRightHeight - 4; // 版权信息位于标题上方
      }
      else // 动画结束，开始退场
      {
        // 设置所有元素的目标位置为屏幕外
        yBackGroundTrg = 0 - HAL::getSystemConfig().screenHeight - 1;
        yStarsTrg.assign(getUIConfig().logoStarNum, 0 - getUIConfig().logoStarLength - 1);
        yTitleTrg = 0 - getUIConfig().logoTextHeight - 1;
        yCopyRightTrg = 0 - getUIConfig().logoCopyRightHeight - 1;
      }

      // 清除画布，准备绘制新帧
      HAL::canvasClear();

      // 绘制背景
      // 设置绘图颜色，0 表示背景色（通常是黑色），1 表示前景色（通常是白色）
      HAL::setDrawType(0);
      // 绘制填充矩形（背景），使用 u8g2 的 u8g2_DrawBox 函数
      // 注意：坐标和尺寸会被四舍五入为整数
      HAL::drawBox(xBackGround, yBackGround, HAL::getSystemConfig().screenWeight, HAL::getSystemConfig().screenHeight);
      animation(yBackGround, yBackGroundTrg, getUIConfig().logoAnimationSpeed);

      // 绘制底部线条
      HAL::setDrawType(1);
      HAL::drawHLine(0, yBackGround + HAL::getSystemConfig().screenHeight, HAL::getSystemConfig().screenWeight);
      // 绘制左边框
      HAL::drawVLine(0, yBackGround, HAL::getSystemConfig().screenHeight);
      // 绘制右边框
      HAL::drawVLine(HAL::getSystemConfig().screenWeight - 1, yBackGround, HAL::getSystemConfig().screenHeight);
      // 绘制上边框
      HAL::drawHLine(0, xBackGround, HAL::getSystemConfig().screenWeight);

      // 绘制星星
      for (unsigned char i = 0; i < getUIConfig().logoStarNum; i++)
      {
        // 绘制星星的四条线
        HAL::drawHLine(xStars[i] - getUIConfig().logoStarLength - 1, yStars[i], getUIConfig().logoStarLength);
        HAL::drawHLine(xStars[i] + 2, yStars[i], getUIConfig().logoStarLength);
        HAL::drawVLine(xStars[i], yStars[i] - getUIConfig().logoStarLength - 1, getUIConfig().logoStarLength);
        HAL::drawVLine(xStars[i], yStars[i] + 2, getUIConfig().logoStarLength);

        // 更新星星位置
        animation(yStars[i], yStarsTrg[i], getUIConfig().logoAnimationSpeed);
      }

      // 绘制文本
      HAL::setFont(getUIConfig().logoTitleFont);
      HAL::drawEnglish(xTitle, yTitle + getUIConfig().logoTextHeight, text);
      HAL::setFont(getUIConfig().logoCopyRightFont);
      HAL::drawEnglish(xCopyRight, yCopyRight + getUIConfig().logoCopyRightHeight, copyRight);

      // 更新文本位置
      animation(yTitle, yTitleTrg, getUIConfig().logoAnimationSpeed);
      animation(yCopyRight, yCopyRightTrg, getUIConfig().logoAnimationSpeed);

      // 更新画布显示
      HAL::canvasUpdate();

      // 检查是否结束渲染
      if (time >= _time && yBackGround == 0 - HAL::getSystemConfig().screenHeight - 1)
        onRender = false;
    }
  }

  void drawSTART(uint16_t _time)
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
    static std::string subText = "正在通讯自检...";

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
    static std::vector<std::string> probeLabels = {"前方探头", "后方探头", "左方探头", "右方探头"};
    static std::vector<std::string> probeStatus(4);
    static std::vector<float> probeX(4, HAL::getSystemConfig().screenWeight + 1);
    static std::vector<float> probeXTrg(4);
    static float probeY[4];

    while (onRender)
    {
      // 初始下降动画
      if (animationState == 0)
      {
        // 等待动画结束，进行探头通信检测(5s),然后主标题和副标题准备从左侧滑出
        if (yBackGround == yBackGroundTrg && yTitle == yTitleTrg && ySubText == ySubTextTrg)
        {
          // 发送自检命令(这一次是为了检测通信)
          CAN_SendSelfCheckCommand(CAN_FILTER1_ID);
          CAN_SendSelfCheckCommand(CAN_FILTER2_ID);
          CAN_SendSelfCheckCommand(CAN_FILTER3_ID);
          CAN_SendSelfCheckCommand(CAN_FILTER4_ID);
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
        for (int i = 0; i < 4; ++i)
        {
          uint32_t checkBit = (1 << i);
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "连接正常" : "连接异常");
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.8) * (HAL::getSystemConfig().screenHeight / 4);
        }
        animationState = 2;
      }
      // 探头状态从右边滑入后，给一个显示的等待时间(3s)，然后探头状态准备从左边滑出
      else if (animationState == 2)
      {
        bool allReached = true;
        for (int i = 0; i < 4; ++i)
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
          for (int i = 0; i < 4; ++i)
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
        for (int i = 0; i < 4; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allExited = false;
            break;
          }
        }
        if (allExited)
        {
          subText = "正在高压自检...";

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
        CAN_SendSelfCheckCommand(CAN_FILTER1_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER2_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER3_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER4_ID);
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
        for (int i = 0; i < 4; ++i)
        {
          uint32_t checkBit = (1 << (i + 4)); // HV状态标志位从第4位开始
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "高压正常" : "高压异常");
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.8) * (HAL::getSystemConfig().screenHeight / 4);
        }
        animationState = 6;
      }
      // 探头高压自检结果从右边滑入后，给一个显示的等待时间(3s)，然后探头高压自检结果准备从左边滑出
      else if (animationState == 6)
      {
        bool allReached = true;
        for (int i = 0; i < 4; ++i)
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
          for (int i = 0; i < 4; ++i)
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
        for (int i = 0; i < 4; ++i)
        {
          if (probeX[i] != probeXTrg[i])
          {
            allExited = false;
            break;
          }
        }
        if (allExited)
        {
          subText = "正在计数自检...";
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
        CAN_SendSelfCheckCommand(CAN_FILTER1_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER2_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER3_ID);
        CAN_SendSelfCheckCommand(CAN_FILTER4_ID);
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
        for (int i = 0; i < 4; ++i)
        {
          uint32_t checkBit = (1 << (i + 8)); // 计数管状态标志位从第8位开始
          probeStatus[i] = probeLabels[i] + ": " + ((uxBits & checkBit) ? "计数正常" : "计数异常");
          probeX[i] = HAL::getSystemConfig().screenWeight + 1;
          probeXTrg[i] = 5; // 左边距
          probeY[i] = (i + 0.8) * (HAL::getSystemConfig().screenHeight / 4);
        }
        animationState = 10;
      }
      // 探头计数自检结果从右边滑入后，给一个显示的等待时间(3s)，然后探头计数自检结果准备从左边滑出
      else if (animationState == 10)
      {
        bool allReached = true;
        for (int i = 0; i < 4; ++i)
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
          for (int i = 0; i < 4; ++i)
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
        for (int i = 0; i < 4; ++i)
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
      HAL::drawVLine(xBackGround, yBackGround, HAL::getSystemConfig().screenHeight);
      HAL::drawHLine(xBackGround, yBackGround, HAL::getSystemConfig().screenWeight);
      HAL::drawHLine(xBackGround, yBackGround + HAL::getSystemConfig().screenHeight - 1, HAL::getSystemConfig().screenWeight);
      HAL::drawVLine(xBackGround + HAL::getSystemConfig().screenWeight - 1, yBackGround, HAL::getSystemConfig().screenHeight);

      // 绘制文本
      HAL::setFont(getUIConfig().logoTitleFont);
      HAL::drawChinese(xTitle, yTitle, text);
      HAL::setFont(getUIConfig().logoCopyRightFont);
      HAL::drawChinese(xSubText, ySubText, subText);

      for (int i = 0; i < 4; ++i)
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
          for (int i = 0; i < 4; ++i)
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
