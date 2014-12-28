/*
 * utils.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "utils.hpp"

namespace bibseq {

std::vector<bfs::path> getOwnFiles(const std::string & dir, const std::string & ext){
	auto files = bib::files::listAllFiles(dir + "/own", true, bibseq::VecStr{ext});
	return bib::getVecOfMapKeys(files);
}
std::vector<bfs::path> getLibFiles(const std::string & dir, const std::string & ext){
	auto files = bib::files::listAllFiles(dir + "/libs", true, bibseq::VecStr{ext});
	return bib::getVecOfMapKeys(files);
}

} /* namespace bibseq */
