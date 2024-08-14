//
// Created by Fir on 2024/2/2.
//
#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_
#define ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_

#include "item/camera/camera.h"
#include "item/page/page.h"

namespace astra
{

  // 使用 enum class 清楚地表明这是一个强类型的枚举，不同类型的枚举之间不能直接比较
  // 这里是界面切换的模式
  enum class UIType
  {
    Circular, // 环形界面
    Tree      // 树形界面
  };

  // 定义一个名为 Launcher 的类，用于管理和控制界面切换
  class Launcher
  {
  private:
    Menu *currentMenu;
    Widget *currentWidget = nullptr;
    Selector *selector;
    Camera *camera;

    uint64_t time;

  public:
    void popInfo(std::string _info, uint16_t _time); // 带时间参数
    bool popInfo(std::string _info);                 // 不带时间参数

    void init(Menu *_rootPage); // 初始化函数，传入根页面

    // 环形界面的前一个界面或者后一个界面
    bool previous(); // 切换到前一个界面
    bool next();     // 切换到后一个界面

    // 树形界面的打开和关闭
    bool open();  // 打开当前选中的子菜单或项目
    bool close(); // 关闭当前菜单，返回上一级
    bool returnToTile(); // 直接返回到Tile界面

    void update(); // 更新函数，可能用于刷新界面或处理逻辑

    Camera *getCamera() { return camera; }       // 获取相机对象的指针
    Selector *getSelector() { return selector; } // 获取选择器对象的指针

    UIType uiType = UIType::Circular; // 默认是环形界面
  };
}

#endif // ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_