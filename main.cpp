#include "Utils.h"
#include "Structures.h"

std::vector<std::string> readLines()
{
    std::vector<std::string> lines;
    for (std::string buffer; std::getline(std::cin, buffer);) {
        lines.push_back(buffer);
    }

    return lines;
}

std::vector<SIpV4> toIpV4(std::vector<std::string> &lines)
{
    std::vector<SIpV4> listIpV4;
    for (auto &i : lines) {
        normalizeLine(i);

        auto it = std::find_if(i.cbegin(), i.cend(), [](const char ch) -> bool {
            return ch == '\t';
        });

        if (it == i.cend())
            throw std::logic_error("not found ip address in line");

        auto ipv4 = SIpV4::fromString(std::string(i.cbegin(), it));
        listIpV4.push_back(ipv4);
    }

    return listIpV4;
}

int main(int, char**) {   
    try {
        auto lines = readLines();
        auto listIpV4 = toIpV4(lines);

        // out result
        std::sort(listIpV4.rbegin(), listIpV4.rend(), SIpV4::comparer);

        std::stringstream ssIp1;            // temp for result
        std::stringstream ssIp46_70;        // temp for result
        std::stringstream ssIp46Any;        // temp for result

        const int sz = listIpV4.size();
        for (int i = 0; i < sz; ++i) {
            const auto &ip = listIpV4.at(i);

            if (ip.oct1() == 1)
                ssIp1 << ip << "\n";

            if (ip.oct1() == 46 && ip.oct2() == 70)
                ssIp46_70 << ip << "\n";

            if (ip.oct1() == 46 || ip.oct2() == 46 || ip.oct3() == 46 || ip.oct4() == 46)
                ssIp46Any << ip << "\n";

            std::cout << ip << "\n";
        }
        
        auto ssIp1Str = ssIp1.str();
        auto ssIp46_70Str = ssIp46_70.str();
        auto ssIp46AnyStr = ssIp46Any.str();

        std::cout   << rtrim(ssIp1Str) << "\n"
                    << rtrim(ssIp46_70Str) << "\n"
                    << rtrim(ssIp46AnyStr) << "\n";
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}

