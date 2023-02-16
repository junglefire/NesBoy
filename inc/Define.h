#ifndef NB_DEFINE_H__
#define NB_DEFINE_H__

#include <functional>
#include <fstream>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#ifndef NAMESPACE_BEGIN
#define NAMESPACE_BEGIN(x) namespace x {
#endif //NAMESPACE_BEGIN

#ifndef NAMESPACE_END
#define NAMESPACE_END(x) }
#endif //NAMESPACE_END

#define _info spdlog::info
#define _debug spdlog::debug
#define _error spdlog::error
#define _warn spdlog::warn

using Byte = std::uint8_t;
using Address = std::uint16_t;

#endif