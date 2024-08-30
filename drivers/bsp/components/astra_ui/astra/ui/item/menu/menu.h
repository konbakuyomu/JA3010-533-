//
// Created by Fir on 2024/1/21.
//

#pragma once
#ifndef ASTRA_ASTRA__H
#define ASTRA_ASTRA__H

#include "cstdint"
#include "string"
#include <vector>
#include <array>
#include <string>
#include <functional>
#include <charconv>
#include <algorithm>
#include <system_error>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "../item.h"
#include "../widget/widget.h"

namespace astra
{

  class Menu : public Item
  {
  public:
    [[nodiscard]] virtual std::string getType() const { return "Base"; }

  public:
    std::vector<float> cameraPosMemory = {};
    void rememberCameraPos(const std::vector<float> &_camera)
    {

      cameraPosMemory = _camera;
      cameraPosMemoryFlag = true;
    }
    [[nodiscard]] std::vector<float> getCameraMemoryPos() const { return cameraPosMemory; }
    void resetCameraMemoryPos() { cameraPosMemory = {0, 0}; }
    // 编写一个变量 指示该页面到底有没有记忆
    bool cameraPosMemoryFlag = false;

  public:
    enum class ExitDirection
    {
      None,
      Left,
      Right
    };

  public:
    // 存储其在父页面中的位置
    // list中就是每一项对应的坐标 tile中就是每一个图片的坐标
    typedef struct Position
    {
      float x, xTrg;
      float y, yTrg;
    } Position;

    Position position{};

    [[nodiscard]] Position getItemPosition(unsigned char _index) const;
    virtual void childPosInit(const std::vector<float> &_camera) {}
    virtual void forePosInit() {}
    virtual int getSelectedIndex() const { return selectIndex; }
    virtual Menu *findTileAncestor()
    {
      if (this->getType() == "Tile")
      {
        return this;
      }
      if (this->parent == nullptr)
      {
        return nullptr;
      }
      return this->parent->findTileAncestor();
    }
    virtual Menu *findListAncestor()
    {
      if (this->getType() == "List")
      {
        return this;
      }
      if (this->parent == nullptr)
      {
        return nullptr;
      }
      return this->parent->findListAncestor();
    }

  public:
    std::string title;
    std::vector<unsigned char> pic;

  protected:
    std::vector<unsigned char> picDefault = {};
    [[nodiscard]] std::vector<unsigned char> generateDefaultPic();

  public:
    // 环形界面的前一个界面和后一个界面
    Menu *preview{};
    Menu *next{};
    [[nodiscard]] Menu *getNextMenu() const; // 启动器调用该方法来获取下一个页面(环形界面)
    [[nodiscard]] Menu *getPreview() const;  // 启动器调用该方法来获取上一个页面(环形界面)

    // 树形界面的父子关系和插件和选择菜单项
    Menu *parent{};
    std::vector<Menu *> childMenu; // allow widget and menu.
    std::vector<Widget *> childWidget;
    unsigned char selectIndex{};

    [[nodiscard]] unsigned char getItemNum() const;
    [[nodiscard]] Menu *getchildMenu() const;  // 启动器调用该方法来获取下一个页面(树形界面)
    [[nodiscard]] Menu *getParentMenu() const; // 启动器调用该方法来获取上一个页面(树形界面)

  public:
    bool initFlag = false;

  public:
    Menu() = default;
    virtual ~Menu() = default;

  public:
    virtual void init(ExitDirection _direction) {}          // 每次打开页面都要调用一次(环形界面的初始化)
    virtual void init(const std::vector<float> &_camera) {} // 每次打开页面都要调用一次(树形界面的初始化)
    void deInit();                                          // 每次关闭页面都要调用一次

  public:
    virtual void render(const std::vector<float> &_camera) {}                          // 树形界面的渲染
    virtual ExitDirection render(bool is_Clear_Canvas) { return ExitDirection::None; } // 环形界面的渲染

  public:
    // 环形界面添加菜单，将 _page 设置后当前菜单的下一个菜单，将当前菜单设置为 _page 的上一个菜单
    bool addMenu(Menu *_page);

    // 树形界面添加子菜单
    bool addItem(Menu *_page);
    bool addItem(Menu *_page, Widget *_anyWidget); // 新建一个带有控件的列表项
  };

  class Divider : public Menu
  {
  public:
    [[nodiscard]] std::string getType() const override { return "Divider"; }

  public:
    Divider(const std::string &_title);
  };

  class List : public Menu
  {
  public:
    [[nodiscard]] std::string getType() const override { return "List"; }

  public:
    // 前景元素的坐标
    typedef struct PositionForeground
    {
      float hBar, hBarTrg; // 进度条高度
      float xBar, xBarTrg; // 进度条x坐标
    } PositionForeground;

