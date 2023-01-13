#include <sagalite/monitor/MonitorFactory.h>
#include "HttpMonitor.h"

using namespace sagalite;

std::shared_ptr<IMonitor>
MonitorFactory::CreateMonitor(const std::string& strType)
{
    if ("http" == strType)
    {
        return std::make_shared<HttpMonitor>();
    }

    throw std::runtime_error("monitor type unsupported.");
}
