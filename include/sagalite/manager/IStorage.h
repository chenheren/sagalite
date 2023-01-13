#ifndef SAGALITE_ISTORAGE_H
#define SAGALITE_ISTORAGE_H

#include <memory>

namespace sagalite
{
class IStorage
{
public:
    virtual void
    Save(const std::string& name, const std::string& data) = 0;

    virtual std::string
    GetByName(const std::string& name) = 0;
};
}

#endif //SAGALITE_ISTORAGE_H
