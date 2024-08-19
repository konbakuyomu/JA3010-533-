#pragma once
#include <array>
#include <string>
#include <vector>
#include <functional>

namespace yomu
{
    class ProbeDataManager
    {
    public:
        // 定义探头数据结构
        struct ProbeData
        {
            std::string label;    // 探头标签
            float doseRate;       // 剂量率
            float cumulativeDose; // 累计剂量
            bool p;               // P警告状态
            bool d;               // D警告状态
        };

        // 获取ProbeDataManager的单例实例
        static ProbeDataManager &getInstance();

        // 更新探头数据
        void updateProbeData(const std::string &label, float doseRate,float cumulativeDose, bool p, bool d);

        // 获取所有探头数据的常量引用
        const std::array<ProbeData, 4> &getProbeData() const;

        // 定义观察者类型，使用std::function来实现回调
        // std::function 是 C++11 引入的一个通用的函数封装器。它可以存储、复制和调用任何可调用目标，包括普通函数、lambda 表达式、函数对象（仿函数）、以及成员函数
        // 这行代码定义了一个类型别名 Observer，它是一个 std::function 类型，表示一个接受 const ProbeData& 参数并返回 void 的函数
        using Observer = std::function<void(const ProbeData &)>;

        // 添加观察者
        void addObserver(Observer observer);

    private:
        // 私有构造函数，防止外部直接创建实例
        ProbeDataManager();

        // 存储探头数据的数组，固定大小为4
        std::array<ProbeData, 4> m_probeData;

        // 存储观察者的vector，也就是存储观察者的回调函数
        std::vector<Observer> m_observers;
    };
}
