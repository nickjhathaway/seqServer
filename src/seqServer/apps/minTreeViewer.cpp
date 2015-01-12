/*
 * minTreeViewer.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "minTreeViewer.hpp"

namespace bibseq {
int minTreeViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string dir = "";
	uint32_t port = 8882;
	std::string name = "mtv";
	setUp.setOption(dir, "-dir", "Name of the Master Result Directory", true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  try {
      cppcms::service app(config);
      app.applications_pool().mount(cppcms::applications_factory<mtv>(name, dir));
      app.run();
  } catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }
	return 0;
}

} /* namespace bibseq */
