#include "page.h"

namespace yomu
{
    NumberEditor::NumberEditor(std::string initialNumber)
    {
        // 这是为了可能在磁贴菜单的情况增加默认标题和图片
        this->title = "数值设置";
        this->pic = generateDefaultPic();
        this->Number = initialNumber;

        // 我自己的page类下面的界面有且只有一个子页面
        this->selectIndex = 0;
        this->childMenu.clear();
        this->childWidget.clear(); // 插件后面会考虑添加
    }

    NumberEditor::NumberEditor(const std::string &_title, const std::vector<unsigned char> &_pic, std::string initialNumber)
    {
        this->title = _title;
        this->pic = _pic;
        this->Number = initialNumber;

        // 我自己的page类下面的界面有且只有一个子页面
        this->selectIndex = 0;
        this->childMenu.clear();
        this->childWidget.clear(); // 插件后面会考虑添加
    }

    void NumberEditor::init()
    {
        // 进行第一次初始化
        if (this->is_init == false)
        {
            // 初始化标志置为true
            this->is_init = true;

            // 设置标题的x坐标
            HAL::setFont(astra::getUIConfig().mainFont);
            xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(title)) / 2;
            xTitleTrg = xTitle;
            // 设置标题的y坐标
            // yTitle 是设置为负值（0减去文本高度再减1）意味着初始时标题在屏幕上方不可见的位置
            // yTitleTrg 是目标位置,标题居中，标题的动画是从上往下移动到屏幕中间
            yTitle = 0 - astra::getUIConfig().logoTextHeight - 1;
            yTitleTrg = HAL::getSystemConfig().screenHeight / 2 - astra::getUIConfig().logoTextHeight;

            // 设置退出按钮的x坐标（起始位置在左下角）
            xExit = astra::getUIConfig().selectorMargin;
            xExitTrg = xExit;
            yExit = systemConfig.screenHeight;
            yExitTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin / 2;

            // 设置确认按钮的x坐标（起始位置在右下角）
            // 这里算坐标要先减去字符的长度再减去边距
            float numberWidth = HAL::getFontWidth(confirm_button);
            xConfirm = HAL::getSystemConfig().screenWeight - xExit - numberWidth;
            xConfirmTrg = xConfirm;
            yConfirm = yExit;
            yConfirmTrg = yExitTrg;

            // 格式化数字
            std::string temp = Number;                                           // 将构造函数中传入的数字字符串赋值给temp
            const int charSpacing = astra::getUIConfig().numberEditorCharMargin; // 字符之间的间隔像素数
            float totalWidth = 0;                                                // 计算总宽度
            digits.clear();                                                      // 初始化digits vector
            for (char c : temp)
            {
                // 创建一个只包含当前字符的字符串
                std::string charStr(1, c);
                // 调用HAL::getFontWidth函数获取字符宽度，并累加到totalWidth
                totalWidth += HAL::getFontWidth(charStr);

                // 如果是数字, 增加字符数量，并给digits vector当前数字
                if (c >= '0' && c <= '9')
                {
                    countNumber++;
                    digits.push_back(c - '0');
                }
                // 如果是小数点，设置dotIndex为当前字符的索引
                else if (c == '.')
                    dotIndex = countNumber - 1;
            }
            // 在总宽度上添加字符间距
            // temp.length() - 1 是字符间隔的数量，乘以3是每个间隔的宽度(因为最后一个字符的后面不需要间隔)
            totalWidth += (temp.length() - 1) * charSpacing;
            // 计算起始X坐标，使字符串居中
            firstDigitX = (systemConfig.screenWeight - totalWidth) / 2.0f;
            yNumber = yTitle;
            yNumberTrg = yTitleTrg + 20;

            // 初始化选择框
            currentIndex = 0;
            updateSelectionTarget(currentIndex);
            ySelection = systemConfig.screenHeight + 1;
            xSelection = xSelectionTrg;
            wSelection = wSelectionTrg;
            hSelection = hSelectionTrg;
        }
        else
        {
            // 设置标题的x坐标
            HAL::setFont(astra::getUIConfig().mainFont);
            xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(title)) / 2;
            xTitleTrg = xTitle;
            // 设置标题的y坐标
            // yTitle 是设置为负值（0减去文本高度再减1）意味着初始时标题在屏幕上方不可见的位置
            // yTitleTrg 是目标位置,标题居中，标题的动画是从上往下移动到屏幕中间
            yTitle = 0 - astra::getUIConfig().logoTextHeight - 1;
            yTitleTrg = HAL::getSystemConfig().screenHeight / 2 - astra::getUIConfig().logoTextHeight;

