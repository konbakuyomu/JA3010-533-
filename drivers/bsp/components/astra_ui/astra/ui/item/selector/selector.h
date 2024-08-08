//
// Created by Fir on 2024/4/14 014.
//

#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_SELECTOR_SELECTOR_H_
#define ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_SELECTOR_SELECTOR_H_
#include "../menu/menu.h"

namespace astra
{
  class Selector : public Item
  {
  private:
    Menu *menu; // 指向当前菜单的指针,用于访问菜单信息

  public:
    // 列表页中就是选择框的坐标 磁贴页中就是大框的坐标
    float x, xTrg; // x坐标和x目标坐标
    float y, yTrg; // y坐标和y目标坐标

    /* 列表模式相关属性 */
    float w, wTrg; // 宽度和目标宽度
    float h, hTrg; // 高度和目标高度

    /* 磁贴模式相关属性 */
    float yText, yTextTrg; // 磁贴页标题的y坐标和目标y坐标

    Selector() = default;
    // 注: 在磁贴模式中,文字和大框都是selector的一部分
    // 这样设计可以实现磁贴文字的出现动画效果

    // 获取选择器当前位置
    std::vector<float> getPosition();

    // 设置选择器位置(通常是目标位置)
    void setPosition();

    // 移动选择器到指定索引的项目
    void go(unsigned char _index);

    // 移动选择器到下一个项目
    void goNext();

    // 移动选择器到上一个项目
    void goPreview();

    // 注入菜单实例,为渲染做准备
    bool inject(Menu *_menu); 
    // 销毁菜单实例,清理资源
    bool destroy();           

    // 渲染选择器
    // _camera参数用于考虑相机位置进行渲染
    void render(std::vector<float> _camera);
  };
}
#endif // ASTRA_CORE_SRC_ASTRA_UI_ELEMENT_SELECTOR_SELECTOR_H_
