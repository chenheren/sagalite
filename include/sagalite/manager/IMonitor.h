#ifndef SAGALITE_IMONITOR_H
#define SAGALITE_IMONITOR_H

#include <string>
#include <unordered_map>

namespace sagalite
{

class TransactionStatus
{
public:
    /**< 事务名称 */
    std::string transactionName;
    /**< 当前步骤 */
    int stepName;
    /**< 用户数据 */
    std::unordered_map<std::string, std::string> userData;
};

class IMonitor
{
public:
    virtual void
    Update(const std::string& strName, const TransactionStatus& status) = 0;
};

}

#endif //SAGALITE_IMONITOR_H
