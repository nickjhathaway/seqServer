/*
 * SeqAppCorePars.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: nick
 */

#include "SeqAppCorePars.hpp"


namespace bibseq {

void SeqAppCorePars::setCoreOptions(seqSetUp & setUp){
	setUp.setOption(port_, "--port", "Port Number to Serve On");
	setUp.setOption(name_, "--name", "Name of root of the server");
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
  config["port"] = bib::json::toJson(port_);
  config["workingDir"] = bib::json::toJson(workingDir_);
  config["seqServerCore"] = bib::json::toJson(seqServerCore_);
  config["verbose"] = bib::json::toJson(verbose_);
  config["debug"] = bib::json::toJson(debug_);
}


std::string SeqAppCorePars::getAddress() const {
	return bib::pasteAsStr("localhost:", port_, name_);
}


}  // namespace bibseq
