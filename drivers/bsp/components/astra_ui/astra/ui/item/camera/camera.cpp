//
// Created by Fir on 2024/4/14 014.
//

#include "camera.h"

#include <cmath>

namespace astra
{
  Camera::Camera()
  {
    this->xInit = 0;
    this->yInit = 0;

    this->x = 0;
    this->y = 0;

    this->xTrg = 0;
    this->yTrg = 0;
  }

  // 这里的坐标应该都是负的 因为最终渲染的时候是加上摄像机的坐标
  // 所以说比如想显示下一页 应该是item本身的坐标减去摄像机的坐标 这样才会让item向上移动
  // 一个办法是用户传进来正的坐标 但是在摄像机内部 所有坐标都取其相反数 负的

  /* 相机的作用 */
  // 1.控制视图: 决定用户在界面中看到的内容范围。
  // 2.实现滚动效果: 通过移动相机位置,创造界面滚动的效果。
  // 3.管理大型界面: 允许界面内容大于实际屏幕尺寸,通过相机移动来查看不同部分。

  Camera::Camera(float _x, float _y)
  {
    this->xInit = 0 - _x;
    this->yInit = 0 - _y;

    this->x = 0 - _x;
    this->y = 0 - _y;

    this->xTrg = 0 - _x;
    this->yTrg = 0 - _y;
  }

  /**
   *
   * @param _x
   * @param _y
   * @return 0: in view, 1: upper, 2: lower
   */
  unsigned char Camera::outOfView(float _x, float _y)
  {
    // 检查是否在视图左上方
    if (_x < (0 - this->x) || _y < (0 - this->y))
    {
      return 1;
    }

    // 检查是否在视图右下方
    if (_x > (0 - this->x) + systemConfig.screenWeight - 1 ||
        _y > (0 - this->y) + systemConfig.screenHeight - 1)
    {
      return 2;
    }

    // 在视图内
    return 0;
  }

  std::vector<float> Camera::getPosition()
  {
    return {x, y};
  }

  std::vector<float> Camera::getPositionTrg()
  {
    return {xTrg, yTrg};
  }

  void Camera::init(const std::string &_type)
  {
    if (_type == "List")
    {
      this->goDirect(0, static_cast<float>((0 - sys::getSystemConfig().screenHeight) * 10));
      // this->render();
    }
    else if (_type == "Tile")
    {
      this->goDirect(static_cast<float>((0 - sys::getSystemConfig().screenWeight) * 10), 0);
      // this->render();
    }
  }

  /**
   * @brief
   * @param _pos
   * @param _posTrg
   * @param _speed
   *
   * @note only support in loop. 仅支持在循环内执行
   * @note 函数设置相机的目标位置。注意这里使用了负值，这是因为相机移动实际上是移动整个场景的反方向
   */
  void Camera::go(float _x, float _y)
  {
    this->xTrg = 0 - _x;
    this->yTrg = 0 - _y;
  }

  void Camera::go(const std::vector<float> &_pos)
  {
    this->xTrg = 0 - _pos[0];
    this->yTrg = 0 - _pos[1];
  }

  void Camera::goDirect(float _x, float _y)
  {
    this->x = 0 - _x;
    this->y = 0 - _y;
    this->xTrg = 0 - _x;
    this->yTrg = 0 - _y;
  }

  void Camera::move(float _x, float _y)
  {
    this->xTrg -= _x;
    this->yTrg -= _y;
  }

  void Camera::moveDirect(float _x, float _y)
  {
    this->x -= _x;
    this->y -= _y;
    this->xTrg -= _x;
    this->yTrg -= _y;
  }

