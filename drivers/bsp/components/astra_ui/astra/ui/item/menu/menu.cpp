//
// Created by Fir on 2024/1/21.
//

// 每个列表应该有 1. 文本（需要显示的内容） 2. 跳转向量表（对应1，每一行都要跳转到哪里）
// 其中文本以string形式 向量表以vector形式 以类的形式整合在一起 编写相应的构造函数 调用构造函数即是新建一个页面
// 通过最后一项跳转向量表可以判断其类型 比如跳转至下一页 或者返回上一级 或者是复选框 或者是弹窗 或者是侧边栏弹窗 等等
// 做一个astra类作为总的框架 在ui命名空间里

// 分为
// 驱动层 - 界面层（包括各种列表 磁铁 复选框 侧边栏 弹窗等） - 处理层（各种坐标的变换等）

// 传进去的有三个东西 第一个是文字（要显示的东西） 第二个是类别（普通列表 跳转列表 复选框） 第三个是特征向量（指向复选框和弹窗对应的参数 要跳转的地方等）

#include "menu.h"

/**
 *     ·  ·     ·   ·
 *  ·   ·  ·  ·   ·
 *  循此苦旅，直抵群星。
 *  ad astra per aspera.
 * happy Chinese new year!
 *      新年快乐！
 * ·   ·   ·      ·
 *   ·   ·    · ·   ·
 */

namespace astra
{
  Menu::Position Menu::getItemPosition(unsigned char _index) const { return childMenu[_index]->position; }

  std::vector<unsigned char> Menu::generateDefaultPic()
  {
    this->picDefault.resize(120, 0xFF);
    return this->picDefault;
  }

  unsigned char Menu::getItemNum() const { return childMenu.size(); }

  Menu *Menu::getNextMenu() const { return childMenu[selectIndex]; }

  Menu *Menu::getPreview() const { return parent; }

  void Menu::deInit()
  {
    // todo 未实现完全
    Animation::exit();
  }

  bool Menu::addMenu(Menu *_page)
  {
    if (_page == nullptr)
      return false;

    this->next = _page;
    _page->preview = this;

    return true;
  }

  bool Menu::addItem(Menu *_page)
  {
    if (_page == nullptr)
      return false;
    if (!_page->childWidget.empty())
      return false;

    _page->parent = this;             // 使子页面的父页面为自己
    this->childMenu.push_back(_page); // 更新自己的子页面列表
    this->forePosInit();              // 更新自己的坐标
    return true;
  }

  bool Menu::addItem(Menu *_page, Widget *_anyWidget)
  {
    if (_anyWidget == nullptr)
      return false;
    if (this->getType() == "Tile" && _page->getType() == "Divider")
      return false;
    if (this->getType() == "Widget")
      return false;
    if (this->addItem(_page))
    {
      _page->childWidget.push_back(_anyWidget);
      _anyWidget->parent = _page;
      _anyWidget->init();
      return true;
    }
    else
      return false;
  }

  Divider::Divider(const std::string &_title)
  {
    title = _title;
  }

  void List::childPosInit(const std::vector<float> &_camera)
  {
    // 用于追踪当前处理的子菜单项索引
    unsigned char _index = 0;

    // 遍历所有子菜单项
    for (auto _iter : childMenu)
    {
      // Divider相当于是一个类别名字，要靠左边一点
      if (_iter->getType() == "Divider")
      {
        // 设置X坐标和目标X坐标为预定义的列表文本边距
        _iter->position.x = astraConfig.listTextMargin;
        _iter->position.xTrg = astraConfig.listTextMargin;
      }
      else
      {
        _iter->position.x = astraConfig.listTextMargin + 10;
        _iter->position.xTrg = astraConfig.listTextMargin + 10;
      }

      // 计算目标Y坐标:
      // 根据当前索引和预定义的行高来确定每个项目的垂直位置
      _iter->position.yTrg = _index * astraConfig.listLineHeight;

      // 增加索引,为下一个项目做准备
      _index++;

      // 处理特殊情况和动画效果

      // 如果是顶级菜单项(没有父级的父级)
      if (_iter->parent->parent == nullptr)
      {
        // 直接设置Y坐标为目标坐标,不需要动画
        // 这是因为根页面有自己的开场动画
        _iter->position.y = _iter->position.yTrg;
        continue; // 跳过剩余的处理
      }

      // 如果配置为展开模式
      if (astraConfig.listUnfold)
      {
        // 设置初始Y坐标在屏幕顶部外部,准备从顶部滑入
        _iter->position.y = _camera[1] - astraConfig.listLineHeight;
        continue; // 跳过剩余的处理
      }
      // 注: 如果不是展开模式,Y坐标将保持为默认值(0)
    }
  }

