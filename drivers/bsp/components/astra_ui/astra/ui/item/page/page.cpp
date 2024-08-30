#include "ProbeDataManager.h"
#include "page.h"

namespace yomu
{
    /**
     * @brief NumberEditor类的构造函数
     *
     * @param _title 设置界面的标题
     * @param unit 设置界面的单位
     *
     * @details 该构造函数用于创建一个NumberEditor对象，并进行以下初始化操作：
     *          1. 设置界面标题
     *          2. 设置默认图标
     *          3. 设置界面单位
     *          4. 初始化选择索引为0
     *          5. 清空子菜单和子部件列表
     */
    NumberEditor::NumberEditor(const std::string &_title, DoseType doseType)
        : m_doseType(doseType)
    {
        this->title = _title;
        this->pic = picDefault;

        // 初始化选择索引为0
        this->selectIndex = 0;
        // 清空子菜单列表
        this->childMenu.clear();
        // 清空子部件列表（为未来可能的扩展做准备）
        this->childWidget.clear();
    }

    /**
     * @brief 初始化NumberEditor界面
     *
     * @details 该函数负责初始化NumberEditor界面的各个元素，包括：
     * 1. 设置标题字体和位置
     * 2. 初始化退出按钮和确认按钮的位置
     * 3. 初始化数字显示位置
     * 4. 根据单位类型更新阈值
     * 5. 格式化数字显示
     * 6. 初始化选择框
     * 7. 初始化单位显示位置（如果有单位）
     *
     * 函数还处理了数字格式化，确保整数部分至少有3位，小数部分保留1位。
     * 同时，它还计算了数字显示的总宽度，以便居中显示。
     */
    void NumberEditor::init()
    {
        // 设置标题字体
        HAL::setFont(astra::getUIConfig().logoCopyRightFont);

        // 初始化通用属性
        // 计算标题的x坐标，使其居中显示
        xTitle = (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(title)) / 2;
        xTitleTrg = xTitle;
        // 初始y坐标设置在屏幕上方（不可见），目标y坐标设置在屏幕中上方
        yTitle = 0 - astra::getUIConfig().logoTextHeight - 1;
        yTitleTrg = HAL::getSystemConfig().screenHeight / 2 - astra::getUIConfig().logoTextHeight;

        // 初始化退出按钮位置
        xExit = astra::getUIConfig().selectorMargin;
        xExitTrg = xExit;
        // 初始y坐标设置在屏幕下方（不可见），目标y坐标设置在屏幕底部上方
        yExit = systemConfig.screenHeight;
        yExitTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin / 2;

        // 初始化确认按钮位置
        float numberWidth = HAL::getFontWidth(confirm_button);
        xConfirm = HAL::getSystemConfig().screenWeight - xExit - numberWidth;
        xConfirmTrg = xConfirm;
        yConfirm = yExit;
        yConfirmTrg = yExitTrg;

        // 初始化数字显示位置
        yNumber = yTitle;
        yNumberTrg = yTitleTrg + 20;

        // 辅助函数：调整阈值和单位
        auto adjustThresholdAndUnit = [this](const float &threshold)
        {
            std::string baseUnit = (this->m_doseType == DoseType::Cumulative) ? "Gy" : "Gy/h";
            if (threshold >= 1000000)
            {
                this->unit = baseUnit;
            }
            else if (threshold >= 1000)
            {
                this->unit = "m" + baseUnit;
            }
            else
            {
                this->unit = "μ" + baseUnit;
            }
        };

        // 初始化单位状态
        if (unit.find("μ") != std::string::npos)
        {
            currentUnitState = UnitState::Micro;
        }
        else if (unit.find("m") != std::string::npos)
        {
            currentUnitState = UnitState::Milli;
        }
        else
        {
            currentUnitState = UnitState::Base;
        }

        // 计算最大的单位框尺寸
        std::vector<std::string> allUnits;
        if (this->m_doseType == DoseType::Cumulative)
        {
            allUnits = {"μGy", "mGy", "Gy"};
        }
        else if (this->m_doseType == DoseType::RealTime)
        {
            allUnits = {"μGy/h", "mGy/h", "Gy/h"};
        }
        maxUnitFrameWidth = 0;
        maxUnitFrameHeight = 0;

        for (const auto &unitStr : allUnits)
        {
            std::string tempUnit = unitStr; // 创建一个非 const 的临时字符串
            float unitWidth = HAL::getFontWidth(tempUnit);
            float unitHeight = HAL::getFontHeight();
            float frameWidth = unitWidth + 6;   // 左右各增加3像素
            float frameHeight = unitHeight + 8; // 上下各增加4像素

            maxUnitFrameWidth = std::max(maxUnitFrameWidth, frameWidth);
            maxUnitFrameHeight = std::max(maxUnitFrameHeight, frameHeight);
        }

        // 每次init都需要执行的操作
        // 检查单位并更新阈值
        if (this->m_doseType == DoseType::Cumulative)
        {
            float threshold = 0.0f;
            // 根据标题中的探头编号提取对应的阈值
            if (this->title.find("探头1") != std::string::npos)
            {
                threshold = data.probe1_cumulative_alarm_threshold;
            }
            else if (this->title.find("探头2") != std::string::npos)
            {
                threshold = data.probe2_cumulative_alarm_threshold;
            }
            else if (this->title.find("探头3") != std::string::npos)
            {
                threshold = data.probe3_cumulative_alarm_threshold;
            }
            else if (this->title.find("探头4") != std::string::npos)
            {
                threshold = data.probe4_cumulative_alarm_threshold;
            }

            // 调整阈值和单位
            adjustThresholdAndUnit(threshold);
            // 将阈值转换为字符串并格式化
            Number = astra::Page::formatFloatToString(threshold);
        }
        else if (this->m_doseType == DoseType::RealTime)
        {
            float threshold = 0.0f;
            if (this->title.find("探头1") != std::string::npos)
            {
                threshold = data.probe1_realtime_alarm_threshold;
            }
            else if (this->title.find("探头2") != std::string::npos)
            {
                threshold = data.probe2_realtime_alarm_threshold;
            }
            else if (this->title.find("探头3") != std::string::npos)
            {
                threshold = data.probe3_realtime_alarm_threshold;
            }
            else if (this->title.find("探头4") != std::string::npos)
            {
                threshold = data.probe4_realtime_alarm_threshold;
            }

            // 调整阈值和单位
            adjustThresholdAndUnit(threshold);
            // 将阈值转换为字符串并格式化
            Number = astra::Page::formatFloatToString(threshold);
        }

        // 重置计数器和数组
        countNumber = 0;
        digits.clear();
        dotIndex = -1;

        // 格式化数字
        std::string temp = Number;
        const int charSpacing = astra::getUIConfig().numberEditorCharMargin;
        float totalWidth = 0;

        // 找到小数点的位置
        size_t dotPos = temp.find('.');
        if (dotPos == std::string::npos)
        {
            // 如果没有小数点，添加一个
            temp += ".0";
            dotPos = temp.length() - 2;
        }

        // 确保整数部分至少有3位
        std::string intPart = temp.substr(0, dotPos);
        while (intPart.length() < 3)
        {
            intPart = "0" + intPart;
        }

        // 只保留小数点后1位
        std::string decPart = temp.substr(dotPos + 1);
        if (decPart.length() > 1)
        {
            decPart = decPart.substr(0, 1);
        }
        else if (decPart.empty())
        {
            decPart = "0";
        }

        // 组合新的数字字符串
        temp = intPart + "." + decPart;

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
                dotIndex = countNumber + 1;
        }

