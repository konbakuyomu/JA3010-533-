//
// Created by Fir on 2024/4/14 014.
//

#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_CAMERA_CAMERA_H_
#define ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_CAMERA_CAMERA_H_

#include "../selector/selector.h"

namespace astra
{

  // 加入了摄像机 随着摄像机动而动
  // 其他的不动的元素 比如虚线和进度条 统称为前景 不受摄像机的控制
  // 这样一来元素本身的坐标并不会改变 只是在渲染的时候加入了摄像机的坐标而已

  // 磁贴类中 前景是虚线 标题 箭头和按钮图标 摄像机横向移动
  // 列表类中 前景是进度条 摄像机纵向移动
  class Camera : public Item
  {
  private:
    float xInit, yInit; // 相机的初始位置

  public:
    float x, y;       // 相机当前位置
    float xTrg, yTrg; // 相机目标位置

    Camera();                   // 构造一个空的相机实例
    Camera(float _x, float _y); // 构造一个指定位置的相机实例

    // 检查给定坐标是否在视图范围外
    // 返回值: 0-在视图内, 1-在视图上方, 2-在视图下方
    unsigned char outOfView(float _x, float _y);
    // 重载版本,接受vector作为参数
    unsigned char outOfView(std::vector<float> _pos) { return outOfView(_pos[0], _pos[1]); }

    [[nodiscard]] std::vector<float> getPosition();    // 获取相机当前位置
    [[nodiscard]] std::vector<float> getPositionTrg(); // 获取相机目标位置

    // 根据菜单类型(List或Tile)初始化相机
    void init(const std::string &_type);

    // 在启动器中新建selector和camera 然后注入menu render
    // 在启动器中执行下述方法即可实现视角移动
    // 启动器要判断是否超过视角范围 若超过则移动摄像机
    // 所有过程中 渲染好的元素绝对坐标都是不变的 只有摄像机的坐标在变

    // 设置相机目标位置
    void go(float _x, float _y);
    void go(const std::vector<float> &_pos);

    // 直接设置相机当前位置和目标位置
    void goDirect(float _x, float _y);

    // 移动相机(改变目标位置)
    void move(float _x, float _y);
    // 直接移动相机(改变当前位置和目标位置)
    void moveDirect(float _x, float _y);

    // 调整相机位置以确保列表中选中项可见(列表单项滚动)
    void goToListItemRolling(List *_menu);
    // 移动相机到选中的磁贴项目
    void goToTileItem(unsigned char _index);

    // 重置相机到初始位置
    void reset();
    void resetDirect();

    // 更新相机位置,考虑菜单和选择器的状态
    void update(Menu *_menu, Selector *_selector);
    // 渲染相机移动,实现平滑过渡
    void render();
  };

}

#endif // ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_CAMERA_CAMERA_H_
