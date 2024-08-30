//
// Created by Fir on 2024/2/2.
//

#include "launcher.h"
#include "ProbeDataManager.h"

namespace astra
{

  /**
   * @brief 显示一个带有自动消失功能的弹出信息
   * @param _info 要显示的信息
   * @param _time 显示的持续时间（单位：毫秒）
   */
  void Launcher::popInfo(std::string _info, uint16_t _time)
  {
    // 静态变量,用于控制初始化和渲染状态
    static bool init = false;
    static unsigned long long int beginTime = this->time;
    static bool onRender = false;

    // 如果未初始化,进行初始化设置
    if (!init)
    {
      init = true;
      beginTime = this->time;
      onRender = true;
    }

    // 计算弹出框的尺寸和位置
    float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin; // 宽度
    float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin;     // 高度
    float yPop = 0 - hPop - 8;                                           // 从屏幕上方滑入
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;    // 目标位置 中间偏上
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;       // 居中

    // 按键事件(这里不需要按键号)
    btn_event_t event = btn_not_press;

    // 如果是自定义 page 页面，要通过camera传参来判断是否使用清屏和刷新函数
    std::vector<float> stop_clear = {1.0f, 1.0f};

    // 渲染循环
    while (onRender)
    {
      time++;

      HAL::canvasClear();
      /*渲染一帧*/
      if (uiType == UIType::Circular)
      {
        // 环形界面刷新
        currentMenu->render(false);
      }
      else
      {
        // 树形界面刷新
        if ((currentMenu->getType() == "Page"))
        {
          currentMenu->render(stop_clear);
        }
        else
        {
          currentMenu->render(camera->getPosition());
          selector->render(camera->getPosition());
          camera->update(currentMenu, selector);
        }
      }
      /*渲染一帧*/

      HAL::setDrawType(0);
      HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
      HAL::setDrawType(1);                                                              // 反色显示
      HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius); // 绘制一个圆角矩形
      HAL::drawChinese(xPop + getUIConfig().popMargin,
                       yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                       _info); // 绘制文字

      HAL::canvasUpdate();

      Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed); // 动画

      // 检查按键并直接处理
      std::pair<uint8_t, btn_event_t> keyResult = HAL::keyScan();
      event = keyResult.second;

      // 如果时间到或者按键按下就退出提示框
      if ((time - beginTime >= _time) || (event == btn_click) || (event == btn_long_press))
        yPopTrg = 0 - hPop - 8; // 滑出

