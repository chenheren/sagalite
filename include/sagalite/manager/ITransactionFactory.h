#ifndef SAGALITE_ITRANSACTIONFACTORY_H
#define SAGALITE_ITRANSACTIONFACTORY_H

#include <memory>
#include <sagalite/manager/ITransaction.h>

namespace sagalite
{
class ITransactionFactory
{
public:
    virtual std::shared_ptr<ITransaction>
    Create(const std::string& name) = 0;
};
}

#endif //SAGALITE_ITRANSACTIONFACTORY_H
