#include <utility>
#include <iostream>
#include <boost/format.hpp>
#include <sagalite/utility/Logger.h>
#include "Transaction.h"

using namespace sagalite;

Transaction::Transaction(std::string  name)
: m_name(std::move(name))
, m_runningFlag(true)
{
    m_thread = std::thread(&Transaction::Run, this);
}

Transaction::~Transaction()
{
    m_runningFlag = false;
    m_context.control = CONTROL_IDLE;
    m_cv.notify_all();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

std::string
Transaction::Name()
{
    return m_name;
}

bool
Transaction::Running()
{
    return m_context.control != CONTROL_IDLE;
}

void
Transaction::WaitForFinished()
{
    while (m_runningFlag &&  m_context.control == CONTROL_RUNNING)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void
Transaction::Start(const std::string& step)
{
    if (m_context.control == CONTROL_IDLE)
    {
        m_context.control = CONTROL_RUNNING;
        m_context.startStep = step;
        m_context.running = true;
        m_context.currentIndex = 0;
        m_context.currentForward = false;
        m_cv.notify_all();
        LOG_INFO("transaction [%s] start\n", m_name.c_str());
    }
    else
    {
        LOG_WARN("transaction [%s] is running\n", m_name.c_str());
    }
}

void
Transaction::Stop()
{
    m_context.control = CONTROL_IDLE;
    WaitForStepFinished();
}

void
Transaction::Pause()
{
    if (m_context.control == CONTROL_RUNNING)
    {
        m_context.control = CONTROL_PAUSE;
        WaitForStepFinished();

        /**< 保存当前步骤上下文数据 */
        if (m_context.currentIndex >= 0 && m_context.currentIndex < m_plans.size())
        {
            std::unordered_map<std::string, std::string> userData;
            if (!m_context.currentForward)
            {
                m_context.currentName = m_plans[m_context.currentIndex].pForward->Name();
                m_plans[m_context.currentIndex].pForward->Serialize(userData);
            }
            else
            {
                m_context.currentName = m_plans[m_context.currentIndex].pRollback->Name();
                m_plans[m_context.currentIndex].pRollback->Serialize(userData);
            }

            userData["transaction.name"] = m_name;
            userData["transaction.stepName"] = m_context.currentName;
            userData["transaction.stepIndex"] = std::to_string(m_context.currentIndex);
            userData["transaction.isRollback"] = std::to_string(m_context.currentForward);
        }

        LOG_INFO("transaction [%s] pause in step [%s]\n",
                 m_name.c_str(), m_context.currentName.c_str());
    }
    else
    {
        LOG_WARN("transaction [%s] is %s\n", m_name.c_str(),
                 m_context.control == CONTROL_IDLE ? "CONTROL_IDLE" : "CONTROL_PAUSE");
    }
}

void
Transaction::Continue()
{
    if (m_context.control == CONTROL_PAUSE)
    {
        LOG_INFO("transaction [%s] continue from step [%s]\n", m_name.c_str(), m_context.currentName.c_str());
        m_context.control = CONTROL_RUNNING;
        m_context.running = true;
        m_context.startStep = "";
        m_cv.notify_all();
    }
    else
    {
        LOG_WARN("cannot continue transaction [%s] which is %s\n", m_name.c_str(),
                 m_context.control == CONTROL_IDLE ? "CONTROL_IDLE" : "CONTROL_RUNNING");
    }
}

void
Transaction::Resume()
{
    if (m_context.control == CONTROL_IDLE)
    {
        LOG_INFO("transaction [%s] RESUME from step [%s]\n", m_name.c_str(), m_context.currentName.c_str());
        m_context.control = CONTROL_RUNNING;
        m_context.startStep = "";
        m_cv.notify_all();
    }
    else
    {
        LOG_WARN("cannot resume transaction [%s] which is %s\n", m_name.c_str(),
                 m_context.control == CONTROL_IDLE ? "CONTROL_PAUSE" : "CONTROL_RUNNING");
    }
}

void
Transaction::Invoke(std::shared_ptr<IStep> pStep, bool forward)
{
    if (m_context.control != CONTROL_IDLE)
    {
        LOG_WARN("transaction [%s] is running", m_name.c_str());
        return;
    }
    if (m_plans.empty() && ! forward)
    {
        LOG_WARN("the first step must be forward");
        return;
    }
    if (!m_plans.empty() && ! forward && m_plans[m_plans.size()-1].pRollback)
    {
        LOG_WARN("there is already a rollback step for the forward step [%s]",
                 m_plans[m_plans.size()-1].pForward->Name().c_str());
        return;
    }

    if (forward)
    {
        m_plans.push_back({pStep, nullptr});
    }
    else
    {
        m_plans[m_plans.size()-1].pRollback = pStep;
    }
}

void
Transaction::Run()
{
    while (m_runningFlag.load())
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this](){
                return (m_context.control == CONTROL_RUNNING &&
                        m_context.currentIndex < m_plans.size()) ||
                       !m_runningFlag.load();
            });
        }
        /**< 退出线程 */
        if (!m_runningFlag.load()) break;

        m_context.running = true;

        int logIndex = -1;
        int startIndex = GetIndexByName(m_context.startStep);
        m_context.currentIndex = startIndex==-1? m_context.currentIndex:startIndex;

        /**< 遍历并执行步骤 */
        while (m_runningFlag.load() &&
               m_context.control == CONTROL_RUNNING &&
               m_context.currentIndex < m_plans.size() &&
               m_context.currentIndex >= startIndex)
        {
            if (m_context.currentForward)
            {
                if(m_plans[m_context.currentIndex].pRollback)
                {
                    LOG_INFO("rollback step [%s] start\n", m_plans[m_context.currentIndex].pRollback->Name().c_str());
                    m_plans[m_context.currentIndex].pRollback->Tick();
                    LOG_INFO("rollback step [%s] finish\n", m_plans[m_context.currentIndex].pRollback->Name().c_str());
                }

                m_context.currentIndex--;
                continue;
            }

            if (logIndex != m_context.currentIndex)
            {
                LOG_INFO("forward step [%s] start\n", m_plans[m_context.currentIndex].pForward->Name().c_str());
                logIndex = m_context.currentIndex;
            }

            if (RunStatus::SUCCESS == m_plans[m_context.currentIndex].pForward->Tick())
            {
                LOG_INFO("forward step [%s] finish success\n", m_plans[m_context.currentIndex].pForward->Name().c_str());
                m_context.currentIndex++;
                continue;
            }
            else if (RunStatus::RUNNING == m_plans[m_context.currentIndex].pForward->Tick())
            {

            }
            else if (RunStatus::FAILED == m_plans[m_context.currentIndex].pForward->Tick())
            {
                LOG_INFO("forward step [%s] finish failed\n", m_plans[m_context.currentIndex].pForward->Name().c_str());
                m_context.currentForward = true;
                continue;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        m_context.running = false;
    }
}

int
Transaction::GetIndexByName(const std::string& name)
{
    if (name.empty())
    {
        return -1;
    }

    for (int i=0; i < m_plans.size(); i++)
    {
        if (name == m_plans[i].pForward->Name())
        {
            return  i;
        }
    }
    return -1;
}

void
Transaction::WaitForStepFinished()
{
    while (m_runningFlag && m_context.running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}