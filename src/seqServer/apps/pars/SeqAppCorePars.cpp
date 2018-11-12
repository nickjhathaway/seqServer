/*
 * SeqAppCorePars.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: nick
 */

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

#include "SeqAppCorePars.hpp"


namespace bibseq {

void SeqAppCorePars::setCoreOptions(seqSetUp & setUp){
	setUp.setOption(port_, "--port", "Port Number to Serve On");
	setUp.setOption(name_, "--name", "Name of root of the server");
	setUp.setOption(bindAddress_, "--bindAddress", "Address to bind to, defaults to 127.0.0.1 (localhost)");

	setUp.setOption(workingDir_, "--workingDir", "The working directory to store temporary files");
	setUp.setOption(seqServerCore_, "-seqServerCore",
			"Name of the seqServerCore Directory where the js and html for the core seqServer code is located",
			!bfs::exists(seqServerCore_));
	verbose_ = setUp.pars_.verbose_;
	debug_ = setUp.pars_.debug_;
	seqServerCore_ = bib::appendAsNeededRet(seqServerCore_.string(), "/");
	if(!bib::beginsWith(name_, "/")){
		name_ = "/" + name_;
	}
}

void SeqAppCorePars::addCoreOpts(Json::Value & config){
	config["name"] =  bib::json::toJson(name_);
	config["bindAddress"] = bib::json::toJson(bindAddress_);
  config["port"] = bib::json::toJson(port_);
  config["workingDir"] = bib::json::toJson(workingDir_);
  config["seqServerCore"] = bib::json::toJson(seqServerCore_);
  config["verbose"] = bib::json::toJson(verbose_);
  config["debug"] = bib::json::toJson(debug_);
}


std::string SeqAppCorePars::getAddress() const {
	return bib::pasteAsStr(bindAddress_, ":", port_, name_);
}


}  // namespace bibseq
