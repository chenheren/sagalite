#include <sagalite/manager/SagaDefinition.h>

using namespace sagalite;

SagaDefinition::SagaDefinition(std::shared_ptr<ITransaction> pTransaction)
: m_pTransaction(std::move(pTransaction))
{

}

SagaDefinition&
SagaDefinition::DefineFromText()
{

}

void
SagaDefinition::Invoke(std::shared_ptr<IStep> pStep, bool forward)
{
    m_pTransaction->Invoke(std::move(pStep), forward);
}
