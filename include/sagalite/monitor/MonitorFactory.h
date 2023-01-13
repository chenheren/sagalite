#ifndef SAGALITE_MONITORFACTORY_H
#define SAGALITE_MONITORFACTORY_H

#include <memory>
#include <sagalite/manager/IMonitor.h>

namespace sagalite
{

class MonitorFactory
{
public:
    static std::shared_ptr<IMonitor>
    CreateMonitor(const std::string& strType="http");
};

}

#endif //SAGALITE_MONITORFACTORY_H
