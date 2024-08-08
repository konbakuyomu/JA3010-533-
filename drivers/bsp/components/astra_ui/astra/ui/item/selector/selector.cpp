//
// Created by Fir on 2024/4/14 014.
//

#include "selector.h"

namespace astra
{

  void Selector::setPosition()
  {
    // 注意：此方法通常在 go() 方法中被调用，此时 menu->selectIndex 已经更新

    if (menu->getType() == "Tile") // 磁贴模式
    {
      // 设置选择框的目标 X 坐标
      // 将选择框向左偏移一定距离，以便完全包围磁贴
      xTrg = menu->childMenu[menu->selectIndex]->position.xTrg - astraConfig.tileSelectBoxMargin;

      // 设置选择框的目标 Y 坐标
      // 同样向上偏移，确保选择框正确包围磁贴
      yTrg = menu->childMenu[menu->selectIndex]->position.yTrg - astraConfig.tileSelectBoxMargin;

      // 设置磁贴文字的初始 Y 坐标为屏幕底部
      // 这样可以实现文字从屏幕底部滑入的动画效果
      yText = systemConfig.screenHeight;

      // 设置磁贴文字的目标 Y 坐标
      // 文字最终会停在距离屏幕底部一定距离的位置
      yTextTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin;

      // 设置选择框的目标宽度和高度
      wTrg = astraConfig.tileSelectBoxWidth;
      hTrg = astraConfig.tileSelectBoxHeight;
    }
    else if (menu->getType() == "List") // 列表模式
    {
      // 设置选择框的目标 X 坐标
      // 向左偏移一定距离，以便留出边距
      xTrg = menu->childMenu[menu->selectIndex]->position.xTrg - astraConfig.selectorMargin;

      // 设置选择框的目标 Y 坐标
      // 直接使用列表项的 Y 坐标
      yTrg = menu->childMenu[menu->selectIndex]->position.yTrg;

      // 设置选择框的目标宽度
      // 根据文本宽度动态调整，并在两侧添加边距
      wTrg = (float)HAL::getFontWidth(menu->childMenu[menu->selectIndex]->title) + astraConfig.listTextMargin * 2;

      // 设置选择框的目标高度
      // 使用预定义的列表行高
      hTrg = astraConfig.listLineHeight;
    }
  }

  /**
   * @brief
   *
   * @param _index
   * @note selector接管了移动选择指针的功能
   * @warning not support in loop. 不支持在循环内执行
   */
  void Selector::go(unsigned char _index)
  {
    Item::updateConfig();

    //  if (_index > menu->childMenu.size() - 1) {
    //    if (astraConfig.menuLoop) _index = 0;
    //    else return;
    //  } else if (_index < 0) {
    //    if (astraConfig.menuLoop) _index = menu->childMenu.size() - 1;
    //    else return;
    //  }

    if (_index > menu->childMenu.size() - 1)
      return;
    if (_index < 0)
      return;
    menu->selectIndex = _index; // 这里更新了 selectIndex

    setPosition();
  }

  void Selector::goNext()
  {
    if (this->menu->selectIndex == this->menu->childMenu.size() - 1)
    {
      if (astraConfig.menuLoop)

        go(0);
      else
        return;
    }
    else
      go(menu->selectIndex + 1);
    setPosition();
  }

  void Selector::goPreview()
  {
    if (this->menu->selectIndex == 0)
    {
      if (astraConfig.menuLoop)
        go(this->menu->childMenu.size() - 1);
      else
        return;
    }
    else
      go(menu->selectIndex - 1);
    setPosition();
  }

  bool Selector::inject(Menu *_menu)
  {
    if (_menu == nullptr)
      return false;

    this->menu = _menu;          // 将menu注入到selector中
    go(this->menu->selectIndex); // 注入之后要初始化选择框的位置

    return true;
  }

