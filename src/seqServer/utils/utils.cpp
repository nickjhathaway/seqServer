//
// seqServer - A library for analyzing sequence data
// Copyright (C) 2012-2018 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
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
/*
 * utils.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "utils.hpp"

namespace njhseq {

std::vector<bfs::path> getOwnFiles(const std::string & dir, const std::string & ext){
	auto files = njh::files::listAllFiles(dir + "/own", true, njhseq::VecStr{ext});
	return njh::getVecOfMapKeys(files);
}
std::vector<bfs::path> getLibFiles(const std::string & dir, const std::string & ext){
	auto files = njh::files::listAllFiles(dir + "/libs", true, njhseq::VecStr{ext});
	return njh::getVecOfMapKeys(files);
}



namespace seqServer {

std::string getSeqServerInstallDir() {
	return seqServer_INSTALLDIR;
}

std::string getSeqServerInstallCoreDir() {
	return njh::files::make_path(seqServer_INSTALLDIR,
			"etc/resources/seqServerCore").string();
}

}  // namespace seqServer

} /* namespace njhseq */
