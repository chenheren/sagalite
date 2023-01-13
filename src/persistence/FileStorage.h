#ifndef SAGALITE_FILESTORAGE_H
#define SAGALITE_FILESTORAGE_H

#include <sagalite/manager/IStorage.h>

namespace sagalite
{
class FileStorage: public IStorage
{
public:
    explicit
    FileStorage();

    ~FileStorage();

    void
    Save(const std::string& name, const std::string& data) override;

    std::string
    GetByName(const std::string& name) override;

private:

};
}

#endif //SAGALITE_FILESTORAGE_H
