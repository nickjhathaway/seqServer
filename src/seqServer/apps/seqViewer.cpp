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
/*
 * seqViewer.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "seqViewer.hpp"
#include "seqServer/objects/seqCache.hpp"

namespace bibseq {

ssv::ssv(cppcms::service& srv, std::map<std::string, std::string> config)
: bibseq::seqApp(srv, config)
{
	configTest(config, requiredOptions(), "ssv");
	pages_.emplace("mainPageHtml",make_path(config["resources"] + "ssv/mainPage.html") );
	rootName_ = config["name"];
	for(auto & fCache : pages_){
		fCache.second.replaceStr("/ssv", rootName_);
	}

	//main page
	dispMapRoot(&ssv::mainPage, this);

	dispMap(&ssv::seqData,this, "seqData");

	//general information
	dispMap(&ssv::rootName,this, "rootName");

	mapper().root(rootName_);
	//read in data and set to the json
	readObjectIOOptions options(config["ioOptions"]);
	readObjectIO reader;
	reader.read(options);
	seqs_.addToCache(rootName_.substr(1), std::make_shared<std::vector<readObject>>(reader.reads));
	std::cout << "Finished set up" << std::endl;

}

VecStr ssv::requiredOptions() const {
	return VecStr{"resources", "ioOptions"};
}

void ssv::seqData() {
	bib::scopedMessage run("seqData", std::cout, debug_);
	printVector(getVectorOfMapKeys(seqs_.cache_));
	ret_json();
	response().out() << seqs_.getJson(rootName_.substr(1));
}


void ssv::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}




void ssv::showMinTree() {

}

void ssv::minTreeData() {

}

void ssv::mainPage() {
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}


int seqViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "ssv";
	std::string resourceDirName = "";
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}
	setUp.processDefaultReader(true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  //
  std::map<std::string, std::string> appConfig;
  appConfig["name"] = name;
  appConfig["port"] = estd::to_string(port);
  auto optsJson = setUp.ioOptions_.toJson();
  appConfig["ioOptions"] = optsJson.toStyledString();
  appConfig["resources"] = resourceDirName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  std::cout << "localhost:"  << port << name << std::endl;
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<ssv>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}



} /* namespace bibseq */