  void List::forePosInit()
  {
    // 设置列表前景条（bar）的目标 X 坐标
    // 将其放置在屏幕右侧，留出一定宽度
    positionForeground.xBarTrg = systemConfig.screenWeight - astraConfig.listBarWeight;

    // 判断列表是否展开
    if (astraConfig.listUnfold)
    {
      // 如果列表设置为展开状态
      positionForeground.hBar = 0; // 将高度设为0，准备从顶部滑入
    }
    else
    {
      // 如果列表设置为折叠状态
      positionForeground.hBar = positionForeground.hBarTrg; // 直接设置为目标高度，无需动画
    }

    // 初始化前景条的 X 坐标
    // 将其设置在屏幕右侧外部，准备滑入动画
    positionForeground.xBar = systemConfig.screenWeight;
  }

  List::List()
  {
    this->title = "-unknown";
    this->pic = generateDefaultPic();

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  List::List(const std::string &_title)
  {
    this->title = _title;
    this->pic = generateDefaultPic();

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  List::List(const std::vector<unsigned char> &_pic)
  {
    this->title = "-unknown";
    this->pic = _pic;

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  List::List(const std::string &_title, const std::vector<unsigned char> &_pic)
  {
    this->title = _title;
    this->pic = _pic;

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  void List::render(const std::vector<float> &_camera)
  {
    // 更新配置信息
    Item::updateConfig();

    // 设置绘图类型为1（通常表示正常绘制）
    HAL::setDrawType(1);

    // 遍历所有子菜单项
    for (auto _iter : childMenu)
    {
      // 如果子菜单项有关联的控件
      if (!_iter->childWidget.empty())
      {
        for (auto _widget : _iter->childWidget)
        {
          // 在列表右侧绘制每个控件的指示器
          _widget->renderIndicator(
              systemConfig.screenWeight - astraConfig.checkBoxRightMargin - astraConfig.checkBoxWidth,
              _iter->position.y + astraConfig.checkBoxTopMargin,
              _camera);
        }
      }

      // 绘制菜单项的文字
      HAL::drawChinese(_iter->position.x + _camera[0],
                       _iter->position.y + astraConfig.listTextHeight +
                           astraConfig.listTextMargin + _camera[1],
                       _iter->title);

      // 使用动画效果移动菜单项到目标位置
      Animation::move(&_iter->position.y, _iter->position.yTrg, astraConfig.listAnimationSpeed);
    }

    // 计算滚动条的目标高度
    positionForeground.hBarTrg = (selectIndex + 1) * ((float)systemConfig.screenHeight / getItemNum());

    // 绘制滚动条的边框线
    HAL::drawHLine(systemConfig.screenWeight - astraConfig.listBarWeight, 0, astraConfig.listBarWeight);
    HAL::drawHLine(systemConfig.screenWeight - astraConfig.listBarWeight,
                   systemConfig.screenHeight - 1,
                   astraConfig.listBarWeight);
    HAL::drawVLine(systemConfig.screenWeight - ceil((float)astraConfig.listBarWeight / 2.0f),
                   0,
                   systemConfig.screenHeight);

    // 绘制滚动条
    HAL::drawBox(positionForeground.xBar, 0, astraConfig.listBarWeight, positionForeground.hBar);

    // 如果启用了亮色模式
    if (astraConfig.lightMode)
    {
      // 设置绘图类型为2（高亮）
      HAL::setDrawType(2);
      // 绘制覆盖整个屏幕的矩形
      HAL::drawBox(0, 0, systemConfig.screenWeight, systemConfig.screenHeight);
      // 恢复正常绘图类型
      HAL::setDrawType(1);
    }

    // 使用动画效果更新滚动条的高度和位置
    Animation::move(&positionForeground.hBar, positionForeground.hBarTrg, astraConfig.listAnimationSpeed);
    Animation::move(&positionForeground.xBar, positionForeground.xBarTrg, astraConfig.listAnimationSpeed);
  }

  void Tile::childPosInit(const std::vector<float> &_camera)
  {
    // 用于追踪当前处理的子菜单项索引
    unsigned char _index = 0;

    // 遍历所有子菜单项
    for (auto _iter : childMenu)
    {
      // 初始化Y坐标为0
      _iter->position.y = 0;

      // 计算目标X坐标:
      // 从屏幕中心开始,减去图标宽度的一半,然后根据索引添加偏移
      _iter->position.xTrg = systemConfig.screenWeight / 2 - astraConfig.tilePicWidth / 2 +
                             (_index) * (astraConfig.tilePicMargin + astraConfig.tilePicWidth);

      // 设置目标Y坐标为预定义的图标上边距
      _iter->position.yTrg = astraConfig.tilePicTopMargin;

      // 增加索引,为下一个项目做准备
      _index++;

      // 如果是顶级菜单项(没有父级的父级)
      if (_iter->parent->parent == nullptr)
      {
        // 直接设置X坐标为目标坐标,不需要动画
        _iter->position.x = _iter->position.xTrg;
        continue; // 跳过剩余的处理
      }

      // 如果配置为展开模式
      if (astraConfig.tileUnfold)
      {
        // 设置初始X坐标在屏幕左侧外部,准备从左侧滑入
        _iter->position.x = _camera[0] - astraConfig.tilePicWidth;
        continue; // 跳过剩余的处理
      }
      // 注: 如果不是展开模式,X坐标将保持不变
    }
  }

  // 设置前景元素(进度条、箭头、虚线)的目标位置
  // 初始化这些元素的起始位置,为之后的动画效果做准备
  void Tile::forePosInit()
  {
    // 设置进度条的目标 Y 坐标为 0（屏幕顶部）
    positionForeground.yBarTrg = 0;
    // 设置箭头的目标 Y 坐标为屏幕底部减去箭头的底部边距
    positionForeground.yArrowTrg = systemConfig.screenHeight - astraConfig.tileArrowBottomMargin;
    // 设置虚线的目标 Y 坐标为屏幕底部减去虚线的底部边距
    positionForeground.yDottedLineTrg = systemConfig.screenHeight - astraConfig.tileDottedLineBottomMargin;

    // 磁贴是否展开
    if (astraConfig.tileUnfold)
      positionForeground.wBar = 0; // 如果设置为展开,初始宽度为 0,之后会从左侧展开
    else
      positionForeground.wBar = positionForeground.wBarTrg;

    // position.y = -astraConfig.tilePicHeight * 2;

    // 设置箭头的初始 Y 坐标为屏幕底部（之后会向上移动）
    positionForeground.yArrow = systemConfig.screenHeight;
    // 设置虚线的初始 Y 坐标为屏幕底部（之后会向上移动）
    positionForeground.yDottedLine = systemConfig.screenHeight;
    // 顶部进度条的从上方滑入的初始化，设置进度条的初始 Y 坐标为屏幕上方（负值,之后会向下滑入）
    // 注意这里是坐标从屏幕外滑入 而不是height从0变大
    positionForeground.yBar = 0 - astraConfig.tileBarHeight;
  }

  Tile::Tile()
  {
    this->title = "-unknown";
    this->pic = generateDefaultPic();

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  Tile::Tile(const std::string &_title)
  {
    this->title = _title;
    this->pic = generateDefaultPic();

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  Tile::Tile(const std::vector<unsigned char> &_pic)
  {
    this->title = "-unknown";
    this->pic = _pic;

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  Tile::Tile(const std::string &_title, const std::vector<unsigned char> &_pic)
  {
    this->title = _title;
    this->pic = _pic;

    this->selectIndex = 0;

    // this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();

    this->position = {};
    this->positionForeground = {};
  }

  void Tile::render(const std::vector<float> &_camera)
  {
    // 更新配置信息
    Item::updateConfig();

    // 设置绘图默认颜色
    HAL::setDrawType(1);

    // 绘制每个子菜单项的图片
    for (auto _iter : childMenu)
    {
      // 绘制位图,考虑相机位置进行偏移
      // _iter->position.x + _camera[0]：这将磁贴的 x 坐标与相机的 x 坐标相加。
      // 如果相机向右移动（_camera[0] 为负值），磁贴就会向左移动，反之亦然
      HAL::drawBMP(_iter->position.x + _camera[0],
                   astraConfig.tilePicTopMargin + _camera[1],
                   astraConfig.tilePicWidth,
                   astraConfig.tilePicHeight,
                   _iter->pic.data());

      // 这里的xTrg在addItem的时候就已经确定了（也就是各自对应的forePosInit函数内）
      // 平滑移动每个项目到目标位置
      Animation::move(&_iter->position.x,
                      _iter->position.xTrg,
                      astraConfig.tileAnimationSpeed);
    }

    // 计算并绘制顶部进度条
    positionForeground.wBarTrg = (selectIndex + 1) * ((float)systemConfig.screenWeight / getItemNum());
    HAL::drawBox(0, positionForeground.yBar, positionForeground.wBar, astraConfig.tileBarHeight);

    // 绘制左箭头
    HAL::drawHLine(astraConfig.tileArrowMargin, positionForeground.yArrow, astraConfig.tileArrowWidth);
    HAL::drawPixel(astraConfig.tileArrowMargin + 1, positionForeground.yArrow + 1);
    HAL::drawPixel(astraConfig.tileArrowMargin + 2, positionForeground.yArrow + 2);
    HAL::drawPixel(astraConfig.tileArrowMargin + 1, positionForeground.yArrow - 1);
    HAL::drawPixel(astraConfig.tileArrowMargin + 2, positionForeground.yArrow - 2);

    // 绘制右箭头
    HAL::drawHLine(systemConfig.screenWeight - astraConfig.tileArrowWidth - astraConfig.tileArrowMargin,
                   positionForeground.yArrow,
                   astraConfig.tileArrowWidth);
    HAL::drawPixel(systemConfig.screenWeight - astraConfig.tileArrowWidth, positionForeground.yArrow + 1);
    HAL::drawPixel(systemConfig.screenWeight - astraConfig.tileArrowWidth - 1, positionForeground.yArrow + 2);
    HAL::drawPixel(systemConfig.screenWeight - astraConfig.tileArrowWidth, positionForeground.yArrow - 1);
    HAL::drawPixel(systemConfig.screenWeight - astraConfig.tileArrowWidth - 1, positionForeground.yArrow - 2);

    // 绘制左按钮
    HAL::drawHLine(astraConfig.tileBtnMargin, positionForeground.yArrow + 2, 9);
    HAL::drawBox(astraConfig.tileBtnMargin + 2, positionForeground.yArrow + 2 - 4, 5, 4);

    // 绘制右按钮
    HAL::drawHLine(systemConfig.screenWeight - astraConfig.tileBtnMargin - 9, positionForeground.yArrow + 2, 9);
    HAL::drawBox(systemConfig.screenWeight - astraConfig.tileBtnMargin - 9 + 2,
                 positionForeground.yArrow + 2 - 4,
                 5,
                 4);

    // 绘制底部虚线
    HAL::drawHDottedLine(0, positionForeground.yDottedLine, systemConfig.screenWeight);

    // 动画更新各元素位置
    Animation::move(&positionForeground.yDottedLine, positionForeground.yDottedLineTrg, astraConfig.tileAnimationSpeed);
    Animation::move(&positionForeground.yArrow, positionForeground.yArrowTrg, astraConfig.tileAnimationSpeed);
    Animation::move(&positionForeground.wBar, positionForeground.wBarTrg, astraConfig.tileAnimationSpeed);
    Animation::move(&positionForeground.yBar, positionForeground.yBarTrg, astraConfig.tileAnimationSpeed);
  }
}
