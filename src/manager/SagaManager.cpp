#include <sagalite/manager/SagaManager.h>
#include <boost/format.hpp>
#include <thread>

using namespace sagalite;

SagaManager::SagaManager(std::shared_ptr<IStorage> pStorage,
     std::shared_ptr<IMonitor> pMonitor,
     std::shared_ptr<ITransactionFactory> pFactory)
 : m_pStorage(pStorage)
 , m_pMonitor(pMonitor)
 , m_pTransactionFactory(std::move(pFactory))
{

}

SagaManager::~SagaManager()
{

}

std::shared_ptr<ITransaction>
SagaManager::CreateTransaction(const std::string& name)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_transactions.find(name);
    if (iter == m_transactions.end())
    {
        auto pTransaction =  m_pTransactionFactory->Create(name);
        m_transactions.insert({name, pTransaction});
        return pTransaction;
    }

    auto fmt = boost::format("cannot create transaction [%s] which is defined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::DestroyTransaction(const std::string& name)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_transactions.find(name);
    if (iter != m_transactions.end())
    {
        m_transactions.erase(iter);
    }

    auto fmt = boost::format("cannot destroy transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::Start(const std::string& name, const std::string& step)
{
    auto pTransaction = FindTransaction(name);
    if (pTransaction)
    {
        return pTransaction->Start(step);
    }

    auto fmt = boost::format("cannot start transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::Pause(const std::string& name)
{
    auto pTransaction = FindTransaction(name);
    if (pTransaction)
    {
        return pTransaction->Pause();
    }

    auto fmt = boost::format("cannot pause transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::Continue(const std::string& name)
{
    auto pTransaction = FindTransaction(name);
    if (pTransaction)
    {
        return pTransaction->Continue();
    }

    auto fmt = boost::format("cannot pause transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::Resume(const std::string& name)
{
    auto pTransaction = FindTransaction(name);
    if (pTransaction)
    {
        return pTransaction->Continue();
    }

    auto fmt = boost::format("cannot resume transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::Stop(const std::string& name)
{
    auto pTransaction = FindTransaction(name);
    if (pTransaction)
    {
        return pTransaction->Stop();
    }

    auto fmt = boost::format("cannot stop transaction [%s] which is undefined")%name.c_str();
    throw std::runtime_error(fmt.str().c_str());
}

void
SagaManager::WaitForFinished()
{
    std::unordered_map<std::string, std::shared_ptr<ITransaction>> waitTransactions;
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    {
        waitTransactions = m_transactions;
    }

    for (const auto& item: waitTransactions)
    {
        item.second->WaitForFinished();
    }
}

std::shared_ptr<ITransaction>
SagaManager::FindTransaction(const std::string& name)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_transactions.find(name);
    if (iter != m_transactions.end())
    {
        return iter->second;
    }
    return nullptr;
}