        // 增加 countNumber （因为默认有取消键和确认键还有单位）
        countNumber += 3;

        totalWidth += (temp.length() - 1) * charSpacing;
        firstDigitX = (systemConfig.screenWeight - totalWidth) / 2.0f;

        // 更新Number为处理后的字符串
        Number = temp;

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
            xUnit = (HAL::getSystemConfig().screenWeight - unitWidth) / 2; // 单位显示处于屏幕中间
            xUnitTrg = xUnit;
            yUnit = systemConfig.screenHeight; // 初始化在屏幕下方（不可见）
            yUnitTrg = yExitTrg;               // 与"取消"和"确认"按钮在同一高度
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
        HAL::drawRBox(xSelection, ySelection, wSelection, hSelection, 0);
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
        else if (index == countNumber - 2) // 确认按钮
        {
            xSelectionTrg = HAL::getSystemConfig().screenWeight - buttonWidth; // 向左移动一个按钮宽度
            ySelectionTrg = buttonY;
            wSelectionTrg = buttonWidth;
            hSelectionTrg = buttonHeight;
        }
        else if (index == countNumber - 1) // 单位
        {
            float unitHeight = HAL::getFontHeight();

            xSelectionTrg = (HAL::getSystemConfig().screenWeight - maxUnitFrameWidth) / 2;
            ySelectionTrg = yUnitTrg - unitHeight;
            wSelectionTrg = maxUnitFrameWidth;
            hSelectionTrg = maxUnitFrameHeight;
        }
        else // 数字位
        {
            if (index >= dotIndex)
            {
                // 如果有小数点，小数点后的字符都要加上中间跳过的小数点的长度
                xSelectionTrg = (firstDigitX + 10) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 3) + astra::getUIConfig().numberEditorCharMoveLengthwithDot;
            }
            else
            {
                // 没有小数点，直接乘以字符长度和设定的间距
                xSelectionTrg = (firstDigitX + 10) + astra::getUIConfig().numberEditorCharMoveLengthwithoutDot * (index - 2);
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
        if (index > 0 && index < countNumber - 2)
        {
            int digitIndex = index - 1;
            digits[digitIndex] = (digits[digitIndex] + 1) % 10;
            updateNumberString();
        }
        else if (index == countNumber - 1)
        {
            // 更新单位
            switch (currentUnitState)
            {
            case UnitState::Micro:
                currentUnitState = UnitState::Milli;
                unit = (m_doseType == DoseType::RealTime) ? "mGy/h" : "mGy";
                break;
            case UnitState::Milli:
                currentUnitState = UnitState::Base;
                unit = (m_doseType == DoseType::RealTime) ? "Gy/h" : "Gy";
                break;
            case UnitState::Base:
                currentUnitState = UnitState::Micro;
                unit = (m_doseType == DoseType::RealTime) ? "μGy/h" : "μGy";
                break;
            }
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
        std::string trimmedNumber = Number;
        // trimmedNumber.find_first_not_of('0'):这个函数在字符串中查找第一个不是'0'的字符的位置，如果字符串全是'0'，它会返回 std::string::npos
        // trimmedNumber.size()-1:这是字符串的长度减1，即最后一个字符的索引
        // std::min(trimmedNumber.find_first_not_of('0'), trimmedNumber.size()-1):这个函数返回上面两个值中较小的一个。如果字符串中有非'0'字符，它会返回第一个非'0'字符的位置。如果字符串全是'0'，它会返回字符串长度减1。
        // trimmedNumber.erase(0, ...):erase 函数从字符串中删除字符。第一个参数'0'表示从字符串的开始删除。第二个参数是要删除的字符数，即上面计算的结果。
        // 这行代码的目的是删除字符串开头的所有'0'，但保留最后一个'0'（如果整个字符串都是'0'的话）。
        // 对于 "00123"，它会删除前面的两个'0'，结果是 "123"。
        // 对于 "0.123"，它不会删除任何字符，因为第一个非'0'字符是'.'。
        // 对于 "000"，它会删除前两个'0'，保留最后一个，结果是 "0"。
        trimmedNumber.erase(0, std::min(trimmedNumber.find_first_not_of('0'), trimmedNumber.size() - 1));

        float result = std::stof(trimmedNumber);

        // 根据单位进行换算
        if (unit.compare(0, 3, "mGy") == 0)
        {
            result *= 1000; // 如果单位是 mGy 或 mGy/h，乘以 1000
        }
        else if (unit.compare(0, 2, "Gy") == 0)
        {
            result *= 1000000; // 如果单位是 Gy 或 Gy/h（但不是 μGy 或 μGy/h），乘以 1000000
        }
        // 如果单位是 μGy 或 μGy/h，不需要调整

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
     * @return ConfirmResult 包含确认状态、数值和探头ID的结构体
     */
    astra::Page::ConfirmResult NumberEditor::onConfirm(int _index)
    {
        bool isConfirmed = false;
        float value = 0.0f;

        if (_index == 0 && currentIndex == 0)
        {
            isConfirmed = true;
        }
        else if (_index == 1 && currentIndex == countNumber - 2)
        {
            isConfirmed = true;
            value = convertToFloat();
        }

        return ConfirmResult(isConfirmed, value, this->title);
    }

    //--------------------------------------------------------------------------

    /**
     * @brief GammaDashboard类的构造函数(剂量率界面)
     *
     * 初始化Gamma仪表盘，设置标签和显示行数。
     *
     * @param labels 要显示的标签列表
     */
    GammaDashboard::GammaDashboard(const std::vector<std::string> &labels, DoseDisplayMode mode)
    {
        this->title = "";
        this->pic = generateDefaultPic();
        this->m_labels = labels;
        this->m_displayMode = mode;
        this->m_displayLines = std::min(labels.size(), size_t(4));

        m_rowCoordinates.clear();
        for (size_t i = 0; i < m_displayLines; ++i)
        {
            RowCoordinates row;
            row.value = "0";  // 只是初始赋值，没有实际意义
            row.unit = "μGy"; // 只是初始赋值，没有实际意义
            m_rowCoordinates.push_back(row);
        }

        if (mode == DoseDisplayMode::RealTime)
        {
            registerObserver_doseRate(); // 注册观察者(实时剂量)
        }
        else if (mode == DoseDisplayMode::Accumulated)
        {
            registerObserver_cumulativeDose(); // 注册观察者(累计剂量)
        }
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

        // 计算行高
        lineHeight = systemConfig.screenHeight / float(m_displayLines);

        // 计算最长标签的宽度
        for (auto &label : m_labels)
        {
            labelWidth = std::max(labelWidth, static_cast<float>(HAL::getFontWidth(label)));
        }

        // 初始化单位状态和计算最大的单位框尺寸
        std::vector<std::string> allUnits;
        if (this->m_displayMode == DoseDisplayMode::RealTime)
        {
            allUnits = {"μGy/h", "mGy/h", "Gy/h"};
        }
        else // DoseDisplayMode::Accumulated
        {
            allUnits = {"μGy", "mGy", "Gy"};
        }

        // 计算值宽度（假设最大值为99999.9）
        std::string maxValueStr = "9999.9";
        valueWidth = HAL::getFontWidth(maxValueStr);

        // 计算单位宽度
        unitWidth = 0;
        for (const auto &unitStr : allUnits)
        {
            std::string tempUnit = unitStr;
            float unitWidth_temp = HAL::getFontWidth(tempUnit);
            float frameWidth = unitWidth_temp;

            unitWidth = std::max(unitWidth, frameWidth);
        }

        // 计算总宽度和起始 X 坐标
        totalWidth = labelWidth + valueWidth + unitWidth + 30; // 20 为间隔

        // 确保 m_rowCoordinates 的大小与 m_displayLines 匹配
        m_rowCoordinates.resize(m_displayLines);

        // 判断进入方向，并设置初始值
        float startX = (_direction == ExitDirection::Right) ? systemConfig.screenWeight + 1 : 0 - totalWidth;
        float targetX = (systemConfig.screenWeight - totalWidth) / 2;

        for (size_t i = 0; i < m_rowCoordinates.size(); ++i)
        {
            auto &row = m_rowCoordinates[i];
            row.xRow = startX;
            row.xRowTrg = targetX;

            float doseValue = 0.0f;
            if (this->m_displayMode == DoseDisplayMode::RealTime)
            {
                switch (i)
                {
                case 0:
                    doseValue = data.probe1_realtime_dose;
                    break;
                case 1:
                    doseValue = data.probe2_realtime_dose;
                    break;
                case 2:
                    doseValue = data.probe3_realtime_dose;
                    break;
                case 3:
                    doseValue = data.probe4_realtime_dose;
                    break;
                default:
                    break;
                }
            }
            else if (this->m_displayMode == DoseDisplayMode::Accumulated)
            {
                switch (i)
                {
                case 0:
                    doseValue = data.probe1_cumulative_dose;
                    break;
                case 1:
                    doseValue = data.probe2_cumulative_dose;
                    break;
                case 2:
                    doseValue = data.probe3_cumulative_dose;
                    break;
                case 3:
                    doseValue = data.probe4_cumulative_dose;
                    break;
                default:
                    break;
                }
            }

            row.unit = adjustDoseAndUnit(doseValue);
            row.value = formatFloatToString(doseValue);
        }
        if (!this->is_init)
        {
            this->is_init = true;
        }
    }

    std::string GammaDashboard::adjustDoseAndUnit(const float &dose) const
    {
        std::string baseUnit = (this->m_displayMode == DoseDisplayMode::RealTime) ? "Gy/h" : "Gy";
        if (dose >= 1000000)
        {
            return baseUnit;
        }
        else if (dose >= 1000)
        {
            return "m" + baseUnit;
        }
        else
        {
            return "μ" + baseUnit;
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

        // // 检查是否有报警
        // if (UpdateMessage.p || UpdateMessage.d)
        // {
        //     // 检查蜂鸣器定时器是否激活
        //     if (xTimerIsTimerActive(xBeepTimer) == pdFALSE)
        //     {
        //         // 开启蜂鸣器
        //         key_value_msg("Beep_Start", NULL, 0);
        //     }

        //     // 拼接报警文本
        //     std::string alarmText;
        //     std::string labelStr(UpdateMessage.label); // 将 char* 转换为 std::string
        //     if (UpdateMessage.p && UpdateMessage.d)
        //     {
        //         alarmText = labelStr + "探头 p/d报警";
        //     }
        //     else if (UpdateMessage.p)
        //     {
        //         alarmText = labelStr + "探头 p报警";
        //     }
        //     else
        //     {
        //         alarmText = labelStr + "探头 d报警";
        //     }

        //     // 显示报警提示框
        // }

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
     * @brief 注册观察者(剂量率界面)
     *
     * @note 使用静态局部变量实现的单例模式，可以确保这些资源只被初始化一次，避免了重复初始化和资源浪费
     * @note 将观察者函数添加到探头数据管理器中，以便接收探头数据更新。
     */
    void GammaDashboard::registerObserver_doseRate()
    {
        // 使用静态局部变量实现的单例模式，可以确保这些资源只被初始化一次，避免了重复初始化和资源浪费
        ProbeDataManager::getInstance().addObserver([this](const ProbeDataManager::ProbeData &Probe_data)
                                                    {
        float doseRate = Probe_data.doseRate;
        std::string unit = adjustDoseAndUnit(doseRate);
        this->updateRowData(Probe_data.label, formatFloatToString(doseRate), unit); });
    }

    /**
     * @brief 注册观察者(累计剂量界面)
     *
     * @note 使用静态局部变量实现的单例模式，可以确保这些资源只被初始化一次，避免了重复初始化和资源浪费
     * @note 将观察者函数添加到探头数据管理器中，以便接收探头数据更新。
     */
    void GammaDashboard::registerObserver_cumulativeDose()
    {
        ProbeDataManager::getInstance().addObserver([this](const ProbeDataManager::ProbeData &Probe_data)
                                                    {
        float cumulativeDose = Probe_data.cumulativeDose;
        std::string unit = adjustDoseAndUnit(cumulativeDose);
        this->updateRowData(Probe_data.label, formatFloatToString(cumulativeDose), unit); });
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

        // 将字符串转换为浮点数
        float value = std::stof(newValue);

        // 根据单位进行转换,μGy 或 μGy/h，不需要转换
        if (newUnit.compare(0, 3, "mGy") == 0)
        {
            // mGy 或 mGy/h，乘以 1000
            value *= 1000;
        }
        else if (newUnit.compare(0, 2, "Gy") == 0)
        {
            // Gy 或 Gy/h，乘以 1000000
            value *= 1000000;
        }

        if (this->m_displayMode == DoseDisplayMode::RealTime)
        {
            switch (index)
            {
            case 0:
                data.probe1_realtime_dose = value;
                break;
            case 1:
                data.probe2_realtime_dose = value;
                break;
            case 2:
                data.probe3_realtime_dose = value;
                break;
            case 3:
                data.probe4_realtime_dose = value;
                break;
            default:
                break;
            }
        }
        else if (this->m_displayMode == DoseDisplayMode::Accumulated)
        {
            switch (index)
            {
            case 0:
                data.probe1_cumulative_dose = value;
                break;
            case 1:
                data.probe2_cumulative_dose = value;
                break;
            case 2:
                data.probe3_cumulative_dose = value;
                break;
            case 3:
                data.probe4_cumulative_dose = value;
                break;
            default:
                break;
            }
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
        this->m_displayLines = std::min(labels.size(), size_t(4));

        // 清空容器，并初始进行初始化(不能放在init函数中，因为init函数会被反复调用)
        m_rowCoordinates.clear();
        for (size_t i = 0; i < m_displayLines; ++i)
        {
            RowCoordinates row;
            row.xRow = systemConfig.screenWeight + 1;
            row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
            row.p = false;
            row.d = false;
            m_rowCoordinates.push_back(row);
        }

        registerObserver(); // 注册观察者
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

        // 确保 m_rowCoordinates 的大小与 m_displayLines 匹配
        m_rowCoordinates.resize(m_displayLines);

        if (_direction == ExitDirection::Right)
        {
            for (auto &row : m_rowCoordinates)
            {
                row.xRow = systemConfig.screenWeight + 1;
                row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
            }
        }
        else
        {
            for (auto &row : m_rowCoordinates)
            {
                row.xRow = 0 - totalWidth;
                row.xRowTrg = (systemConfig.screenWeight - totalWidth) / 2;
            }
        }
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
                HAL::drawRBox(xPDX - 2, y - 12, charWidth + 4, lineHeight, 0);
            }
            if (row.d)
            {
                HAL::drawRBox(xPDX + charWidth + 8, y - 12, charWidth + 4, lineHeight, 0);
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
     * @brief 注册观察者
     *
     * 将观察者函数添加到探头数据管理器中，以便接收探头数据更新。
     */
    void PDXDashboard::registerObserver()
    {
        // 这里的data其实就是 updateProbeData 的auto it，也就是指向m_probeData的迭代器
        ProbeDataManager::getInstance().addObserver([this](const ProbeDataManager::ProbeData &data)
                                                    { this->updateWarningStatus(data.label, data.p, data.d); });
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
