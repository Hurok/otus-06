#include "Utils.h"
#include "Structures.h"
#include <assert.h>

static constexpr auto kCountDots = 3;
static constexpr auto kIdxOct1 = 0;
static constexpr auto kIdxOct2 = 1;
static constexpr auto kIdxOct3 = 2;
static constexpr auto kIdxOct4 = 3;

uint8_t SIpV4::oct1() const noexcept
{
    return ip[kIdxOct1];
}

uint8_t SIpV4::oct2() const noexcept
{
    return ip[kIdxOct2];
}

uint8_t SIpV4::oct3() const noexcept
{
    return ip[kIdxOct3];
}

uint8_t SIpV4::oct4() const noexcept
{
    return ip[kIdxOct4];
}

bool SIpV4::comparer(const SIpV4 &l, const SIpV4 &r) noexcept
{
    if (l.ip[kIdxOct1] == r.ip[kIdxOct1]) {
        if (l.ip[kIdxOct2] == r.ip[kIdxOct2]) {
            if (l.ip[kIdxOct3] == r.ip[kIdxOct3]) {
                return l.ip[kIdxOct4] < r.ip[kIdxOct4];
            }

            return l.ip[kIdxOct3] < r.ip[kIdxOct3];
        }

        return l.ip[kIdxOct2] < r.ip[kIdxOct2];
    }

    return l.ip[kIdxOct1] < r.ip[kIdxOct1];
}

SIpV4 SIpV4::fromString(const std::string &val)
{
    SIpV4 res;
    std::string buffer;
    std::istringstream ss(val);
    int foundDot = -1;
    while (std::getline(ss, buffer, '.')) {
        switch (++foundDot) {
            case kIdxOct1: res.ip[kIdxOct1] = std::atoi(buffer.c_str()); break;
            case kIdxOct2: res.ip[kIdxOct2] = std::atoi(buffer.c_str()); break;
            case kIdxOct3: res.ip[kIdxOct3] = std::atoi(buffer.c_str()); break;
            case kIdxOct4: res.ip[kIdxOct4] = std::atoi(buffer.c_str()); break;
            default: throw std::logic_error("invalid ip-address format");
        }
    }

    if (kCountDots != foundDot)
        throw std::logic_error("invalid ip-address format");

    return res;
}

std::string SIpV4::toString() const noexcept 
{
    assert(ip.size() == 4);
    return std::accumulate(std::next(ip.cbegin()), ip.cend(), std::to_string(*ip.cbegin()), [](std::string res, uint8_t val){
        return std::move(res) + std::string(".") + std::to_string(static_cast<int>(val));
    });
}

std::ostream &operator<<(std::ostream &stream, const SIpV4 &other) noexcept
{
    

    stream << other.toString();
    return stream;
}
