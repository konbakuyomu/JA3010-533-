#pragma once
#ifndef ASTRA_PAGE__H
#define ASTRA_PAGE__H

#include "../menu/menu.h"

namespace yomu
{
    class NumberEditor : public astra::Page
    {
    public:
        explicit NumberEditor(std::string initialNumber);
        NumberEditor(const std::string &_title, const std::vector<unsigned char> &_pic, std::string initialNumber);

    public:
        void init() override;
        void render(const std::vector<float> &_camera) override;

    public:
        void onLeft() override; 
        void onRight() override;
        void onUp() override;
        bool onConfirm(int _index) override;
        float convertToFloat();

    public:
        std::string cancel_button = "取消";
        std::string confirm_button = "确认";
        std::string Number = "1000";
        int currentIndex = 0;    // 当前的选中框索引
        int countNumber = 2;     // 当设置界面需要设置的字符数量（默认是加上了取消和确认键）
        int dotIndex = -1;       // 当前小数点的索引，默认为-1，即没有小数点
        float firstDigitX = -1;  // 用于存储第一个数字的X坐标
        std::vector<int> digits; // 用于存储每一位数字

    public:
        float xTitle = 0;    // 标题x坐标
        float yTitle = 0;    // 标题y坐标
        float xTitleTrg = 0; // 标题目标x坐标
        float yTitleTrg = 0; // 标题目标y坐标

        float xExit = 0;    // 退出按钮x坐标
        float yExit = 0;    // 退出按钮y坐标
        float xExitTrg = 0; // 退出按钮目标x坐标
        float yExitTrg = 0; // 退出按钮目标y坐标

        float xConfirm = 0;    // 确认按钮x坐标
        float yConfirm = 0;    // 确认按钮y坐标
        float xConfirmTrg = 0; // 确认按钮目标x坐标
        float yConfirmTrg = 0; // 确认按钮目标y坐标

        float yNumber = 0;    // 数值y坐标
        float yNumberTrg = 0; // 数值目标y坐标

        float xSelection = 0;    // 选择框x坐标
        float ySelection = 0;    // 选择框y坐标
        float xSelectionTrg = 0; // 选择框目标x坐标
        float ySelectionTrg = 0; // 选择框目标y坐标
        float wSelection = 0;    // 选择框宽度
        float hSelection = 0;    // 选择框高度
        float wSelectionTrg = 0; // 选择框目标宽度
        float hSelectionTrg = 0; // 选择框目标高度

    private:
        void updateSelectionTarget(int index);
        void updateNumber(int index);
        void updateNumberString(void);
    };

    class GammaDashboard : public astra::Page
    {
    public:
        GammaDashboard() = default;
        GammaDashboard(const std::string &_title, const std::vector<unsigned char> &_pic, const std::vector<std::string> &labels);

    public:
        void init(astra::Menu::ExitDirection _direction) override;
        ExitDirection render(bool is_Clear_Canvas) override;

    public:
        void onRight() override;
        void onLeft() override;
        ExitDirection shouldExit() const override;

    public:
        bool updateRowData(const std::string &label, const std::string &newValue, const std::string &newUnit = "");

    public:
        struct RowCoordinates
        {
            float xRow;
            float xRowTrg;
            std::string value;
            std::string unit;
        };

    public:
        float lineHeight = 0;                         // 每一行的行高
        float labelWidth = 0;                         // 保存最长的标签宽度
        float valueWidth = 0;                         // 数值显示的宽度
        float unitWidth = 0;                          // 单位显示的宽度
        float totalWidth = 0;                         // 总宽度
        std::vector<RowCoordinates> m_rowCoordinates; // 每一行的 实时x坐标 和 预期x坐标

    private:
        std::vector<std::string> m_labels;
        size_t m_displayLines;
    };
}

#endif // ASTRA_PAGE__H