            // 设置退出按钮的x坐标（起始位置在左下角）
            xExit = astra::getUIConfig().tilePicMargin - 4;
            xExitTrg = xExit;
            yExit = systemConfig.screenHeight;
            yExitTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin / 2;

            // 设置确认按钮的x坐标（起始位置在右下角）
            // 这里算坐标要先减去字符的长度再减去边距
            float numberWidth = HAL::getFontWidth(confirm_button);
            xConfirm = HAL::getSystemConfig().screenWeight - xExit - numberWidth;
            xConfirmTrg = xConfirm;
            yConfirm = yExit;
            yConfirmTrg = yExitTrg;

            // 设置字符串y坐标
            yNumber = yTitle;
            yNumberTrg = yTitleTrg + 20;

            // 初始化选择框
            currentIndex = 0;
            updateSelectionTarget(currentIndex);
            ySelection = systemConfig.screenHeight + 1;
            xSelection = xSelectionTrg;
            wSelection = wSelectionTrg;
            hSelection = hSelectionTrg;
        }
    }

    void NumberEditor::render(const std::vector<float> &_camera)
    {
        if (_camera[0] == 0 && _camera[1] == 0)
            // 清除画布，准备绘制新帧
            HAL::canvasClear();

        // 绘制元素
        HAL::setDrawType(1);

        // 绘制标题
        HAL::setFont(astra::getUIConfig().mainFont);
        HAL::drawChinese(xTitle, yTitle, title);
        animatePosition(yTitle, yTitleTrg);

        // 绘制每个字符
        float currentX = firstDigitX;
        // 遍历字符串中的每个字符
        for (char c : Number)
        {
            // 创建一个只包含当前字符的字符串
            std::string charStr(1, c);
            HAL::drawChinese(currentX, yNumber, charStr);
            currentX += HAL::getFontWidth(charStr) + astra::getUIConfig().numberEditorCharMargin; // 3是字符间距
        }
        animatePosition(yNumber, yNumberTrg);

        // 绘制退出按钮
        HAL::setFont(astra::getUIConfig().mainFont);
        HAL::drawChinese(xExit, yExit, cancel_button);
        animatePosition(yExit, yExitTrg);

        // 绘制确认按钮
        HAL::drawChinese(xConfirm, yConfirm, confirm_button);
        animatePosition(yConfirm, yConfirmTrg);

        // 更新选择框位置
        animatePosition(xSelection, xSelectionTrg);
        animatePosition(ySelection, ySelectionTrg);
        animatePosition(wSelection, wSelectionTrg);
        animatePosition(hSelection, hSelectionTrg);

        HAL::setDrawType(2); // 设置绘制类型为图形
        // 绘制选择框
        HAL::drawRBox(xSelection, ySelection, wSelection, hSelection, 0.5);
        HAL::setDrawType(1); // 重置绘制类型为默认

        if (_camera[0] == 0 && _camera[1] == 0)
            // 更新画布显示
            HAL::canvasUpdate();
    }

    void NumberEditor::updateSelectionTarget(int index)
    {
        HAL::setFont(astra::getUIConfig().mainFont);
        std::string buttonText = "数据";
        std::string digitText = "0";
        float buttonWidth = HAL::getFontWidth(buttonText) + 2;
        float digitWidth = HAL::getFontWidth(digitText) + 2;
        float buttonHeight = astraConfig.listLineHeight;
        float buttonY = systemConfig.screenHeight - astraConfig.tileTextBottomMargin - 6;

        if (index == 0) // 取消按钮
        {
            xSelectionTrg = 0;
            ySelectionTrg = buttonY;
            wSelectionTrg = buttonWidth;
            hSelectionTrg = buttonHeight;
        }
        else if (index == countNumber - 1) // 确认按钮
        {
            xSelectionTrg = HAL::getSystemConfig().screenWeight - buttonWidth;
            ySelectionTrg = buttonY;
            wSelectionTrg = buttonWidth;
            hSelectionTrg = buttonHeight;
        }
        else // 数字位
        {
            if (index >= dotIndex)
            {
                // 如果有小数点，小数点后的字符都要加上中间跳过的小数点的长度
                xSelectionTrg = (firstDigitX - 1) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 2) + astra::getUIConfig().numberEditorCharMoveLengthwithDot;
            }
            else
            {
                // 没有小数点，直接乘以字符长度和设定的间距
                xSelectionTrg = (firstDigitX - 1) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 1);
            }
            ySelectionTrg = yNumberTrg - HAL::getFontHeight() + 4;
            wSelectionTrg = digitWidth;
            hSelectionTrg = buttonHeight;
        }
    }

    void NumberEditor::updateNumber(int index)
    {
        if (index > 0 && index < countNumber - 1)
        {
            int digitIndex = index - 1;
            digits[digitIndex] = (digits[digitIndex] + 1) % 10;
            updateNumberString();
        }
    }

    void NumberEditor::updateNumberString(void)
    {
        std::stringstream ss;
        for (size_t i = 0; i < digits.size(); ++i)
        {
            if (i == static_cast<size_t>(dotIndex) - 1)
            {
                ss << '.';
            }
            ss << digits[i];
        }
        Number = ss.str();
    }

    float NumberEditor::convertToFloat(void)
    {
        float result = std::stof(Number);
        return result;
    }

    void NumberEditor::onLeft()
    {
        // 向左移动，需要处理从第一个元素到最后一个元素的循环
        if (currentIndex == 0)
        {
            // 如果当前在最左边（取消按钮），就跳到最右边（确认按钮）
            currentIndex = countNumber - 1;
        }
        else
        {
            // 否则向左移动一位
            currentIndex--;
        }
        updateSelectionTarget(currentIndex);
    }

    void NumberEditor::onRight()
    {
        // 第一个是取消，最后一个是确认
        currentIndex = (currentIndex + 1) % countNumber;
        updateSelectionTarget(currentIndex);
    }

    void NumberEditor::onUp()
    {
        updateNumber(currentIndex);
    }

    bool NumberEditor::onConfirm(int _index)
    {
        if (_index == 0)
        {
            if (currentIndex == 0)
            {
                return true;
            }
        }
        else if (_index == 1)
        {
            if (currentIndex == countNumber - 1)
            {
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    GammaDashboard::GammaDashboard(const std::string &_title, const std::vector<unsigned char> &_pic, const std::vector<std::string> &labels)
    {
        this->title = _title;
        this->pic = _pic;
        this->m_labels = labels;
        // 返回参数中较小的那个，可以这里把最大数量限制到了4个
        this->m_displayLines = std::min(labels.size(), size_t(4));
    }

    void GammaDashboard::init(astra::Menu::ExitDirection _direction)
    {
        if (m_labels.empty() || m_displayLines == 0)
            return;

        lineHeight = systemConfig.screenHeight / float(m_displayLines);
        std::string temp = "0x00";
        valueWidth = HAL::getFontWidth(temp);
        temp = "μGy/h";
        unitWidth = HAL::getFontWidth(temp);

        // 计算最长标签的宽度
        for (auto &label : m_labels)
        {
            labelWidth = std::max(labelWidth, static_cast<float>(HAL::getFontWidth(label)));
        }

        // 计算总宽度和起始 X 坐标
        totalWidth = labelWidth + valueWidth + unitWidth + 30; // 20 为间隔

        // 先清空这个容器
        m_rowCoordinates.clear();

        if (_direction == ExitDirection::Right)
        {
            // 从右边进入
            for (size_t i = 0; i < m_displayLines; ++i)
            {
                RowCoordinates row;
                row.xRow = systemConfig.screenWeight + 1;
                row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
                row.value = "10000";
                row.unit = "μGy/h";
                m_rowCoordinates.push_back(row);
            }
        }
        else
        {
            // 从左边进入
            for (size_t i = 0; i < m_displayLines; ++i)
            {
                RowCoordinates row;
                row.xRow = 0 - totalWidth;
                row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
                row.value = "10000";
                row.unit = "μGy/h";
                m_rowCoordinates.push_back(row);
            }
        }
    }

    astra::Menu::ExitDirection GammaDashboard::render(bool is_Clear_Canvas)
    {
        if (is_Clear_Canvas)
            // 清除画布，准备绘制新帧
            HAL::canvasClear();

        // 绘制元素
        HAL::setDrawType(1);

        // C++ 11 的写法
        // 自动遍历 m_rowCoordinates 中的每个元素
        // auto& 表示我们使用引用来避免复制，并允许修改元素
        for (auto &row : m_rowCoordinates)
        {
            // &row 是当前元素的地址，&m_rowCoordinates[0] 是容器第一个元素的地址
            // &row - &m_rowCoordinates[0] 计算当前元素在容器中的索引
            float y = lineHeight * ((&row - &m_rowCoordinates[0]) + 0.7f); // 计算 y 坐标

            // 绘制元素
            HAL::drawChinese(row.xRow, y, m_labels[&row - &m_rowCoordinates[0]]);
            HAL::drawEnglish(row.xRow + labelWidth + 20, y, row.value);
            HAL::drawChinese(row.xRow + labelWidth + valueWidth + 30, y, row.unit);

            // 更新位置
            animatePosition(row.xRow, row.xRowTrg);
        }

        if (is_Clear_Canvas)
            // 更新画布显示
            HAL::canvasUpdate();

        // 查看退出方向
        if (shouldExit() == ExitDirection::Right)
        {
            return ExitDirection::Right;
        }
        else if (shouldExit() == ExitDirection::Left)
        {
            return ExitDirection::Left;
        }

        return ExitDirection::None;
    }

    bool GammaDashboard::updateRowData(const std::string &label, const std::string &newValue, const std::string &newUnit)
    {
        // 查找标签对应的索引
        // auto it = std::find(开始迭代器, 结束迭代器, 要查找的值);
        // 如果找到了值,它返回指向该值的迭代器。
        // 如果没找到,它返回结束迭代器。
        auto it = std::find(m_labels.begin(), m_labels.end(), label);
        if (it == m_labels.end())
        {
            // 如果没找到标签，返回 false
            return false;
        }

        // 计算索引
        // size_t index = std::distance(起始迭代器, 目标迭代器);
        // std::distance 用于计算两个迭代器之间的距离(元素数量)
        size_t index = std::distance(m_labels.begin(), it);

        // 确保索引在有效范围内
        if (index >= m_rowCoordinates.size())
        {
            return false;
        }

        // 更新值
        m_rowCoordinates[index].value = newValue;

        // 如果提供了新的单位，则更新单位
        if (!newUnit.empty())
        {
            m_rowCoordinates[index].unit = newUnit;
        }

        return true;
    }

    void GammaDashboard::onRight()
    {
        // 目标 x坐标 全部移动到屏幕的左边外面
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = 0 - totalWidth;
        }
    }

    void GammaDashboard::onLeft()
    {
        // 目标 x坐标 全部移动到屏幕的右边外面
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = systemConfig.screenWeight + 1;
        }
    }

    astra::Menu::ExitDirection GammaDashboard::shouldExit() const
    {
        // std::all_of 函数:它检查容器中的所有元素是否都满足特定条件。
        // 基本语法: std::all_of(开始迭代器, 结束迭代器, 判断条件)
        // [] 是捕获列表,这里为空,表示不捕获外部变量
        // const RowCoordinates &row 这个变量是 std::all_of 函数提供的。std::all_of 会遍历容器中的每个元素，并将每个元素作为参数传递给 Lambda 表达式
        // (const RowCoordinates& row) 是参数列表,接收每个元素
        // 函数体比较当前x坐标(xRow)和目标x坐标(xRowTrg)的差的绝对值
        if (!std::all_of(m_rowCoordinates.begin(), m_rowCoordinates.end(),
                         [](const RowCoordinates &row)
                         {
                             return std::abs(row.xRow - row.xRowTrg) < 0.1f;
                         }))
        {
            return ExitDirection::None; // 如果不是所有行都到达目标，返回 None
        }

        // 所有行都到达目标，现在检查是哪个方向
        if (m_rowCoordinates[0].xRowTrg == 0 - totalWidth)
        {
            return ExitDirection::Right; // 目标是屏幕左边
        }
        else if (m_rowCoordinates[0].xRowTrg == systemConfig.screenWeight + 1)
        {
            return ExitDirection::Left; // 目标是屏幕右边
        }

        return ExitDirection::None; // 以防万一，虽然不应该发生
    }
}
