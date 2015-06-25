#pragma once
//
// seqServer - A library for visualizing sequence results data
// Copyright (C) 2012, 2015 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
// Jeffrey Bailey <Jeffrey.Bailey@umassmed.edu>
//
// This file is part of seqServer.
//
// seqServer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// seqServer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with seqServer.  If not, see <http://www.gnu.org/licenses/>.
//

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