    PositionForeground positionForeground{};

  public:
    enum class ItemAction
    {
      ShowPopup,
      EnterSubpage
    };
    ItemAction action = ItemAction::EnterSubpage;

  public:
    void childPosInit(const std::vector<float> &_camera) override;
    void forePosInit() override;

    List();
    // 如果不使用 explicit，编译器可能会在某些情况下进行隐式转换，这可能导致意外的行为
    // 例如，如果没有 explicit, [List myList = "Some Title"; ] 这种写法是合法的
    explicit List(const std::string &_title);
    explicit List(const std::vector<unsigned char> &_pic);
    // 双参数构造函数不需要 explicit，因为它有多个参数，不会发生隐式转换
    List(const std::string &_title, const std::vector<unsigned char> &_pic);
    List(const std::string &_title, const std::vector<unsigned char> &_pic, ItemAction _action);

  public:
    // 第一个值 0 表示当前可见列表的起始项索引
    // 第二个值 1 表示当前可见列表的结束项索引
    // static_cast<unsigned char>(systemConfig.screenHeight / astraConfig.listLineHeight - 1)
    // 计算屏幕可以显示的项目数量
    // 减1是因为索引从0开始
    std::vector<unsigned char> boundary = {0, static_cast<unsigned char>(systemConfig.screenHeight / astraConfig.listLineHeight - 1)};
    [[nodiscard]] std::vector<unsigned char> getBoundary() const { return boundary; }
    void refreshBoundary(unsigned char _l, unsigned char _r) { boundary = {_l, _r}; }

  public:
    void render(const std::vector<float> &_camera) override;
  };

  class Tile : public Menu
  {
  public:
    [[nodiscard]] std::string getType() const override { return "Tile"; }

  public:
    // 前景元素的坐标
    typedef struct PositionForeground
    {
      float wBar, wBarTrg;               // 进度条宽度
      float yBar, yBarTrg;               // 进度条y坐标
      float yArrow, yArrowTrg;           // 箭头y坐标
      float yDottedLine, yDottedLineTrg; // 虚线y坐标
    } PositionForeground;

    PositionForeground positionForeground{};

  public:
    void childPosInit(const std::vector<float> &_camera) override;
    void forePosInit() override;

    Tile();
    explicit Tile(const std::string &_title);
    explicit Tile(const std::vector<unsigned char> &_pic);
    Tile(const std::string &_title, const std::vector<unsigned char> &_pic);

  public:
    void render(const std::vector<float> &_camera) override;
  };

  class Page : public Menu
  {
  public:
    [[nodiscard]] std::string getType() const override { return "Page"; }

  public:
    Page() = default;

  public:
    virtual void init() {};
    virtual void init(int32_t u32ID) {};
    virtual void init(ExitDirection _direction) override {};
    virtual void deInit() {};

  public:
    // 在 namespace yomu 中添加以下结构体定义
    struct ConfirmResult
    {
      bool isConfirmed;
      float value;
      std::string probeId_text;

      // 结构体的构造函数
      ConfirmResult(bool confirmed = false, float val = 0.0f, std::string id_text = "")
          : isConfirmed(confirmed), value(val), probeId_text(id_text) {}
    };

  public: // 处理用户输入
    virtual void onLeft() {};
    virtual void onRight() {};
    virtual void onUp() {};
    virtual void onDown() {};
    virtual std::pair<bool, bool> onConfirm() { return {false, false}; }
    virtual ConfirmResult onConfirm(int _index) { return {false, 0.0f, ""}; }
    virtual ExitDirection shouldExit() const { return ExitDirection::None; }

  public:
    void animatePosition(float &pos, float target)
    {
      if (pos != target)
      {
        if (std::fabs(pos - target) < 0.15f)
          pos = target;
        else
          pos += (target - pos) / ((100 - astra::getUIConfig().numberEditorCharAnimationSpeed) / 1.0f);
      }
    }

  public:
    static std::string formatFloatToString(const float &value)
    {
      float scaledValue = value;

      if (value >= 1000000)
      {
        scaledValue = value / 1000000;
      }
      else if (value >= 1000)
      {
        scaledValue = value / 1000;
      }
      else
      {
        scaledValue = value;
      }

      char buffer[20];
      formatFloat(scaledValue, buffer, sizeof(buffer));
      return std::string(buffer);
    }

  public:
    bool is_confirm = false;
    bool is_exit = false;
    bool is_init = false;

  public:
    [[nodiscard]] bool isConfirm() const { return is_confirm; }
    [[nodiscard]] bool isExit() const { return is_exit; }
  };
}

#endif // ASTRA_ASTRA__H