  bool Selector::destroy()
  {
    if (this->menu == nullptr)
      return false;

    delete this->menu;
    this->menu = nullptr;
    return true; // 添加这行
  }

  // 这里是进行 Tile/List 选择框的初始化
  void Selector::render(std::vector<float> _camera)
  {
    // 更新配置，确保使用最新的设置
    Item::updateConfig();

    // 实现选择器位置和大小的平滑动画
    // 通过逐帧小幅度移动，创造流畅的过渡效果
    Animation::move(&x, xTrg, astraConfig.selectorXAnimationSpeed);
    Animation::move(&y, yTrg, astraConfig.selectorYAnimationSpeed);
    Animation::move(&h, hTrg, astraConfig.selectorHeightAnimationSpeed);
    Animation::move(&w, wTrg, astraConfig.selectorWidthAnimationSpeed);

    if (menu->getType() == "Tile") // 磁贴模式
    {
      // 实现文本Y坐标的平滑动画
      Animation::move(&yText, yTextTrg, astraConfig.selectorYAnimationSpeed);

      // 绘制文本
      // 注意：文本位置不受相机影响，保持固定位置,从下面的 drawChinese 就看得出来，x和y并没有受到相机的影响
      HAL::setDrawType(1); // 设置绘制类型为文本
      // 计算文本X坐标，使其居中显示
      float textX = (systemConfig.screenWeight -
                     (float)HAL::getFontWidth(menu->childMenu[menu->selectIndex]->title)) /
                    2.0;
      // 绘制中文文本
      HAL::drawChinese(textX, yText + astraConfig.tileTitleHeight,
                       menu->childMenu[menu->selectIndex]->title);

      // 绘制选择框
      // 注意：选择框位置需要考虑相机位置
      HAL::setDrawType(2); // 设置绘制类型为图形

      // 绘制选择框的四个角，每个角由两条垂直线段组成
      // 左上角
      HAL::drawPixel(x + _camera[0], y + _camera[1]); // 绘制角点
      HAL::drawHLine(x + _camera[0], y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);
      HAL::drawVLine(x + _camera[0], y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);

      // 左下角
      HAL::drawHLine(x + _camera[0], y + _camera[1] + h - 1, astraConfig.tileSelectBoxLineLength + 1);
      HAL::drawVLine(x + _camera[0],
                     y + _camera[1] + h - astraConfig.tileSelectBoxLineLength - 1,
                     astraConfig.tileSelectBoxLineLength);

      // 右上角
      HAL::drawHLine(x + _camera[0] + w - astraConfig.tileSelectBoxLineLength - 1,
                     y + _camera[1],
                     astraConfig.tileSelectBoxLineLength);
      HAL::drawVLine(x + _camera[0] + w - 1, y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);

      // 右下角
      HAL::drawHLine(x + _camera[0] + w - astraConfig.tileSelectBoxLineLength - 1,
                     y + _camera[1] + h - 1,
                     astraConfig.tileSelectBoxLineLength);
      HAL::drawVLine(x + _camera[0] + w - 1,
                     y + _camera[1] + h - astraConfig.tileSelectBoxLineLength - 1,
                     astraConfig.tileSelectBoxLineLength);

      // 绘制右下角的像素点，确保完整性
      HAL::drawPixel(x + _camera[0] + w - 1, y + _camera[1] + h - 1);
    }
    else if (menu->getType() == "List") // 列表模式
    {
      // 绘制选择框
      // 注意：选择框位置需要考虑相机位置
      HAL::setDrawType(2); // 设置绘制类型为图形
      // 绘制圆角矩形作为选择框
      HAL::drawRBox(x + _camera[0],
                    y + _camera[1] + astraConfig.listTextMargin - astraConfig.selectorTopMargin * 2, w, h - 1,
                    astraConfig.selectorRadius);

      HAL::setDrawType(1); // 重置绘制类型为默认
    }
  }

  std::vector<float> Selector::getPosition()
  {
    return {xTrg, yTrg};
  }
}