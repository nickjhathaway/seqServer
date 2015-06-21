/*
 * tableViewer.cpp
 *
 *  Created on: Jun 18, 2015
 *      Author: nickhathaway
 */

#include "viewTable.hpp"

namespace bibseq {

tableViewer::tableViewer(cppcms::service& srv, std::map<std::string, std::string> config)
: bibseq::seqApp(srv, config)
{
	configTest(config, requiredOptions(), "tableViewer");
	pages_.emplace("mainPageHtml",make_path(config["resources"] + "tv/mainPage.html") );
	rootName_ = config["name"];
	for(auto & fCache : pages_){
		fCache.second.replaceStr("/ssv", rootName_);
	}

	originalTable_ = table(config["tableName"],config["delim"], config["header"] =="true");
	updatedTable_ = originalTable_;
	//main page
	dispMapRoot(&tableViewer::mainPage, this);
	//table data
	dispMap(&tableViewer::tableData, this, "tableData");
	//updated table data
	dispMap(&tableViewer::updatedTableData, this, "updatedTableData");

	mapper().root(rootName_);

	std::cout << "Finished set up" << std::endl;

}

VecStr tableViewer::requiredOptions() const {
	return VecStr{"resources", "tableName", "delim", "header"};
}

void tableViewer::tableData(){
	ret_json();
	response().out() << tableToJsonRowWise(originalTable_, originalTable_.columnNames_[0], VecStr{}, VecStr{});
}

void tableViewer::updatedTableData(){
	ret_json();
	response().out() << tableToJsonRowWise(updatedTable_, updatedTable_.columnNames_[0], VecStr{}, VecStr{});
}

void tableViewer::updateTable(){

}


void tableViewer::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}




void tableViewer::mainPage() {
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}


int tableViewerMain(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	std::string tableName = "";
	std::string delim = "";
	bool header = false;

	uint32_t port = 8881;
	std::string name = "tv";
	std::string resourceDirName = "";
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}
	setUp.setOption(delim , "-delim", "The delimiter of the Input Table");
	setUp.setOption(header, "-header", "Whether the table has a header or not");
	setUp.setOption(tableName, "-table", "Input Table");
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
				cppcms::applications_factory<tableViewer>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}




} /* namespace bibseq */
