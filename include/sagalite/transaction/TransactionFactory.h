#ifndef SAGALITE_TRANSACTIONFACTORY_H
#define SAGALITE_TRANSACTIONFACTORY_H

#include <memory>
#include <sagalite/manager/ITransactionFactory.h>

namespace sagalite
{
class TransactionFactory: public ITransactionFactory
{
public:
    std::shared_ptr<ITransaction>
    Create(const std::string& name) override;
};
}

#endif //SAGALITE_TRANSACTIONFACTORY_H
