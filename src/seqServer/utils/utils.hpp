#pragma once
/*
 * utils.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include <bibseq.h>
#include <bibcpp.h>

namespace bibseq {
namespace bfs = boost::filesystem;
std::vector<bfs::path> getOwnFiles(const std::string & dir, const std::string & ext);
std::vector<bfs::path> getLibFiles(const std::string & dir, const std::string & ext);



} /* namespace bibseq */




