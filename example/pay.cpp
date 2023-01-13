#include <iostream>
#include <sagalite/manager/SagaManager.h>
#include <sagalite/manager/SagaDefinition.h>
#include <sagalite/monitor/MonitorFactory.h>
#include <sagalite/persistence/StorageFactory.h>
#include <sagalite/transaction/TransactionFactory.h>

using namespace sagalite;

int main()
{
    auto pStorage = StorageFactory::CreateStorage();
    auto pMonitor = MonitorFactory::CreateMonitor();
    auto pFactory = std::make_shared<TransactionFactory>();
    SagaManager manager(pStorage, pMonitor, pFactory);

    auto payTransaction = manager.CreateTransaction("pay");

    SagaDefinition definition(payTransaction);
    definition.ForwardSimple("GoToMarket", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    })
    .RollbackSimple("GoBackHome", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    })
    .ForwardSimple("BuySomething", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    })
    .RollbackSimple("ReturnSomething", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    })
    .ForwardSimple("CheckMoney", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    })
    .ForwardSimple("PayForSomething", [](){
        sleep(1);
        return RunStatus::FAILED;
    })
    .ForwardSimple("GoBackHome", [](){
        sleep(1);
        return RunStatus::SUCCESS;
    });

    manager.Start("pay");

    sleep(2);
    manager.Pause("pay");

    sleep(2);
    manager.Continue("pay");

    manager.WaitForFinished();

//    manager.Start("pay", "BuySomething");
//    manager.WaitForFinished();

    return 0;
}