#ifndef SAGALITE_SAGAMANAGER_H
#define SAGALITE_SAGAMANAGER_H

#include <mutex>
#include <string>
#include <memory>
#include <unordered_map>
#include <sagalite/manager/IMonitor.h>
#include <sagalite/manager/IStorage.h>
#include <sagalite/manager/ITransactionFactory.h>

namespace sagalite
{
class SagaManager
{
public:
    explicit
    SagaManager(std::shared_ptr<IStorage> pStorage,
                std::shared_ptr<IMonitor> pMonitor,
                std::shared_ptr<ITransactionFactory> pFactory);

    ~SagaManager();

    std::shared_ptr<ITransaction>
    CreateTransaction(const std::string& name);

    void
    DestroyTransaction(const std::string& name);

    void
    Start(const std::string& name, const std::string& step = "");

    void
    Pause(const std::string& name);

    void
    Continue(const std::string& name);

    void
    Resume(const std::string& name);

    void
    Stop(const std::string& name);

    void
    WaitForFinished();

private:
    std::shared_ptr<ITransaction>
    FindTransaction(const std::string& name);

private:
    std::mutex m_mutex;
    std::shared_ptr<IStorage> m_pStorage;
    std::shared_ptr<IMonitor> m_pMonitor;
    std::shared_ptr<ITransactionFactory> m_pTransactionFactory;
    std::unordered_map<std::string, std::shared_ptr<ITransaction>> m_transactions;
};

}

#endif //SAGALITE_SAGAMANAGER_H
