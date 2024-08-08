//
// Created by Fir on 2024/1/25.
//

#pragma once
#ifndef ASTRA_CORE_SRC_SYSTEM_H_
#define ASTRA_CORE_SRC_SYSTEM_H_

#include "u8g2.h"

namespace astra
{
  /**
   * @brief config of astra ui. astra ui的配置结构体
   */
  struct config
  {
    // 动画速度设置（值越大，动画越快）
    float tileAnimationSpeed = 90;             // 磁贴动画速度
    float listAnimationSpeed = 90;             // 列表动画速度
    float selectorYAnimationSpeed = 90;        // 选择器Y轴动画速度
    float selectorXAnimationSpeed = 90;        // 选择器X轴动画速度
    float selectorWidthAnimationSpeed = 90;    // 选择器宽度动画速度
    float selectorHeightAnimationSpeed = 95;   // 选择器高度动画速度
    float windowAnimationSpeed = 55;           // 窗口动画速度
    float sideBarAnimationSpeed = 55;          // 侧边栏动画速度
    float fadeAnimationSpeed = 100;            // 淡入淡出动画速度
    float cameraAnimationSpeed = 90;           // 相机动画速度
    float logoAnimationSpeed = 85;             // Logo动画速度
    float numberEditorCharAnimationSpeed = 92; // 数字选择框字符动画速度

    // 展开设置
    bool tileUnfold = true; // 磁贴是否展开
    bool listUnfold = true; // 列表是否展开

    bool menuLoop = true; // 菜单是否循环

    // 视觉效果设置
    bool backgroundBlur = true; // 是否启用背景模糊
    bool lightMode = false;     // 是否使用亮色模式

    // 列表样式设置
    float listBarWeight = 5;   // 列表条宽度
    float listTextHeight = 8;  // 列表文本高度
    float listTextMargin = 4;  // 列表文本边距
    float listLineHeight = 16; // 列表行高

    // 选择器样式设置
    float selectorRadius = 0.5f; // 选择器圆角半径
    float selectorMargin = 4;    // 选择器与文字左边距
    float selectorTopMargin = 2; // 选择器与文字上边距

    // 磁贴样式设置
    float tilePicWidth = 30;    // 磁贴图片宽度
    float tilePicHeight = 30;   // 磁贴图片高度
    float tilePicMargin = 8;    // 磁贴图片边距
    float tilePicTopMargin = 8; // 磁贴图标上边距
    float tileArrowWidth = 6;   // 磁贴箭头宽度
    float tileArrowMargin = 4;  // 磁贴箭头边距

    float tileDottedLineBottomMargin = 18; // 磁贴虚线下边距
    float tileArrowBottomMargin = 8;       // 磁贴箭头下边距
    float tileTextBottomMargin = 12;       // 磁贴标题下边距

    float tileBarHeight = 2; // 磁贴进度条高度

    float tileSelectBoxLineLength = 5;                                   // 磁贴选择框线长
    float tileSelectBoxMargin = 3;                                       // 磁贴选择框边距
    float tileSelectBoxWidth = tileSelectBoxMargin * 2 + tilePicWidth;   // 磁贴选择框宽度
    float tileSelectBoxHeight = tileSelectBoxMargin * 2 + tilePicHeight; // 磁贴选择框高度
    float tileTitleHeight = 8;                                           // 磁贴标题高度

    float tileBtnMargin = 16; // 磁贴按钮边距

    // 弹窗设置
    float popMargin = 4;           // 弹窗边距
    float popRadius = 2;           // 弹窗圆角半径
    float popSpeed = 90;           // 弹窗动画速度
    float popMargin_Selector = 15; // 选择器弹窗边距

    // Logo设置
    float logoStarLength = 2;      // Logo星星长度
    float logoTextHeight = 14;     // Logo文字高度
    float logoCopyRightHeight = 8; // Logo版权文字高度
    unsigned char logoStarNum = 8; // Logo星星数量

    // 字体设置
    const unsigned char *logoTitleFont = u8g2_my_font_16;     // Logo标题字体
    const unsigned char *logoCopyRightFont = u8g2_my_font_13; // Logo版权字体
    const unsigned char *mainFont = u8g2_my_font_12;          // 主要字体

    // 复选框设置
    float checkBoxWidth = 8;        // 复选框宽度
    float checkBoxHeight = 8;       // 复选框高度
    float checkBoxTopMargin = 4;    // 复选框与选项上边缘的距离
    float checkBoxRightMargin = 10; // 复选框与屏幕右边缘的距离
    float checkBoxRadius = 1;       // 复选框圆角半径

    // 数字编辑页面设置
    float numberEditorCharMargin = 4;                  // 字符之间的间隔像素数
    float numberEditorCharMoveLengthwithoutDot = 12.5; // 字符之间选中框每次移动的长度(中间没有小数点)
    float numberEditorCharMoveLengthwithDot = 20;      // 字符之间选中框每次移动的长度(中间有小数点)
  };

  static config &getUIConfig()
  {
    static config astraConfig;
    return astraConfig;
  }
}
#endif // ASTRA_CORE_SRC_SYSTEM_H_