#ifndef SAGALITE_HTTPMONITOR_H
#define SAGALITE_HTTPMONITOR_H

#include <sagalite/manager/IMonitor.h>

namespace sagalite
{
class HttpMonitor: public IMonitor
{
public:
    struct Option
    {
        std::string host;
        int port;

        Option()
        : host("localhost")
        , port(10001)
        {
        }
    };

    explicit
    HttpMonitor(Option option = Option());

    void
    Update(const std::string& strName, const TransactionStatus& status) override;

private:
    Option m_option;
};

}


#endif //SAGALITE_HTTPMONITOR_H
