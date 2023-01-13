#ifndef SAGALITE_SAGADEFINITION_H
#define SAGALITE_SAGADEFINITION_H

#include <utility>
#include <functional>
#include <unordered_map>
#include <boost/format.hpp>
#include <sagalite/manager/ITransaction.h>

namespace sagalite
{
using SimpleAction = std::function<RunStatus()>;
class SimpleStep: public IStep
{
public:
    explicit
    SimpleStep(const std::string& name, SimpleAction fun)
        : IStep(name) , m_fun(std::move(fun))
    {
    }

    RunStatus
    Tick() override
    {
        if (m_fun)
        {
            return m_fun();
        }
        return RunStatus::FAILED;
    }

private:
    SimpleAction m_fun;
};

class SagaDefinition
{
public:
    using StepBuilder = std::function<std::shared_ptr<IStep>(const std::string&)>;

    explicit
    SagaDefinition(std::shared_ptr<ITransaction> pTransaction);

    /**
     * @brief 注册步骤类型
     * @param ID 类型ID
     */
    template<typename T> void
    RegisterStep(const std::string& ID)
    {
        static_assert(std::is_base_of<IStep, T>::value,"[RegisterStep]: accepts only classed derived from either IStep");
        static_assert(!std::is_abstract<T>::value, "[RegisterStep]: tick() must be overridden");
        constexpr bool constructWithName = std::is_constructible<T, const std::string&>::value;
        static_assert(constructWithName, "[RegisterStep]: the registered class must have constructors: (const std::string& name).");

        auto iter = m_builders.find(ID);
        if (iter != m_builders.end())
        {
            auto fmt = boost::format("ID [%s] already registered")%ID.c_str();
            throw std::runtime_error(fmt.str().c_str());
        }

        m_builders.insert({ID, [=](const std::string& name)
        {
            return std::shared_ptr<IStep>(new T(name));
        }});
    }

    /**
    * @brief 注册简单步骤类型
    * @param ID 类型ID
    */
    void
    RegisterSimpleStep(const std::string& ID, const SimpleAction& callback)
    {
        auto iter = m_builders.find(ID);
        if (iter != m_builders.end())
        {
            auto fmt = boost::format("ID [%s] already registered")%ID.c_str();
            throw std::runtime_error(fmt.str().c_str());
        }

        m_builders.insert({ID, [=](const std::string& name){
            return std::shared_ptr<IStep>(new SimpleStep(name, callback));}
        });
    }

    /**
     * @brief 注册向前步骤
     * @param name 步骤名称
     */
    template<typename T> SagaDefinition&
    Forward(const std::string& name)
    {
        auto pStep = std::make_shared<T>();
        Invoke(std::move(pStep), true);
        return *this;
    }

    SagaDefinition&
    ForwardSimple(const std::string& name, SimpleAction action)
    {
        auto pStep = std::make_shared<SimpleStep>(name, std::move(action));
        Invoke(std::move(pStep), true);
        return *this;
    }

    /**
     * @brief 注册回滚步骤
     * @param name 步骤名称
     */
    template<typename T> SagaDefinition&
    Rollback(const std::string& name)
    {
        auto pStep = std::make_shared<T>();
        Invoke(name, std::move(pStep), false);
        return *this;
    }

    SagaDefinition&
    RollbackSimple(const std::string& name, SimpleAction action)
    {
        auto pStep = std::make_shared<SimpleStep>(name, std::move(action));
        Invoke(std::move(pStep), false);
        return *this;
    }

    SagaDefinition&
    DefineFromText();

private:
    void
    Invoke(std::shared_ptr<IStep> pStep, bool forward);

private:
    std::shared_ptr<ITransaction> m_pTransaction;
    std::unordered_map<std::string, StepBuilder> m_builders;
};

}

#endif //SAGALITE_SAGADEFINITION_H
