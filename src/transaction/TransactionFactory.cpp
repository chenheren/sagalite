#include <sagalite/transaction/TransactionFactory.h>
#include "Transaction.h"

using namespace sagalite;

std::shared_ptr<ITransaction>
TransactionFactory::Create(const std::string& name)
{
    return std::make_shared<Transaction>(name);
}
