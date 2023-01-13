#ifndef SAGALITE_ITRANSACTION_H
#define SAGALITE_ITRANSACTION_H

#include <string>
#include <utility>
#include <unordered_map>
#include <sagalite/manager/Define.h>

namespace sagalite
{
class SagaDefinition;

enum class RunStatus
{
    /**< 未运行，初始状态 */
    IDLE,
    /**< 运行中，异步步骤专用 */
    RUNNING,
    /**< 运行成功 */
    SUCCESS,
    /**< 运行失败 */
    FAILED
};

class IStep
{
public:
    explicit
    IStep(std::string name)
    : m_name(std::move(name))
    , m_status(RunStatus::IDLE)
    {
    }

    /**
     * @brief 步骤名称
     */
    std::string
    Name()
    {
        return m_name;
    }

    /**
     * @brief 步骤状态
     */
    RunStatus
    Status()
    {
        return m_status;
    }

    /**
     * @brief 上下文数据，用于监控、保存等，
     * 示例： data["ip"] = ip; data["host"] = m_host;
     */
    virtual void
    GetContext(std::unordered_map<std::string, std::string>& data)
    {

    }

    /**
     * @brief 序列化
     */
    virtual void
    Serialize(std::unordered_map<std::string, std::string>& data)
    {

    }

    /**
     * @brief 反序列化
     */
    virtual void
    Deserialize(const std::unordered_map<std::string, std::string>& data)
    {

    }

    /**
     * @brief 步骤触发
     */
    virtual RunStatus
    Tick() =0;

protected:
    std::string m_name;
    RunStatus m_status;
};

class ITransaction
{
public:
    /**
     * @brief 事务名称
     */
    virtual std::string
    Name() = 0;

    /**
     * @brief 是否在运行
     */
    virtual bool
    Running() = 0;

    /**
     * @brief 等待运行完成
     */
    virtual void
    WaitForFinished() = 0;

    /**
     * @brief 启动事务
     */
    virtual void
    Start(const std::string& step) = 0;

    /**
     * @brief 停止事务
     */
    virtual void
    Stop() = 0;

    /**
     * @brief 恢复事务
     */
    virtual void
    Pause() = 0;

    /**
     * @brief 继续事务
     */
    virtual void
    Continue() = 0;

    virtual void
    Resume() = 0;

    /**
    * @brief 插入步骤
    */
    virtual void
    Invoke(std::shared_ptr<IStep> pStep, bool forward) = 0;
};
}

#endif //SAGALITE_ITRANSACTION_H
