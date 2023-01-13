#ifndef SAGALITE_TRANSACTION_H
#define SAGALITE_TRANSACTION_H

#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include <sagalite/manager/SagaDefinition.h>
#include <sagalite/manager/ITransaction.h>

namespace sagalite
{
class Transaction: public ITransaction
{
public:
    enum Control
    {
        CONTROL_IDLE,
        CONTROL_RUNNING,
        CONTROL_PAUSE,
    };

    struct Context
    {
        /**< 控制状态 */
        std::atomic<Control> control{CONTROL_IDLE};
        /**< 当前状态 */
        std::atomic_bool running{false};
        /**< 开始步骤 */
        std::string startStep;
        /**< 步骤序号 */
        int currentIndex{-1};
        /**< 步骤名称 */
        std::string currentName;
        /**< 是否在回滚 */
        bool currentForward{false};
        /**< 数据 */
        std::unordered_map<std::string, std::string> userData;
    };

    struct StepPlan
    {
        /**< 前进步骤 */
        std::shared_ptr<IStep> pForward{nullptr};
        /**< 回滚步骤 */
        std::shared_ptr<IStep> pRollback{nullptr};
    };

    explicit
    Transaction(std::string  name);

    ~Transaction();

    std::string
    Name() override;

    bool
    Running() override;

    void
    WaitForFinished() override;

    void
    Start(const std::string& step) override;

    void
    Stop() override;

    void
    Pause() override;

    void
    Continue() override;

    void
    Resume() override;

    void
    Invoke(std::shared_ptr<IStep> pStep, bool forward) override;

private:
    void
    Run();

    int
    GetIndexByName(const std::string& name);

    void
    WaitForStepFinished();

private:
    /**< 事务名称 */
    std::string m_name;
    /**< 运行标志 */
    std::atomic_bool m_runningFlag;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_thread;

    Context m_context;
    std::vector<StepPlan> m_plans;
};
}

#endif //SAGALITE_TRANSACTION_H
