#include "page.h"

namespace yomu
{
    /**
     * @brief NumberEditor类的构造函数
     *
     * @param initialNumber 初始数字字符串
     *
     * @details 该构造函数用于创建一个NumberEditor对象，并进行以下初始化操作：
     *          1. 设置默认标题为"数值设置"
     *          2. 生成默认图片
     *          3. 设置初始数字
     *          4. 初始化选择索引为0
     *          5. 清空子菜单和子部件列表
     *
     * @note 这个构造函数主要用于在磁贴菜单中使用，因此添加了默认标题和图片
     */
    NumberEditor::NumberEditor(std::string initialNumber)
    {
        this->title = "数值设置";
        this->pic = generateDefaultPic();
        this->Number = initialNumber;

        this->selectIndex = 0;
        this->childMenu.clear();
        this->childWidget.clear();
    }

    /**
     * @brief NumberEditor类的构造函数
     *
     * @param _title 设置界面的标题
     * @param initialNumber 初始数字字符串
     *
     * @details 该构造函数用于创建一个NumberEditor对象，并进行以下初始化操作：
     *          1. 设置界面标题
     *          2. 设置界面图标
     *          3. 设置初始数字
     *          4. 初始化选择索引为0
     *          5. 清空子菜单和子部件列表
     */
    NumberEditor::NumberEditor(const std::string &_title, std::string initialNumber)
    {
        this->title = _title;
        this->pic = generateDefaultPic();
        this->Number = initialNumber;

        // 初始化选择索引为0
        this->selectIndex = 0;
        // 清空子菜单列表
        this->childMenu.clear();
        // 清空子部件列表（为未来可能的扩展做准备）
        this->childWidget.clear();
    }

    /**
     * @brief NumberEditor类的构造函数
     *
     * @param _title 设置界面的标题
     * @param _pic 设置界面的图标
     * @param initialNumber 初始数字字符串
     *
     * @details 该构造函数用于创建一个NumberEditor对象，并进行以下初始化操作：
     *          1. 设置界面标题
     *          2. 设置界面图标
     *          3. 设置初始数字
     *          4. 初始化选择索引为0
     *          5. 清空子菜单和子部件列表
     *
     * @note 这个构造函数允许自定义标题和图标，更加灵活地创建NumberEditor对象
     */
    NumberEditor::NumberEditor(const std::string &_title, const std::vector<unsigned char> &_pic, std::string initialNumber)
    {
        this->title = _title;
        this->pic = _pic;
        this->Number = initialNumber;

        // 初始化选择索引为0
        this->selectIndex = 0;
        // 清空子菜单列表
        this->childMenu.clear();
        // 清空子部件列表（为未来可能的扩展做准备）
        this->childWidget.clear();
    }

    /**
     * @brief NumberEditor类的构造函数
     *
     * @param _title 设置界面的标题
     * @param _pic 设置界面的图标
     * @param initialNumber 初始数字字符串
     * @param unit 设置界面的单位
     *
     * @details 该构造函数用于创建一个NumberEditor对象，并进行以下初始化操作：
     *          1. 设置界面标题
     *          2. 设置界面图标
     *          3. 设置初始数字
     *          4. 设置界面单位
     *          5. 初始化选择索引为0
     *          6. 清空子菜单和子部件列表
     */
    NumberEditor::NumberEditor(const std::string &_title, std::string initialNumber, const std::string &unit)
    {
        this->title = _title;
        this->pic = picDefault;
        this->Number = initialNumber;
        this->unit = unit;

        // 初始化选择索引为0
        this->selectIndex = 0;
        // 清空子菜单列表
        this->childMenu.clear();
        // 清空子部件列表（为未来可能的扩展做准备）
        this->childWidget.clear();
    }

    /**
     * @brief 初始化NumberEditor对象
     *
     * @details 该函数负责初始化NumberEditor对象的各项属性，包括：
     *          1. 设置标题的位置和动画
     *          2. 设置退出和确认按钮的位置
     *          3. 格式化输入的数字字符串
     *          4. 计算并设置数字显示的位置
     *          5. 初始化选择框的位置和大小
     *
     * @note 该函数仅在第一次调用时执行初始化操作，之后的调用将不会重复执行
     *
     * @see NumberEditor::render() 渲染函数，使用这里初始化的属性进行绘制
     */
    void NumberEditor::init()
    {
        // 设置标题字体
        HAL::setFont(astra::getUIConfig().logoCopyRightFont);

        // 初始化通用属性
        xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(title)) / 2;
        xTitleTrg = xTitle;
        yTitle = 0 - astra::getUIConfig().logoTextHeight - 1;
        yTitleTrg = HAL::getSystemConfig().screenHeight / 2 - astra::getUIConfig().logoTextHeight;