  // 确保用户选中的项目总是可以在屏幕上看到。
  // 如果用户选择了一个当前不在屏幕上的项目,这个函数会移动整个列表,让那个项目出现在屏幕上
  void Camera::goToListItemRolling(List *_menu)
  {
    // 计算每页最大项目数
    // systemConfig.screenHeight：这个值代表整个屏幕的高度（以像素为单位
    // astraConfig.listLineHeight：这个值代表列表中每个项目的高度（以像素为单位）
    // 通过将屏幕高度除以每个项目的高度，我们就能得到屏幕上可以容纳的项目数量
    static const unsigned char maxItemPerPage = systemConfig.screenHeight / astraConfig.listLineHeight;

    // 第一次进入的时候初始化 退出页面记住坐标 再次进入就OK了
    if (!_menu->initFlag)
    {
      go(0, 0);
      _menu->initFlag = true;
    }

    // 这是在检查:用户选中的项目是不是在屏幕可见范围的上面
    if (_menu->selectIndex < _menu->getBoundary()[0])
    {
      // 注意这里是go不是move（move 方法通常用于相对移动，即从当前位置移动一定距离
      // 这里我们只需要微调相机位置，而不是跳转到绝对位置）
      // 如果是,就向上移动列表,移动的距离刚好让选中的项目出现在屏幕顶部
      // _menu->selectIndex 是当前选中项的索引
      // _menu->getBoundary()[0] 是当前屏幕顶部项目的索引
      // _menu->selectIndex - _menu->getBoundary()[0] 是当前选中项目和屏幕顶部项目之间的差值
      // 乘以 astraConfig.listLineHeight（每个项目的高度）得到精确的像素移动距离
      move(0, (_menu->selectIndex - _menu->getBoundary()[0]) * astraConfig.listLineHeight);
      _menu->refreshBoundary(_menu->selectIndex, _menu->selectIndex + maxItemPerPage - 1);
      return;
    }
    // 这是在检查:用户选中的项目是不是在屏幕可见范围的下面
    else if (_menu->selectIndex > _menu->getBoundary()[1])
    {
      // 如果是,就向下移动列表,移动的距离刚好让选中的项目出现在屏幕底部
      // _menu->selectIndex 是当前选中项的索引
      // _menu->getBoundary()[1] 是当前屏幕底部项目的索引
      // _menu->selectIndex - _menu->getBoundary()[1] 是当前选中项目和屏幕底部项目之间的差值
      // 乘以 astraConfig.listLineHeight（每个项目的高度）得到精确的像素移动距离
      move(0, (_menu->selectIndex - _menu->getBoundary()[1]) * astraConfig.listLineHeight);
      _menu->refreshBoundary(_menu->selectIndex - maxItemPerPage + 1, _menu->selectIndex);
      return;
    }
    else
      return;
  }

  void Camera::goToTileItem(unsigned char _index)
  {
    go(_index * (astraConfig.tilePicWidth + astraConfig.tilePicMargin), 0);
  }

  void Camera::reset()
  {
    go(this->xInit, this->yInit);
  }

  void Camera::resetDirect()
  {
    goDirect(this->xInit, this->yInit);
  }

  void Camera::render()
  {
    Animation::move(&this->x, this->xTrg, astraConfig.cameraAnimationSpeed);
    Animation::move(&this->y, this->yTrg, astraConfig.cameraAnimationSpeed);
  }

  void Camera::update(Menu *_menu, Selector *_selector)
  {
    // 1. 检查并恢复记忆的相机位置
    if (_menu->cameraPosMemoryFlag)
    {
      // 如果有记忆的相机位置,则移动到记忆的位置
      go(0 - _menu->getCameraMemoryPos()[0], 0 - _menu->getCameraMemoryPos()[1]);
      _menu->cameraPosMemoryFlag = false;
      _menu->resetCameraMemoryPos();
    }

    // 2. 根据菜单类型调整相机位置
    // if (this->isReached(_menu->getCameraMemoryPos())) _menu->cameraPosMemoryFlag = false;
    if (_menu->getType() == "List")
      // 对于列表类，确保选中的项目总是可以在屏幕上看到
      goToListItemRolling(dynamic_cast<List *>(_menu));
    else if (_menu->getType() == "Tile")
      // 对于磁贴类型,移动到选中的磁贴
      goToTileItem(_menu->selectIndex);
    else if (_menu->getType() == "Page")
      go(0, 0);

    // 3. 渲染相机位置变化
    this->render();
  }
}