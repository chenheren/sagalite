#ifndef SAGALITE_STORAGEFACTORY_H
#define SAGALITE_STORAGEFACTORY_H

#include <memory>
#include <sagalite/manager/IStorage.h>

namespace sagalite
{

class StorageFactory
{
public:
    static std::shared_ptr<IStorage>
    CreateStorage(const std::string& strType="xml");
};

}


#endif //SAGALITE_STORAGEFACTORY_H
