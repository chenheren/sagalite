#ifndef SAGALITE_LOGGER_H
#define SAGALITE_LOGGER_H

#include <cstdio>

#define LOG_INFO(format, ...) (std::printf("[sagalite] " format, ##__VA_ARGS__))

#define LOG_DEBUG(format, ...) (std::printf("[sagalite] " format, ##__VA_ARGS__))

#define LOG_WARN(format, ...) (std::printf("[sagalite] " format, ##__VA_ARGS__))

#define LOG_ERROR(format, ...) (std::printf("[sagalite] " format, ##__VA_ARGS__))

namespace sagalite
{

}

#endif //SAGALITE_LOGGER_H
