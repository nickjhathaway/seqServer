/*
 * pearOutViewer.cpp
 *
 *  Created on: Dec 29, 2014
 *      Author: nickhathaway
 */

#include "pearOutViewer.hpp"

namespace bibseq {

int pearOutViwer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string dir = "";
	uint32_t port = 8881;
	std::string name = "pov";
	setUp.setOption(dir, "-dir", "Name of the Master Result Directory", true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  //
  std::map<std::string, std::string> appConfig;
  appConfig["name"] = name;
  appConfig["dir"] = dir;
  appConfig["js"] = "../resources/js/";
  appConfig["css"] = "../resources/css/";
  auto pass = configTest(appConfig, pov::requiredOptions());
  if(!pass){
  	exit(1);
  }
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