        xExit = astra::getUIConfig().selectorMargin;
        xExitTrg = xExit;
        yExit = systemConfig.screenHeight;
        yExitTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin / 2;

        float numberWidth = HAL::getFontWidth(confirm_button);
        xConfirm = HAL::getSystemConfig().screenWeight - xExit - numberWidth;
        xConfirmTrg = xConfirm;
        yConfirm = yExit;
        yConfirmTrg = yExitTrg;

        yNumber = yTitle;
        yNumberTrg = yTitleTrg + 20;

        // 进行第一次初始化
        if (!this->is_init)
        {
            this->is_init = true;

            // 格式化数字
            std::string temp = Number;
            const int charSpacing = astra::getUIConfig().numberEditorCharMargin;
            float totalWidth = 0;
            digits.clear();
            dotIndex = -1;

            for (char c : temp)
            {
                std::string charStr(1, c);
                totalWidth += HAL::getFontWidth(charStr);

                if (c >= '0' && c <= '9')
                {
                    countNumber++;
                    digits.push_back(c - '0');
                }
                else if (c == '.')
                    dotIndex = countNumber - 1;
            }

            totalWidth += (temp.length() - 1) * charSpacing;
            firstDigitX = (systemConfig.screenWeight - totalWidth) / 2.0f;
        }

        // 初始化选择框
        currentIndex = 0;
        updateSelectionTarget(currentIndex);
        ySelection = systemConfig.screenHeight + 1;
        xSelection = xSelectionTrg;
        wSelection = wSelectionTrg;
        hSelection = hSelectionTrg;

        // 字体还原
        HAL::setFont(astra::getUIConfig().mainFont);

