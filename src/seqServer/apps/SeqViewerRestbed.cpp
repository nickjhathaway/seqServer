/*
 * SeqViewerRestbed.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "SeqViewerRestbed.hpp"

namespace bibseq {




int seqViewerRestbed(const bib::progutils::CmdArgs & inputCommands){
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "ssv";
	std::string resourceDirName = bib::files::make_path(seqServer_INSTALLDIR,
			"etc/resources").string();
	bool protein = false;
	bool svg = false;
	bibseq::seqSetUp setUp(inputCommands);
	setUp.setOption(protein, "--protein", "Viewing Protein");
	setUp.setOption(svg, "--svg", "Viewing svg viewer");
	setUp.setOption(resourceDirName, "-resourceDirName",
			"Name of the resource Directory where the js and hmtl is located",
			!bfs::exists(resourceDirName));
	bib::appendAsNeeded(resourceDirName, "/");
	setUp.processDefaultReader(true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.processDebug();
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  //
  Json::Value appConfig;
  appConfig["name"] =  bib::json::toJson(name);
  appConfig["port"] = bib::json::toJson( estd::to_string(port));
  auto optsJson = setUp.pars_.ioOptions_.toJson();
  appConfig["ioOptions"] =  bib::json::toJson(optsJson.toStyledString());
  appConfig["resources"] = bib::json::toJson(resourceDirName);
  appConfig["js"] = bib::json::toJson(resourceDirName + "js/");
  appConfig["css"] = bib::json::toJson(resourceDirName + "css/");
  appConfig["debug"] =  bib::json::toJson(setUp.pars_.debug_);
  appConfig["protein"] = bib::json::toJson(protein);
  appConfig["svg"] = bib::json::toJson(svg);


  std::cout << "localhost:"  << port << name << std::endl;

	SeqViewerRestbed viewerModel(appConfig);

	auto resources = viewerModel.getAllResources();

	auto settings = std::make_shared<restbed::Settings>();
	settings->set_port(port);
	//settings->set_root(name);
	settings->set_default_header("Connection", "close");

	restbed::Service service;
	for(const auto & resource : resources){
		service.publish(resource);
	}
	service.start(settings);
	return 0;
}


} /* namespace bibseq */
