#pragma once

#include <map>
#include <ostream>

namespace utils {

class GetIFaddrs {
private:
    struct ifaddrs *ifaddr;
    std::map<std::string, std::string> adapter_ip_;

    void find_ips();

public:
    GetIFaddrs();
    ~GetIFaddrs();

    const std::map<std::string, std::string>& ips() const {
        return adapter_ip_;
    }

    std::string get_host_ip() const;
};

std::ostream& operator<< (std::ostream& s, const GetIFaddrs& e);

std::string GetHosIP();

} // namespace utils
