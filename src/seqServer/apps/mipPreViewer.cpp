/*
 * mipPreViewer.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: nickhathaway
 */

#include "mipPreViewer.hpp"

namespace bibseq {

mpv::mpv(cppcms::service& srv, std::map<std::string, std::string> config)
: bibseq::seqApp(srv, config)
{
	configTest(config, requiredOptions(), "mpv");
	pages_.emplace("mainPageHtml",make_path(config["resources"] + "mpv/mainPage.html") );
	rootName_ = config["name"];
	for(auto & fCache : pages_){
		fCache.second.replaceStr("/ssv", rootName_);
	}

	//main page
	dispMapRoot(&mpv::mainPage, this);

	//general information
	dispMap(&mpv::rootName,this, "rootName");

	mapper().root(rootName_);
	//read in data and set to the json

	std::cout << "Finished set up" << std::endl;

}

VecStr mpv::requiredOptions() const {
	return VecStr{"twoBit", "infoFile"};
}




void mpv::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}



void mpv::mainPage() {
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}


int mipPreViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	uint32_t port = 8981;
	std::string name = "mpv";
	std::string resourceDirName = "";
	std::string twoBitFileName = "";
	std::string infoFileName = "";
	setUp.setOption(twoBitFileName, "-twoBitFileName", "Name of the two bit file to use", true);
	setUp.setOption(infoFileName, "-infoFileName", "Name of the info file to use", true);
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}

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
  appConfig["resources"] = resourceDirName;
  appConfig["twoBit"] = twoBitFileName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  std::cout << "localhost:"  << port << name << std::endl;
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<mpv>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

} /* namespace bibseq */