        // 只有当单位不为空时才初始化单位的位置
        if (!unit.empty())
        {
            float unitWidth = HAL::getFontWidth(unit);
            xUnit = (HAL::getSystemConfig().screenWeight - unitWidth) / 2;
            xUnitTrg = xUnit;
            yUnit = systemConfig.screenHeight;
            yUnitTrg = yExitTrg; // 与"取消"和"确认"按钮在同一高度
        }
    }

    /**
     * @brief 渲染数字编辑器界面
     *
     * 该函数负责绘制数字编辑器的所有元素，包括标题、数字、退出按钮、确认按钮和选择框。
     * 它还处理这些元素的动画效果。
     *
     * @param _camera 相机参数，用于判断是否需要清除和更新画布
     */
    void NumberEditor::render(const std::vector<float> &_camera)
    {
        if (_camera[0] == 0 && _camera[1] == 0)
            // 清除画布，准备绘制新帧
            HAL::canvasClear();

        // 绘制元素
        HAL::setDrawType(1);

        // 绘制标题
        HAL::setFont(astra::getUIConfig().logoCopyRightFont);
        HAL::drawChinese(xTitle, yTitle, title);
        animatePosition(yTitle, yTitleTrg);

        // 绘制每个字符
        float currentX = firstDigitX;
        HAL::setFont(astra::getUIConfig().logoCopyRightFont);
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

        // 只有当单位不为空时才绘制单位
        if (!unit.empty())
        {
            // 计算单位文本的宽度和高度
            float unitWidth = HAL::getFontWidth(unit);
            float unitHeight = HAL::getFontHeight();

            // 计算边框的尺寸（比文本稍大一些）
            float frameWidth = unitWidth + 10;  // 左右各增加5像素
            float frameHeight = unitHeight + 6; // 上下各增加3像素

            // 计算边框的位置
            float frameX = xUnit - 5;
            float frameY = yUnit - unitHeight;

            // 绘制圆角矩形边框
            HAL::drawRFrame(frameX, frameY, frameWidth, frameHeight, 2); // 2是圆角半径

            // 绘制单位文本
            HAL::drawChinese(xUnit, yUnit, unit);
        }

        // 更新选择框位置
        animatePosition(xSelection, xSelectionTrg);
        animatePosition(ySelection, ySelectionTrg);
        animatePosition(wSelection, wSelectionTrg);
        animatePosition(hSelection, hSelectionTrg);
        animatePosition(yUnit, yUnitTrg);

        HAL::setDrawType(2); // 设置绘制类型为图形
        // 绘制选择框
        HAL::drawRBox(xSelection, ySelection, wSelection, hSelection, 0.5);
        HAL::setDrawType(1); // 重置绘制类型为默认

        if (_camera[0] == 0 && _camera[1] == 0)
            // 更新画布显示
            HAL::canvasUpdate();
    }

    /**
     * @brief 更新选择框的目标位置
     *
     * 根据当前选中的索引，计算并设置选择框的目标位置。
     * 选择框可以在取消按钮、确认按钮或数字位上移动。
     *
     * @param index 当前选中的索引
     */
    void NumberEditor::updateSelectionTarget(int index)
    {
        HAL::setFont(astra::getUIConfig().mainFont);
        std::string buttonText = "数据";
        std::string digitText = "0";
        float buttonWidth = HAL::getFontWidth(buttonText) + 9;
        float digitWidth = HAL::getFontWidth(digitText) + 5;
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
                xSelectionTrg = (firstDigitX + 4) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 3) + astra::getUIConfig().numberEditorCharMoveLengthwithDot;
            }
            else
            {
                // 没有小数点，直接乘以字符长度和设定的间距
                xSelectionTrg = (firstDigitX + 4) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 2);
            }
            ySelectionTrg = yNumberTrg - HAL::getFontHeight();
            wSelectionTrg = digitWidth;
            hSelectionTrg = buttonHeight;
        }
    }

    /**
     * @brief 更新数字
     *
     * 根据给定的索引，增加对应位置的数字值。
     * 如果数字达到9，则循环回0。
     *
     * @param index 要更新的数字位置
     */
    void NumberEditor::updateNumber(int index)
    {
        if (index > 0 && index < countNumber - 1)
        {
            int digitIndex = index - 1;
            digits[digitIndex] = (digits[digitIndex] + 1) % 10;
            updateNumberString();
        }
    }

    /**
     * @brief 更新数字字符串
     *
     * 根据当前的数字数组，重新生成数字字符串。
     * 如果有小数点，会在适当的位置插入小数点。
     */
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

    /**
     * @brief 将数字字符串转换为浮点数
     *
     * @return float 转换后的浮点数值
     */
    float NumberEditor::convertToFloat(void)
    {
        float result = std::stof(Number);
        return result;
    }

    /**
     * @brief 处理向左移动的操作
     *
     * 将当前选中的索引向左移动一位。
     * 如果当前在最左边（取消按钮），则循环到最右边（确认按钮）。
     */
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

    /**
     * @brief 处理向右移动的操作
     *
     * 将当前选中的索引向右移动一位。
     * 如果到达最右边，则循环回到最左边。
     */
    void NumberEditor::onRight()
    {
        // 第一个是取消，最后一个是确认
        currentIndex = (currentIndex + 1) % countNumber;
        updateSelectionTarget(currentIndex);
    }

    /**
     * @brief 处理向上移动的操作
     *
     * 增加当前选中位置的数字值。
     */
    void NumberEditor::onUp()
    {
        updateNumber(currentIndex);
    }

    /**
     * @brief 处理确认操作
     *
     * 检查是否点击了取消或确认按钮。
     *
     * @param _index 按钮索引（0为取消，1为确认）
     * @return bool 如果点击了相应的按钮则返回true，否则返回false
     */
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

    /**
     * @brief GammaDashboard类的构造函数
     *
     * 初始化Gamma仪表盘，设置标签和显示行数。
     *
     * @param labels 要显示的标签列表
     */
    GammaDashboard::GammaDashboard(const std::vector<std::string> &labels)
    {
        this->title = "";
        this->pic = generateDefaultPic();
        this->m_labels = labels;
        // 返回参数中较小的那个，可以这里把最大数量限制到了4个
        this->m_displayLines = std::min(labels.size(), size_t(4));
    }

    /**
     * @brief 初始化Gamma仪表盘
     *
     * 设置仪表盘的布局和初始位置，准备动画效果。
     *
     * @param _direction 进入方向
     */
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

        if (!this->is_init)
        {
            this->is_init = true;

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
        else
        {
            // 确保 m_rowCoordinates 的大小与 m_displayLines 匹配
            m_rowCoordinates.resize(m_displayLines);

            if (_direction == ExitDirection::Right)
            {
                // 从右边进入
                for (auto &row : m_rowCoordinates)
                {
                    row.xRow = systemConfig.screenWeight + 1;
                    row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
                }
            }
            else
            {
                // 从左边进入
                for (auto &row : m_rowCoordinates)
                {
                    row.xRow = 0 - totalWidth;
                    row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
                }
            }
        }
    }

    /**
     * @brief 渲染Gamma仪表盘
     *
     * 绘制仪表盘的所有元素，包括标签、值和单位。
     * 处理元素的动画效果，并检查是否应该退出。
     *
     * @param is_Clear_Canvas 是否需要清除画布
     * @return ExitDirection 退出方向（如果应该退出）
     */
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

    /**
     * @brief 更新行数据
     *
     * 根据给定的标签，更新对应行的值和单位。
     *
     * @param label 要更新的行的标签
     * @param newValue 新的值
     * @param newUnit 新的单位（可选）
     * @return bool 更新是否成功
     */
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

    /**
     * @brief 处理向右移动的操作
     *
     * 将所有行的目标 x 坐标设置为屏幕左边外侧，
     * 以实现向右滑出屏幕的效果。
     */
    void GammaDashboard::onRight()
    {
        // 目标 x坐标 全部移动到屏幕的左边外面
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = 0 - totalWidth;
        }
    }

    /**
     * @brief 处理向左移动的操作
     *
     * 将所有行的目标 x 坐标设置为屏幕右边外侧，
     * 以实现向左滑出屏幕的效果。
     */
    void GammaDashboard::onLeft()
    {
        // 目标 x坐标 全部移动到屏幕的右边外面
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = systemConfig.screenWeight + 1;
        }
    }

    /**
     * @brief 检查是否应该退出当前页面
     *
     * 检查所有行是否都达到了目标位置，如果是，
     * 则根据目标位置判断退出方向。
     *
     * @return ExitDirection 退出方向，可能是Right、Left或None
     */
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

    //--------------------------------------------------------------------------

    /**
     * @brief PDXDashboard类的构造函数
     *
     * 初始化PDXDashboard对象，设置标签和显示行数。
     *
     * @param labels 标签字符串的向量
     */
    PDXDashboard::PDXDashboard(const std::vector<std::string> &labels)
    {
        this->title = "";
        this->pic = generateDefaultPic();
        this->m_labels = labels;
        // 返回参数中较小的那个，可以这里把最大数量限制到了4个
        this->m_displayLines = std::min(labels.size(), size_t(4));
    }

    /**
     * @brief 初始化PDXDashboard
     *
     * 根据给定的退出方向初始化仪表盘，设置行坐标和警告状态。
     *
     * @param _direction 退出方向
     */
    void PDXDashboard::init(astra::Menu::ExitDirection _direction)
    {
        if (m_labels.empty() || m_displayLines == 0)
            return;

        lineHeight = systemConfig.screenHeight / float(m_displayLines);
        std::string temp = "P报警";
        charWidth = HAL::getFontWidth(temp);

        // 计算最长标签的宽度
        for (auto &label : m_labels)
        {
            labelWidth = std::max(labelWidth, static_cast<float>(HAL::getFontWidth(label)));
        }

        // 计算总宽度和起始 X 坐标
        totalWidth = labelWidth + (charWidth * 2) + 30; // 30 为间隔

        // 先清空这个容器
        m_rowCoordinates.clear();

        // float xPDX = systemConfig.screenWeight - (charWidth * 3) - 30; // 30 为右边距

        if (_direction == ExitDirection::Right)
        {
            // 从右边进入
            for (size_t i = 0; i < m_displayLines; ++i)
            {
                RowCoordinates row;
                row.xRow = systemConfig.screenWeight + 1;
                row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
                row.p = false;
                row.d = false;
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
                row.p = false;
                row.d = false;
                m_rowCoordinates.push_back(row);
            }
        }

        // 测试
        static uint8_t count = 0;
        if (count == 0)
        {
            updateWarningStatus("前方", true, false);
            updateWarningStatus("后方", true, true);
            updateWarningStatus("左方", false, true);
            updateWarningStatus("测试", true, false);
        }
        if (count == 1)
        {
            updateWarningStatus("前方", false, true);
            updateWarningStatus("后方", false, false);
            updateWarningStatus("左方", true, false);
            updateWarningStatus("测试", false, true);
        }
        count++;
    }

    /**
     * @brief 渲染PDXDashboard
     *
     * 绘制仪表盘的所有元素，包括标签和警告状态。
     * 处理元素的动画效果，并检查是否应该退出。
     *
     * @param is_Clear_Canvas 是否需要清除画布
     * @return ExitDirection 退出方向（如果应该退出）
     */
    astra::Menu::ExitDirection PDXDashboard::render(bool is_Clear_Canvas)
    {
        if (is_Clear_Canvas)
            HAL::canvasClear();

        HAL::setDrawType(1);

        for (auto &row : m_rowCoordinates)
        {
            float y = lineHeight * ((&row - &m_rowCoordinates[0]) + 0.7f);

            HAL::drawChinese(row.xRow, y, m_labels[&row - &m_rowCoordinates[0]]);

            float xPDX = row.xRow + labelWidth + 20;
            HAL::drawChinese(xPDX, y, "P报警");
            HAL::drawChinese(xPDX + charWidth + 10, y, "D报警");

            // 绘制选择框
            HAL::setDrawType(2);
            if (row.p)
            {
                HAL::drawRBox(xPDX - 2, y - 12, charWidth + 4, lineHeight, 0.5);
            }
            if (row.d)
            {
                HAL::drawRBox(xPDX + charWidth + 8, y - 12, charWidth + 4, lineHeight, 0.5);
            }
            HAL::setDrawType(1);

            animatePosition(row.xRow, row.xRowTrg);
        }

        if (is_Clear_Canvas)
            HAL::canvasUpdate();

        if (shouldExit() == ExitDirection::Right)
            return ExitDirection::Right;
        else if (shouldExit() == ExitDirection::Left)
            return ExitDirection::Left;

        return ExitDirection::None;
    }

    /**
     * @brief 更新警告状态
     *
     * 根据给定的标签，更新对应行的P和D警告状态。
     *
     * @param label 要更新的行的标签
     * @param p P警告的新状态
     * @param d D警告的新状态
     * @return bool 更新是否成功
     */
    bool PDXDashboard::updateWarningStatus(const std::string &label, bool p, bool d)
    {
        auto it = std::find(m_labels.begin(), m_labels.end(), label);
        if (it == m_labels.end())
            return false;

        size_t index = std::distance(m_labels.begin(), it);
        if (index >= m_rowCoordinates.size())
            return false;

        m_rowCoordinates[index].p = p;
        m_rowCoordinates[index].d = d;

        return true;
    }

    /**
     * @brief 处理向右移动的操作
     *
     * 将所有行的目标 x 坐标设置为屏幕左边外侧，
     * 以实现向右滑出屏幕的效果。
     */
    void PDXDashboard::onRight()
    {
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = 0 - totalWidth;
        }
    }

    /**
     * @brief 处理向左移动的操作
     *
     * 将所有行的目标 x 坐标设置为屏幕右边外侧，
     * 以实现向左滑出屏幕的效果。
     */
    void PDXDashboard::onLeft()
    {
        for (auto &row : m_rowCoordinates)
        {
            row.xRowTrg = systemConfig.screenWeight + 1;
        }
    }

    /**
     * @brief 检查是否应该退出当前页面
     *
     * 检查所有行是否都达到了目标位置，如果是，
     * 则根据目标位置判断退出方向。
     *
     * @return ExitDirection 退出方向，可能是Right、Left或None
     */
    astra::Menu::ExitDirection PDXDashboard::shouldExit() const
    {
        if (!std::all_of(m_rowCoordinates.begin(), m_rowCoordinates.end(),
                         [](const RowCoordinates &row)
                         {
                             return std::abs(row.xRow - row.xRowTrg) < 0.1f;
                         }))
        {
            return ExitDirection::None;
        }

        if (m_rowCoordinates[0].xRowTrg == 0 - totalWidth)
        {
            return ExitDirection::Right;
        }
        else if (m_rowCoordinates[0].xRowTrg == systemConfig.screenWeight + 1)
        {
            return ExitDirection::Left;
        }

        return ExitDirection::None;
    }
}
