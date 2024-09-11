#include "ProbeDataManager.h"
#include <algorithm>

namespace yomu
{
    // 获取ProbeDataManager的单例实例
    ProbeDataManager &ProbeDataManager::getInstance()
    {
        // 创建一个静态局部变量，确保只有一个实例
        static ProbeDataManager instance;
        return instance;
    }

    // 构造函数，初始化探头数据
    ProbeDataManager::ProbeDataManager()
        : m_probeData{{{"探头1", 0.0f, false, false},
                       {"探头2", 0.0f, false, false}}} {}

    // 更新探头数据
    void ProbeDataManager::updateProbeData(const std::string &label, float doseRate, float cumulativeDose, bool p, bool d)
    {
        // 查找匹配标签的探头数据
        // 这里的 it 可以理解为指向 m_probeData 中某个元素的指针
        auto it = std::find_if(m_probeData.begin(), m_probeData.end(),
                               [&label](const ProbeData &data)
                               { return data.label == label; });

        // 如果找到匹配的探头数据
        if (it != m_probeData.end())
        {
            it->doseRate = doseRate;
            it->cumulativeDose = cumulativeDose;
            it->p = p;
            it->d = d;

            // 通知所有观察者
            for (const auto &observer : m_observers)
            {
                // *it 是解引用操作。它获取 it 指向的实际 ProbeData 对象
                // observer 是一个函数对象。它可能是一个函数指针、lambda 表达式或者任何可调用的对象
                // observer(*it) 是在调用这个函数对象，并将 *it（也就是更新后的 ProbeData 对象）作为参数传递给它
                // 比如说有个这个函数 void updateDashboard(const ProbeData& data) {
                // std::cout << "更新仪表盘：" << data.label << " 的新剂量率是 " << data.doseRate << std::endl;
                // }
                // 当我们将这个函数添加为观察者时，observer(*it); 实际上就相当于在调用 updateDashboard(*it);
                observer(*it);
            }
        }
    }

    // 获取所有探头数据的常量引用
    const std::array<ProbeDataManager::ProbeData, 2> &ProbeDataManager::getProbeData() const
    {
        return m_probeData;
    }

    // 添加观察者
    void ProbeDataManager::addObserver(Observer observer)
    {
        m_observers.push_back(observer);
    }
}
