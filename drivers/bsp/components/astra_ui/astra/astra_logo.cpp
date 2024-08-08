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
    static int animationState = 0; // 0: 初始下降, 1: 左侧滑出, 2: 右侧滑入
    static uint8_t count = 0;
    static EventBits_t uxBits = 0;
    static EventBits_t waitBits = WAIT_STORAGE_CHECK | WAIT_CLOCK_CHECK | WAIT_PROBE_CHECK | WAIT_PROBE_HV_CHECK | WAIT_PROBE_STORAGE_CHECK | WAIT_GJ6401_CHECK | WAIT_J405_CHECK;

    // 定义要显示的文本
    static std::string text = "车载式辐射仪";
    static std::string subText = "正在通讯自检...";

    // 初始化文本y坐标（屏幕外）
    static float yTitle = 0 - HAL::getFontHeight() - 1;
    static float ySubText = 0 - HAL::getFontHeight() - 1;
    static float yTitleTrg = (HAL::getSystemConfig().screenHeight - HAL::getFontHeight()) / 2;
    static float ySubTextTrg = HAL::getSystemConfig().screenHeight - getUIConfig().logoTextHeight;

    // 初始化文本x坐标
    // 计算文本的x坐标（居中显示）
    HAL::setFont(getUIConfig().logoTitleFont);
    static float xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(text)) / 2;
    HAL::setFont(getUIConfig().logoCopyRightFont);
    static float xSubText = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
    static float xTitleTrg, xSubTextTrg;

    // 初始化背景位置（屏幕外）
    static float xBackGround = 0;
    static float yBackGround = 0 - HAL::getSystemConfig().screenHeight - 1;
    static float yBackGroundTrg = 0;

    while (onRender)
    {
      if (animationState == 0) // 初始下降动画
      {
        // 初始动画完成，这里的条件的意思是已经走到位
        if (yBackGround == yBackGroundTrg && yTitle == yTitleTrg && ySubText == ySubTextTrg)
        {
          // 等待电池检测标志位(5s)，这里要么超时，要么收到对应的事件标志位
          uxBits = xEventGroupWaitBits(xInit_EventGroup, WAIT_BATTERY_CHECK, pdTRUE, pdTRUE, pdMS_TO_TICKS(5000));

          // 进行一次左边滑出
          xSubTextTrg = 0 - HAL::getFontWidth(subText) - 1;
          animationState = 1;
        }
      }
      // 等待左边滑出动作完成
      else if (animationState == 1 && xSubText == xSubTextTrg)
      {
        // 这里为什么要先看count是因为要兼顾到第一次检测
        switch (count)
        {
        case 0:
          /* 打印电池检测情况 */
          if (uxBits & WAIT_BATTERY_CHECK)
          {
            subText = "探头通讯正常";
          }
          else
          {
            subText = "探头通讯异常";
          }
          break;
        case 1:
          /* 电池信息打印结束，开始存储检测，并同时开始接收事件标志位，等待2s */
          subText = "正在高压自检...";
          break;
        case 2:
          /* 打印存储检测情况 */
          if (uxBits & WAIT_STORAGE_CHECK)
          {
            subText = "探头高压正常";
          }
          else
          {
            subText = "探头高压异常";
          }
          break;
        case 3:
          /* 存储信息打印结束，开始时钟检测，并同时开始接收事件标志位，等待2s */
          subText = "正在进行预热...";
          break;

        default:
          break;
        }

        // 文本完全退出左侧，准备重新从右侧进入,在前面的switch中已经初始化好了文本
        animationState = 2;
        xSubText = HAL::getSystemConfig().screenWeight + 1;
        xSubTextTrg = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(subText)) / 2;
      }
      // 文本已经从右侧进入，准备退出
      else if (animationState == 2 && xSubText == xSubTextTrg)
      {
        /* 根据上面的switch奇数的时候都是开始查询的时候，所以在这里也就是正在查询的字样显示完毕后再开始检测事件标志组 */
        if (count % 2 != 0)
        {
          if (count == 3)
          {
            // 到了这里是预热阶段
            HAL::delay(10000);
            // 预热结束，准备退出
            onRender = false;
          }
          else
          {
            // 等待检查的事件标志位
            uxBits = xEventGroupWaitBits(xInit_EventGroup, waitBits, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));
          }
        }
        else
        {
          /* 等待2s准备退出文本 */
          HAL::delay(2000);
        }

        // 自检流程推进
        count++;

        // xTitleTrg = 0 - HAL::getFontWidth(text) - (HAL::getSystemConfig().screenWeight / 1.5);
        xSubTextTrg = 0 - HAL::getFontWidth(subText) - 1;

        // 开始退出动画
        animationState = 1;
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

      // 更新位置
      if (animationState == 0)
      {
        animation(yBackGround, yBackGroundTrg, getUIConfig().logoAnimationSpeed);
        animation(yTitle, yTitleTrg, getUIConfig().logoAnimationSpeed);
        animation(ySubText, ySubTextTrg, getUIConfig().logoAnimationSpeed);
      }
      else
      {
        // animation(xTitle, xTitleTrg, getUIConfig().logoAnimationSpeed);
        animation(xSubText, xSubTextTrg, getUIConfig().logoAnimationSpeed);
      }

      HAL::canvasUpdate();

      // 检查是否需要重新开始循环
      if (animationState == 3 && xTitle == xTitleTrg && xSubText == xSubTextTrg)
      {
        animationState = 1;
      }

      // // 检查是否结束渲染
      // if (animationState == 4 && time >= _time && yBackGround == 0 - HAL::getSystemConfig().screenHeight - 1)
      //   onRender = false;
    }

    // 设置字体为主要字体
    HAL::setFont(astra::getUIConfig().mainFont);
  }
}
