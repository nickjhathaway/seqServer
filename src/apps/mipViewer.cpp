/*
 * mipViewer.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "mipViewer.hpp"

namespace bibseq {

int mipViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "ssv";
	setUp.setOption(clusDir, "-clusDir", "Name of the Master Result Directory", true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  //
  std::map<std::string, std::string> appConfig;
  appConfig["name"] = name;
  appConfig["clusDir"] = clusDir;
  appConfig["js"] = "../resources/js/";
  appConfig["css"] = "../resources/css/";
  auto pass = configTest(appConfig, VecStr{});
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
