#include "HttpMonitor.h"

#include <utility>

using namespace sagalite;

HttpMonitor::HttpMonitor(Option  option)
: m_option(std::move(option))
{

}

void
HttpMonitor::Update(const std::string& strName, const TransactionStatus& status)
{

}