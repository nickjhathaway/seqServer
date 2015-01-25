// from http://stackoverflow.com/a/12131131
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ip.hpp"
#include <bibcpp/debug/exception.hpp>

namespace utils {

void GetIFaddrs::find_ips(){
    for(auto* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
        if(NULL == ifa->ifa_addr){
            continue;
        }

        char host[NI_MAXHOST];
        const int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                  host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        const auto family = ifa->ifa_addr->sa_family;
        if(AF_INET == family){
            if(0 == s){
                std::string name = ifa->ifa_name;
                std::string ip = host;
                adapter_ip_[name] = ip;
            }
        }
    }
}

GetIFaddrs::GetIFaddrs(){
    if (getifaddrs(&ifaddr) == -1){
        throw bib::err::Exception("could not getifaddrs");
    }

    find_ips();
}

GetIFaddrs::~GetIFaddrs(){
    freeifaddrs(ifaddr);
}

std::string GetIFaddrs::get_host_ip() const {
    for(auto& name : {"eth0", "eth1"}){
        auto it = adapter_ip_.find(name);
        if(adapter_ip_.end() != it){
            return it->second;
        }
    }
    throw bib::err::Exception("could not determine host ip");
}

std::ostream& operator<< (std::ostream& s, const GetIFaddrs& e) {
    for(const auto& kv : e.ips()){
        s << kv.first << ": " << kv.second << std::endl;
    }
    return s;
}

std::string GetHosIP(){
    GetIFaddrs ips;
    return ips.get_host_ip();
}

} // namespace utils