      if (yPop == 0 - hPop - 8)
      {
        onRender = false; // 退出条件
        init = false;
      }
    }
  }

  /**
   * @brief 显示一个带有确认和取消选项的弹出信息
   * @param _info 要显示的信息
   */
  bool Launcher::popInfo(std::string _info)
  {
    // 静态变量,用于控制初始化和渲染状态
    static bool init = false;
    static bool onRender = false;

    // 如果未初始化,进行初始化设置
    if (!init)
    {
      init = true;
      onRender = true;
    }

    // 计算弹出框的尺寸和位置
    float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin_Selector; // 宽度
    float hPop = HAL::getFontHeight() * 3 + 2 * getUIConfig().popMargin;          // 高度
    float yPop = 0 - hPop - 8;                                                    // 从屏幕上方滑入
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;             // 目标位置 中间偏上
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;                // 居中

    // 取消按键
    float xExit = xPop + getUIConfig().popMargin_Selector / 2; // x轴坐标
    float yExit = yPop;                                        // y轴起始坐标(跟 yPop 一样一开始都在屏幕上方)
    float yExitTrg = yPopTrg + hPop - getUIConfig().popMargin; // y轴目标位置(在弹出框左下方)

    // 确认按键
    float xConfirm = xPop + wPop - getUIConfig().popMargin_Selector - 14; // x轴坐标
    float yConfirm = yExit;                                               // 与取消按钮起始位置一致
    float yConfirmTrg = yExitTrg;                                         // 与取消按钮目标位置一致

    // 选择框(取消键)
    float xSelection = xExit - 2;
    float xSelectionTrg = xSelection;
    float ySelection = yPop;
    std::string temp_0 = "取消";
    float charWidth = HAL::getFontWidth(temp_0);
    float ySelectionTrg = yExitTrg - getUIConfig().popMargin_Selector + 3;
    float wSelection = 0;
    float hSelection = 0;
    float wSelectionTrg = charWidth + 4;
    float hSelectionTrg = getUIConfig().listLineHeight;

    // 按键事件
    uint8_t press_num = 0;
    uint8_t btn_io_num = 0;
    btn_event_t event = btn_not_press;

    // 如果是自定义 page 页面，要通过camera传参来判断是否使用清屏和刷新函数
    std::vector<float> stop_clear = {1.0f, 1.0f};

    // 渲染循环
    while (onRender)
    {
      HAL::canvasClear();
      /*渲染一帧*/
      if (uiType == UIType::Circular)
      {
        // 环形界面刷新
        currentMenu->render(false);
      }
      else
      {
        // 树形界面刷新
        if ((currentMenu->getType() == "Page"))
        {
          currentMenu->render(stop_clear);
        }
        else
        {
          currentMenu->render(camera->getPosition());
          selector->render(camera->getPosition());
          camera->update(currentMenu, selector);
        }
      }
      /*渲染一帧*/

      HAL::setDrawType(0); // 设置为清除模式。之后的绘图操作会清除像素（设为背景色）
      HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
      HAL::setDrawType(1);                                                              // 设置为绘制模式。之后的绘图操作会设置像素（设为前景色）
      HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius); // 绘制一个圆角矩形(边框)
      HAL::drawChinese(xPop + getUIConfig().popMargin_Selector,
                       yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                       _info); // 绘制文字

      HAL::drawChinese(xExit, yExit, "取消");
      HAL::drawChinese(xConfirm, yConfirm, "确认");

      HAL::setDrawType(2); // 绘制选择框。这会反转选择框区域的像素，创造出一种高亮或选中的效果
      HAL::drawRBox(xSelection, ySelection, wSelection, hSelection, 0.5);

      HAL::canvasUpdate();

      Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed); // 动画
      Animation::move(&yExit, yExitTrg, getUIConfig().popSpeed);
      Animation::move(&yConfirm, yConfirmTrg, getUIConfig().popSpeed);
      Animation::move(&xSelection, xSelectionTrg, getUIConfig().popSpeed);
      Animation::move(&ySelection, ySelectionTrg, getUIConfig().popSpeed);
      Animation::move(&wSelection, wSelectionTrg, getUIConfig().popSpeed);
      Animation::move(&hSelection, hSelectionTrg, getUIConfig().popSpeed);

      // 检查按键并直接处理
      std::pair<uint8_t, btn_event_t> keyResult = HAL::keyScan();
      btn_io_num = keyResult.first;
      event = keyResult.second;

      switch (event)
      {
      case btn_click:
        if ((btn_io_num == KEY_BUTTON_2) || (btn_io_num == KEY_BUTTON_3))
        {
          if (press_num == 0)
          {
            // 移动到确认键的位置
            xSelectionTrg = xConfirm - 2;
            press_num = 1;
          }
          else
          {
            xSelectionTrg = xExit - 2;
            press_num = 0;
          }
        }
        else if (btn_io_num == KEY_BUTTON_4)
        {
          // 按下确认键退出
          if ((press_num == 1) && (xSelectionTrg == xSelection))
          {
            yPopTrg = 0 - hPop - 8; // 滑出
            yExitTrg = yPopTrg;
            yConfirmTrg = yPopTrg;
            ySelectionTrg = yPopTrg;
            wSelectionTrg = 0;
            hSelectionTrg = 0;

            // 保存值
            // float temps = static_cast<yomu::NumberEditor*>(currentMenu)->convertToFloat();
          }
          // 按下取消键退出
          else if ((press_num == 0) && (xSelectionTrg == xSelection))
          {
            yPopTrg = 0 - hPop - 8; // 滑出
            yExitTrg = yPopTrg;
            yConfirmTrg = yPopTrg;
            ySelectionTrg = yPopTrg;
            wSelectionTrg = 0;
            hSelectionTrg = 0;
          }
        }

      default:
        break;
      }

      if (yPop == 0 - hPop - 8)
      {
        onRender = false; // 退出条件
        init = false;
      }
    }

    if (press_num == 1)
    {
      // 确认键
      return true;
    }
    else
    {
      // 取消键
      return false;
    }
  }

  /**
   * @brief 显示一个带有确认和取消选项的弹出信息,支持两行文本
   * @param _info1 要显示的第一行信息
   * @param _info2 要显示的第二行信息
   */
  bool Launcher::popInfo(std::string _info1, std::string _info2)
  {
    // 静态变量,用于控制初始化和渲染状态
    static bool init = false;
    static bool onRender = false;

    // 如果未初始化,进行初始化设置
    if (!init)
    {
      init = true;
      onRender = true;
    }

    // 计算弹出框的尺寸和位置
    float wPop = std::max(HAL::getFontWidth(_info1), HAL::getFontWidth(_info2)) + 2 * getUIConfig().popMargin_Selector; // 宽度
    float hPop = HAL::getFontHeight() * 4 + 2 * getUIConfig().popMargin;                                                // 高度
    float yPop = 0 - hPop - 8;                                                                                          // 从屏幕上方滑入
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;                                                   // 目标位置 中间偏上
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;                                                      // 居中

    // 取消按键
    float xExit = xPop + getUIConfig().popMargin_Selector / 2; // x轴坐标
    float yExit = yPop;                                        // y轴起始坐标(跟 yPop 一样一开始都在屏幕上方)
    float yExitTrg = yPopTrg + hPop - getUIConfig().popMargin; // y轴目标位置(在弹出框左下方)

    // 确认按键
    float xConfirm = xPop + wPop - getUIConfig().popMargin_Selector - 14; // x轴坐标
    float yConfirm = yExit;                                               // 与取消按钮起始位置一致
    float yConfirmTrg = yExitTrg;                                         // 与取消按钮目标位置一致

    // 选择框(取消键)
    float xSelection = xExit - 2;
    float xSelectionTrg = xSelection;
    float ySelection = yPop;
    std::string temp_0 = "取消";
    float charWidth = HAL::getFontWidth(temp_0);
    float ySelectionTrg = yExitTrg - getUIConfig().popMargin_Selector + 3;
    float wSelection = 0;
    float hSelection = 0;
    float wSelectionTrg = charWidth + 4;
    float hSelectionTrg = getUIConfig().listLineHeight;

    // 按键事件
    uint8_t press_num = 0;
    uint8_t btn_io_num = 0;
    btn_event_t event = btn_not_press;

    // 如果是自定义 page 页面，要通过camera传参来判断是否使用清屏和刷新函数
    std::vector<float> stop_clear = {1.0f, 1.0f};

    // 渲染循环
    while (onRender)
    {
      HAL::canvasClear();
      /*渲染一帧*/
      if (uiType == UIType::Circular)
      {
        // 环形界面刷新
        currentMenu->render(false);
      }
      else
      {
        // 树形界面刷新
        if ((currentMenu->getType() == "Page"))
        {
          currentMenu->render(stop_clear);
        }
        else
        {
          currentMenu->render(camera->getPosition());
          selector->render(camera->getPosition());
          camera->update(currentMenu, selector);
        }
      }
      /*渲染一帧*/

      HAL::setDrawType(0); // 设置为清除模式。之后的绘图操作会清除像素（设为背景色）
      HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
      HAL::setDrawType(1);                                                              // 设置为绘制模式。之后的绘图操作会设置像素（设为前景色）
      HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius); // 绘制一个圆角矩形(边框)
      HAL::drawChinese(xPop + getUIConfig().popMargin_Selector,
                       yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                       _info1); // 绘制第一行文字
      HAL::drawChinese(xPop + getUIConfig().popMargin_Selector,
                       yPop + getUIConfig().popMargin + HAL::getFontHeight() * 2,
                       _info2); // 绘制第二行文字

      HAL::drawChinese(xExit, yExit, "取消");
      HAL::drawChinese(xConfirm, yConfirm, "确认");

      HAL::setDrawType(2); // 绘制选择框。这会反转选择框区域的像素，创造出一种高亮或选中的效果
      HAL::drawRBox(xSelection, ySelection, wSelection, hSelection, 0.5);

      HAL::canvasUpdate();

      Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed); // 动画
      Animation::move(&yExit, yExitTrg, getUIConfig().popSpeed);
      Animation::move(&yConfirm, yConfirmTrg, getUIConfig().popSpeed);
      Animation::move(&xSelection, xSelectionTrg, getUIConfig().popSpeed);
      Animation::move(&ySelection, ySelectionTrg, getUIConfig().popSpeed);
      Animation::move(&wSelection, wSelectionTrg, getUIConfig().popSpeed);
      Animation::move(&hSelection, hSelectionTrg, getUIConfig().popSpeed);

      // 检查按键并直接处理
      std::pair<uint8_t, btn_event_t> keyResult = HAL::keyScan();
      btn_io_num = keyResult.first;
      event = keyResult.second;

      switch (event)
      {
      case btn_click:
        if ((btn_io_num == KEY_BUTTON_2) || (btn_io_num == KEY_BUTTON_3))
        {
          if (press_num == 0)
          {
            // 移动到确认键的位置
            xSelectionTrg = xConfirm - 2;
            press_num = 1;
          }
          else
          {
            xSelectionTrg = xExit - 2;
            press_num = 0;
          }
        }
        else if (btn_io_num == KEY_BUTTON_4)
        {
          // 按下确认键退出
          if ((press_num == 1) && (xSelectionTrg == xSelection))
          {
            yPopTrg = 0 - hPop - 8; // 滑出
            yExitTrg = yPopTrg;
            yConfirmTrg = yPopTrg;
            ySelectionTrg = yPopTrg;
            wSelectionTrg = 0;
            hSelectionTrg = 0;
          }
          // 按下取消键退出
          else if ((press_num == 0) && (xSelectionTrg == xSelection))
          {
            yPopTrg = 0 - hPop - 8; // 滑出
            yExitTrg = yPopTrg;
            yConfirmTrg = yPopTrg;
            ySelectionTrg = yPopTrg;
            wSelectionTrg = 0;
            hSelectionTrg = 0;
          }
        }

      default:
        break;
      }

      if (yPop == 0 - hPop - 8)
      {
        onRender = false; // 退出条件
        init = false;
      }
    }

    if (press_num == 1)
    {
      // 确认键
      return true;
    }
    else
    {
      // 取消键
      return false;
    }
  }

  /**
   * @brief 初始化启动器
   * @param _rootPage 根页面指针
   */
  void Launcher::init(Menu *_rootPage)
  {
    currentMenu = _rootPage;

    camera = new Camera(0, 0);
    selector = new Selector();
    _rootPage->parent = nullptr; // 根页面的父页面为空

    if (uiType == UIType::Circular)
    {
      // 如果是我自定义的 page 类，需要先初始化
      if ((currentMenu->getType() == "Page"))
      {
        // 当前进入环形界面
        xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_CIRCLE);
        dynamic_cast<Page *>(currentMenu)->init(astra::Menu::ExitDirection::Right);
      }
    }
    else if (uiType == UIType::Tree)
    {
      // 如果是我自定义的 page 类，需要先初始化
      if ((currentMenu->getType() == "Page"))
      {
        // 初始化的时候默认探头号是0
        dynamic_cast<Page *>(currentMenu)->init();
      }
      else if ((currentMenu->getType() == "Tile"))
      {
        // 当前进入树形界面(只有进入Tile界面时，才需要发送探头数据)
        xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);
      }

      _rootPage->childPosInit(camera->getPosition());
      // 它初始化了选择器的位置
      selector->inject(_rootPage);
      camera->init(_rootPage->getType());
    }
  }

  /**
   * @brief 打开上一个页面（环形界面）
   * @return 是否成功打开
   * @warning 仅可调用一次
   */
  bool Launcher::previous()
  {
    if (currentMenu->getPreview() == nullptr)
    {
      popInfo("父页面为空!");
      return false;
    }

    currentMenu->deInit(); // 先析构（退场动画）再挪动指针
    currentMenu = currentMenu->preview;
    dynamic_cast<Page *>(currentMenu)->init(astra::Menu::ExitDirection::Left);
    return true;
  }

  /**
   * @brief 打开下一个页面（环形界面）
   * @return 是否成功打开
   * @warning 仅可调用一次
   */
  bool Launcher::next()
  {
    if (currentMenu->getNextMenu() == nullptr)
    {
      popInfo("子页面为空!");
      return false;
    }

    currentMenu->deInit(); // 先析构（退场动画）再挪动指针
    currentMenu = currentMenu->next;
    dynamic_cast<Page *>(currentMenu)->init(astra::Menu::ExitDirection::Right);
    return true;
  }

  /**
   * @brief 打开选中的页面（树形界面）
   * @return 是否成功打开
   * @warning 仅可调用一次
   */
  bool Launcher::open()
  {
    // 如果当前页面指向的当前item没有后继 那就返回false
    if (currentMenu->childMenu.empty())
    {
      // 如果是自定义 page 类需要使用这种退出方式
      if ((currentMenu->getType() == "Page"))
      {
        popInfo("子页面为空!");
      }
      else
      {
        popInfo("子页面为空!", 300);
      }
      return false;
    }
    if (currentMenu->getType() == "Tile" && currentMenu->getchildMenu()->getType() != "Tile")
    {
      // 界面关系需满足当前界面是Tile界面，但是子界面不为Tile界面
      // 当前界面是Tile界面，但是子界面为其他界面的时候
      xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_ANOTHER);
      xEventGroupClearBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);
    }
    if (currentMenu->getchildMenu()->getType() == "Page")
    {
      // 记住当前的camera坐标(page类没用到)，然后先析构（退场动画）再挪动指针
      currentMenu->rememberCameraPos(camera->getPositionTrg());
      currentMenu->deInit();
      currentMenu = currentMenu->getchildMenu();

      // 初始化新的page类
      dynamic_cast<Page *>(currentMenu)->init();
      return true;
    }
    else if (currentMenu->getchildMenu()->getType() == "Divider")
    {
      return false;
    }
    else
    {
      if (currentMenu->getchildMenu()->childMenu.empty())
      {
        popInfo("子页面下无子页面!", 300);
        return false;
      }

      currentMenu->rememberCameraPos(camera->getPositionTrg());
      currentMenu->deInit(); // 先析构（退场动画）再挪动指针
      currentMenu = currentMenu->getchildMenu();
      currentMenu->forePosInit();
      currentMenu->childPosInit(camera->getPosition());
      selector->inject(currentMenu);

      return true;
    }
  }

  /**
   * @brief 关闭选中的页面
   *
   * @return 是否成功关闭
   * @warning 仅可调用一次
   */
  bool Launcher::close()
  {
    if (currentMenu->getParentMenu() == nullptr)
    {
      if ((currentMenu->getType() == "Page"))
      {
        // 如果是自定义 page 类需要使用这种退出方式
        popInfo("父页面为空!");
      }
      else
      {
        popInfo("父页面为空!", 600);
      }
      return false;
    }
    if (currentMenu->getParentMenu()->getType() == "Tile" && currentMenu->getType() != "Tile")
    {
      // 界面关系需满足当前界面不是Tile界面，但是父界面为Tile界面
      // 当前界面的父界面是Tile磁贴界面时，需要先清除其余界面的标志位，再设置树形界面的标志位
      xEventGroupClearBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_ANOTHER);
      xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);
    }

    currentMenu->rememberCameraPos(camera->getPositionTrg()); // 记录当前页面的相机位置

    currentMenu->deInit(); // 先析构（退场动画）再挪动指针

    currentMenu = currentMenu->getParentMenu(); // 这里其实是把用当前页面的父页面的指针替换掉了

    // 如果当前处于环形页面，也就是从树形页面切换到环形页面，要重新初始化一次（增加动画效果）
    if (uiType == UIType::Circular)
    {
      if ((currentMenu->getType() == "Page"))
      {
        dynamic_cast<Page *>(currentMenu)->init(astra::Menu::ExitDirection::Right);
      }
    }
    else
    {
      // 当关闭当前界面返回的父页面是自定义page界面时（前面已经交换了指针），需要init来初始化动画(非第一次初始化)
      if ((currentMenu->getType() == "Page"))
      {
        dynamic_cast<Page *>(currentMenu)->init();
      }
      else
      {
        currentMenu->forePosInit();                       // 设置前景元素(进度条、箭头、虚线)的目标位置和起始位置
        currentMenu->childPosInit(camera->getPosition()); // 设置子菜单的位置
        selector->inject(currentMenu);
      }
    }

    return true;
  }

  bool Launcher::returnToTile()
  {
    Menu *tileMenu = currentMenu->findTileAncestor();
    if (tileMenu == nullptr)
    {
      popInfo("未找到磁贴页面!", 300);
      return false;
    }

    while (currentMenu != tileMenu)
    {
      currentMenu->rememberCameraPos(camera->getPositionTrg());
      currentMenu->deInit();
      currentMenu = currentMenu->getParentMenu();
    }

    // 重新初始化磁贴页面
    currentMenu->forePosInit();
    currentMenu->childPosInit(camera->getPosition());
    selector->inject(currentMenu);
    // 重新设置标志位
    xEventGroupClearBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_ANOTHER);
    xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);

    return true;
  }

  void Launcher::update()
  {
    uint8_t btn_io_num = 0;
    btn_event_t event = btn_not_press;
    std::vector<float> dont_stop_clear = {0.0f, 0.0f};

    // 队列接收更新探头报警信息
    WarningUpdateMessage updateMessage;
    std::string info;
    const std::string probeNames[] = {"前方", "后方", "左方", "右方"};

    // 获取 ProbeDataManager 的实例
    yomu::ProbeDataManager &probeManager = yomu::ProbeDataManager::getInstance();

    // 处理队列中的所有消息
    while (xQueueReceive(xQueue_ProbeInfoTransfer, &updateMessage, 0) == pdTRUE)
    {
      // 更新探头数据
      probeManager.updateProbeData(updateMessage.label, updateMessage.doseRate, updateMessage.cumulativeDose, updateMessage.p, updateMessage.d);

      uint8_t probeIndex = updateMessage.probeNumber - CAN_PROBE1_ID;
      if (probeIndex < 4)
      {
        if (updateMessage.d || updateMessage.p)
        {
          info = probeNames[probeIndex] + "探头 ";
          info += (updateMessage.d && updateMessage.p) ? "p/d报警" : (updateMessage.p ? "p报警" : "d报警");

          switch (updateMessage.probeNumber)
          {
          case CAN_PROBE1_ID:
            xEventGroupSetBits(xProbeDataSendEventGroup, PROBE1_ALARM_FLAG);
            if (xTimerIsTimerActive(xProbe1AlarmTimer) == pdFALSE)
              key_value_msg("Probe1_Alarm_Start", NULL, 0);
            if ((xEventGroupGetBits(xProbeDataSendEventGroup) & (CURRENT_INTERFACE_FLAG_CIRCLE | CURRENT_INTERFACE_FLAG_TREE)) &&
                (xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE1_POPUP_FLAG))
            {
              popInfo(info, 200);
              xEventGroupClearBits(xProbeDataSendEventGroup, PROBE1_POPUP_FLAG);
            }
            break;
          case CAN_PROBE2_ID:
            xEventGroupSetBits(xProbeDataSendEventGroup, PROBE2_ALARM_FLAG);
            if (xTimerIsTimerActive(xProbe2AlarmTimer) == pdFALSE)
              key_value_msg("Probe2_Alarm_Start", NULL, 0);
            if ((xEventGroupGetBits(xProbeDataSendEventGroup) & (CURRENT_INTERFACE_FLAG_CIRCLE | CURRENT_INTERFACE_FLAG_TREE)) &&
                (xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE2_POPUP_FLAG))
            {
              popInfo(info, 200);
              xEventGroupClearBits(xProbeDataSendEventGroup, PROBE2_POPUP_FLAG);
            }
            break;
          case CAN_PROBE3_ID:
            xEventGroupSetBits(xProbeDataSendEventGroup, PROBE3_ALARM_FLAG);
            if (xTimerIsTimerActive(xProbe3AlarmTimer) == pdFALSE)
              key_value_msg("Probe3_Alarm_Start", NULL, 0);
            if ((xEventGroupGetBits(xProbeDataSendEventGroup) & (CURRENT_INTERFACE_FLAG_CIRCLE | CURRENT_INTERFACE_FLAG_TREE)) &&
                (xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE3_POPUP_FLAG))
            {
              popInfo(info, 200);
              xEventGroupClearBits(xProbeDataSendEventGroup, PROBE3_POPUP_FLAG);
            }
            break;
          case CAN_PROBE4_ID:
            xEventGroupSetBits(xProbeDataSendEventGroup, PROBE4_ALARM_FLAG);
            if (xTimerIsTimerActive(xProbe4AlarmTimer) == pdFALSE)
              key_value_msg("Probe4_Alarm_Start", NULL, 0);
            if ((xEventGroupGetBits(xProbeDataSendEventGroup) & (CURRENT_INTERFACE_FLAG_CIRCLE | CURRENT_INTERFACE_FLAG_TREE)) &&
                (xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE4_POPUP_FLAG))
            {
              popInfo(info, 200);
              xEventGroupClearBits(xProbeDataSendEventGroup, PROBE4_POPUP_FLAG);
            }
            break;
          default:
            break;
          }
        }
        else
        {
          switch (updateMessage.probeNumber)
          {
          case CAN_PROBE1_ID:
            xEventGroupClearBits(xProbeDataSendEventGroup, PROBE1_ALARM_FLAG);
            break;
          case CAN_PROBE2_ID:
            xEventGroupClearBits(xProbeDataSendEventGroup, PROBE2_ALARM_FLAG);
            break;
          case CAN_PROBE3_ID:
            xEventGroupClearBits(xProbeDataSendEventGroup, PROBE3_ALARM_FLAG);
            break;
          case CAN_PROBE4_ID:
            xEventGroupClearBits(xProbeDataSendEventGroup, PROBE4_ALARM_FLAG);
            break;
          default:
            break;
          }
        }
      }
    }

    // 检查所有探头的报警状态
    EventBits_t alarmBits = xEventGroupGetBits(xProbeDataSendEventGroup);
    // alarmBits & (...):如果 alarmBits 中任何对应的位为 1，结果中相应的位也将为 1
    // (...) != 0: 如果结果不为 0，则表示至少有一个位为 1
    bool anyProbeAlarming = (alarmBits & (PROBE1_ALARM_FLAG | PROBE2_ALARM_FLAG | PROBE3_ALARM_FLAG | PROBE4_ALARM_FLAG)) != 0;

    // 根据报警状态控制蜂鸣器
    if (anyProbeAlarming && (alarmBits & BEEP_FLAG_CHECK))
    {
      // 判断蜂鸣器软件定时器是否在运行，如果不在运行，则启动蜂鸣器软件定时器
      if (xTimerIsTimerActive(xBeepTimer) == pdFALSE)
      {
        HAL::beepStart();
      }
    }
    else
    {
      if (xTimerIsTimerActive(xBeepTimer) == pdTRUE)
      {
        HAL::beepStop();
      }
    }

    HAL::canvasClear();

    // 环形界面的更新模式
    if (uiType == UIType::Circular)
    {
      astra::Menu::ExitDirection exit = astra::Menu::ExitDirection::None;
      if (dynamic_cast<Page *>(currentMenu) != nullptr)
      {
        // 通用page类的render函数
        exit = dynamic_cast<Page *>(currentMenu)->render(false);
        // exit = dynamic_cast<yomu::PDXDashboard *>(currentMenu)->render(false);
      }

      if (exit == astra::Menu::ExitDirection::Left)
        previous();
      else if (exit == astra::Menu::ExitDirection::Right)
        next();
    }
    // 树形界面的更新模式
    else if (uiType == UIType::Tree)
    {
      if ((currentMenu->getType() == "Page"))
      {
        dynamic_cast<Page *>(currentMenu)->render(dont_stop_clear);
      }
      else
      {
        currentMenu->render(camera->getPosition());
        if (currentWidget != nullptr)
          currentWidget->render(camera->getPosition());
        selector->render(camera->getPosition());
        camera->update(currentMenu, selector);
      }
    }

    // 检查按键并直接处理
    std::pair<uint8_t, btn_event_t> keyResult = HAL::keyScan();
    btn_io_num = keyResult.first;
    event = keyResult.second;

    switch (event)
    {
    case btn_click:
      if (btn_io_num == KEY_BUTTON_1)
      {
        if (uiType == UIType::Circular)
        {
          // 从环形界面切换到树形界面
          uiType = UIType::Tree;

          // 设置界面切换的标志位
          xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);
          xEventGroupClearBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_CIRCLE);

          open();
        }
        else if (uiType == UIType::Tree)
        {
          if ((currentMenu->getType() == "Page"))
          {
            dynamic_cast<Page *>(currentMenu)->onUp();
          }
          else
          {
            // 从树形界面切换到环形界面
            if (currentMenu->parent->getType() == "Page")
            {
              uiType = UIType::Circular;

              // 设置界面切换的标志位
              xEventGroupSetBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_CIRCLE);
              xEventGroupClearBits(xProbeDataSendEventGroup, CURRENT_INTERFACE_FLAG_TREE);
            }
            close();
          }
        }
      }
      else if (btn_io_num == KEY_BUTTON_2)
      {
        if (uiType == UIType::Circular)
        {
          // 如果是page类或者是page类的子类
          if (dynamic_cast<Page *>(currentMenu) != nullptr)
          {
            dynamic_cast<Page *>(currentMenu)->onLeft();
          }
        }
        else if (uiType == UIType::Tree)
        {
          if ((currentMenu->getType() == "Page"))
          {
            dynamic_cast<Page *>(currentMenu)->onLeft();
          }
          else
            selector->goPreview();
        }
      }
      else if (btn_io_num == KEY_BUTTON_3)
      {
        if (uiType == UIType::Circular)
        {
          // 如果是page类或者是page类的子类
          if (dynamic_cast<Page *>(currentMenu) != nullptr)
          {
            dynamic_cast<Page *>(currentMenu)->onRight();
          }
        }
        else if (uiType == UIType::Tree)
        {
          if ((currentMenu->getType() == "Page"))
          {
            dynamic_cast<Page *>(currentMenu)->onRight();
          }
          else
            selector->goNext();
        }
      }
      else if (btn_io_num == KEY_BUTTON_4)
      {
        if (uiType == UIType::Tree)
        {
          if ((currentMenu->getType() == "Page"))
          {
            // 检查是否按下取消按键
            astra::Page::ConfirmResult is_exit = dynamic_cast<Page *>(currentMenu)->onConfirm(0);
            if (is_exit.isConfirmed)
            {
              close();
            }
            else
            {
              // 检查是否按下确认按键
              is_exit = dynamic_cast<Page *>(currentMenu)->onConfirm(1);
              if (is_exit.isConfirmed)
              {
                if (popInfo("是否设置?"))
                {
                  // 点下确认按键后，找到父页面List
                  Menu *listMenu = currentMenu->findListAncestor();
                  if (listMenu != nullptr)
                  {
                    // 检查父页面List的名字，如果这里是累计剂量阈值，那么就设置累计剂量阈值
                    if (listMenu->title == "累计剂量阈值")
                    {
                      if (is_exit.probeId_text == "探头1")
                      {
                        data.probe1_cumulative_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe1_cumulative_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头2")
                      {
                        data.probe2_cumulative_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe2_cumulative_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头3")
                      {
                        data.probe3_cumulative_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe3_cumulative_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头4")
                      {
                        data.probe4_cumulative_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe4_cumulative_alarm_threshold);
                      }
                    }
                    else if (listMenu->title == "实时剂量阈值")
                    {
                      if (is_exit.probeId_text == "探头1")
                      {
                        data.probe1_realtime_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe1_realtime_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头2")
                      {
                        data.probe2_realtime_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe2_realtime_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头3")
                      {
                        data.probe3_realtime_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe3_realtime_alarm_threshold);
                      }
                      else if (is_exit.probeId_text == "探头4")
                      {
                        data.probe4_realtime_alarm_threshold = is_exit.value;
                        EEPROM_WRITE(data, probe4_realtime_alarm_threshold);
                      }
                    }
                  }
                  // 按下确认设置，等待popinfo动画完成后直接跳回磁贴页面
                  returnToTile();
                }
              }
            }
          }
          else if (currentMenu->getType() == "List")
          {
            // 如果当前菜单的action是ShowPopup，那么就弹出提示菜单
            if (dynamic_cast<List *>(currentMenu)->action == List::ItemAction::ShowPopup)
            {
              int selectedIndex = currentMenu->getSelectedIndex();
              if (selectedIndex != -1)
              {
                if (currentMenu->title == "清空累计剂量")
                {
                  // 弹出提示菜单
                  if (popInfo("是否清空?"))
                  {
                    switch (selectedIndex)
                    {
                    case 0:
                      data.probe1_cumulative_dose = 0.0f;
                      EEPROM_WRITE(data, probe1_cumulative_dose);
                      break;
                    case 1:
                      data.probe2_cumulative_dose = 0.0f;
                      EEPROM_WRITE(data, probe2_cumulative_dose);
                      break;
                    case 2:
                      data.probe3_cumulative_dose = 0.0f;
                      EEPROM_WRITE(data, probe3_cumulative_dose);
                      break;
                    case 3:
                      data.probe4_cumulative_dose = 0.0f;
                      EEPROM_WRITE(data, probe4_cumulative_dose);
                      break;
                    default:
                      break;
                    }
                    // 发送对应探头的CAN命令(清零累计剂量)
                    uint32_t probeId = 0x40 + selectedIndex;
                    key_value_msg("CAN_ClearDose", &probeId, sizeof(probeId));
                    // 按下确认设置，等待popinfo动画完成后直接跳回磁贴页面
                    returnToTile();
                  }
                }
                else if (currentMenu->title == "蜂鸣器开关")
                {
                  // 弹出提示菜单
                  if (popInfo("确认设置?"))
                  {
                    switch (selectedIndex)
                    {
                    case 0:
                      xEventGroupSetBits(xProbeDataSendEventGroup, BEEP_FLAG_CHECK);
                      break;
                    case 1:
                      xEventGroupClearBits(xProbeDataSendEventGroup, BEEP_FLAG_CHECK);
                      break;
                    default:
                      break;
                    }
                    // 按下确认设置，等待popinfo动画完成后直接跳回磁贴页面
                    returnToTile();
                  }
                }
              }
            }
            else
              open();
          }
          else
            open();
        }
      }

      // 如果当前菜单的子菜单项中的子控件不为空，那么就执行对应子控件的操作
      // if (!currentMenu->childMenu[currentMenu->selectIndex]->childWidget.empty())
      // {
      //   Widget *widget = currentMenu->childMenu[currentMenu->selectIndex]->childWidget[0];

      //   if (widget->getType() == "CheckBox")
      //   {
      //     CheckBox *checkBox = dynamic_cast<CheckBox *>(widget);
      //     if (checkBox)
      //     {
      //       bool result = checkBox->toggle();
      //       // 可以根据需要处理 toggle 的结果
      //       if (result)
      //       {
      //         BSP_LED_On(LED_RED);
      //       }
      //       else
      //       {
      //         BSP_LED_Off(LED_RED);
      //       }
      //     }
      //   }
      //   else if (widget->getType() == "Slider")
      //   {
      //     Slider *slider = dynamic_cast<Slider *>(widget);
      //     if (slider)
      //     {
      //       slider->add();
      //       slider->init();
      //     }
      //   }
      //   else if (widget->getType() == "PopUp")
      //   {
      //     PopUp *popup = dynamic_cast<PopUp *>(widget);
      //     if (popup)
      //     {
      //       popup->selectNext();
      //       popup->init();
      //     }
      //   }
      // }

      break;

    case btn_long_press:
      break;

    // 可以根据需要添加其他按键事件的处理
    default:
      break;
    }

    HAL::canvasUpdate();

    // time++;
    time = HAL::millis() / 1000;
  }
}