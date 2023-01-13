#include <sagalite/persistence/StorageFactory.h>
#include "FileStorage.h"

using namespace sagalite;

std::shared_ptr<IStorage>
StorageFactory::CreateStorage(const std::string &strType)
{
    if ("xml" == strType)
    {
        return std::make_shared<FileStorage>();
    }

    throw std::runtime_error("storage type unsupported.");
}
