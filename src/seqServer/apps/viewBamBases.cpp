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
 * bamBaseViewer.cpp
 *
 *  Created on: Jun 18, 2015
 *      Author: nickhathaway
 */

#include "viewBamBases.hpp"

namespace bibseq {

bamBaseViewer::bamBaseViewer(cppcms::service& srv, std::map<std::string, std::string> config)
: bibseq::seqApp(srv, config)
{
	configTest(config, requiredOptions(), "bamBaseViewer");
	pages_.emplace("mainPageHtml",bib::files::make_path(config["resources"], "bbv/mainPage.html") );
	rootName_ = config["name"];
	for(auto & fCache : pages_){
		fCache.second.replaceStr("/ssv", rootName_);
	}
	filename_ = config["tableName"];
	originalTable_ = table(config["tableName"],config["delim"], config["header"] == "true");
	//main page
	dispMapRoot(&bamBaseViewer::mainPage, this);
	//table data
	dispMap(&bamBaseViewer::tableData, this, "tableData");
	//updated table data

	mapper().root(rootName_);

	std::cout << "Finished set up" << std::endl;

}


VecStr bamBaseViewer::requiredOptions() const {
	return VecStr{"resources", "tableName", "delim", "header"};
}



void bamBaseViewer::tableData(){
	ret_json();
	response().out() << tableToJsonByRow(originalTable_, originalTable_.columnNames_[0]);
}




void bamBaseViewer::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}




void bamBaseViewer::mainPage() {
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}


int bamBaseViewerMain(const bib::progutils::CmdArgs & inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	std::string tableName = "";
	std::string delim = "\t";
	bool header = false;
	uint32_t port = 8981;
	std::string name = "tv";
	std::string resourceDirName = "";
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}
	setUp.setOption(delim , "-delim", "The delimiter of the Input Table");
	setUp.setOption(header, "-header", "Whether the table has a header or not");
	setUp.setOption(tableName, "-table", "Input Table", true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  //
  std::map<std::string, std::string> appConfig;
  appConfig["name"] = name;
  appConfig["port"] = estd::to_string(port);
  appConfig["resources"] = resourceDirName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  appConfig["tableName"] = tableName;
  appConfig["delim"] = delim;
  appConfig["header"] = convertBoolToString(header);
  std::cout << "localhost:"  << port << name << std::endl;
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<bamBaseViewer>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}



	return 0;
}




} /* namespace bibseq */
