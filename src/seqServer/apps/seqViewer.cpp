/*
 * seqViewer.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "seqViewer.hpp"

namespace bibseq {

ssv::ssv(cppcms::service& srv, std::map<std::string, std::string> config)
: bibseq::seqApp(srv, config)
{
	bool pass = configTest(config, requiredOptions(), "ssv");
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
	dispMap_1arg(&ssv::sort,this, "sort", "(\\w+)");
	dispMap(&ssv::muscleAln,this, "muscle");
	dispMap(&ssv::complementSeqs,this, "complement");
	dispMap(&ssv::removeGaps,this, "removeGaps");
	mapper().root(rootName_);
	//read in data and set to the json
	readObjectIOOptions options(config["ioOptions"]);
	readObjectIO reader;
	reader.read(options);
	reads_ = reader.reads;
	readsJson_ = seqsToJson(reads_);
	needsUpdate_= false;
	std::cout << "Finished set up" << std::endl;

}

VecStr ssv::requiredOptions() const {
	return VecStr{"resources", "ioOptions"};
}

void ssv::seqData() {
	scopedMessage run("seqData", std::cout, debug_);
	ret_json();
	if(needsUpdate_){
		readsJson_ = seqsToJson(reads_);
	}
	response().out() << readsJson_;
}


void ssv::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}

void ssv::sort(std::string sortBy){
	scopedMessage run("sort", std::cout, debug_);
	std::cout << "sort; sortBy: " << sortBy << std::endl;
	readVecSorter::sortReadVector(reads_, sortBy);
	ret_json();
	readsJson_ = seqsToJson(reads_);
	response().out() << readsJson_;
}

void ssv::muscleAln(){
	scopedMessage run("muscleAln", std::cout, debug_);
	bib::for_each(reads_, [](readObject & read){ read.seqBase_.removeGaps();});
	sys::muscleSeqs(reads_);
	ret_json();
	readsJson_ = seqsToJson(reads_);
	response().out() << readsJson_;
}

void ssv::removeGaps(){
	scopedMessage run("removeGaps", std::cout, debug_);
	bib::for_each(reads_, [](readObject & read){ read.seqBase_.removeGaps();});
	ret_json();
	readsJson_ = seqsToJson(reads_);
	response().out() << readsJson_;
}

void ssv::complementSeqs(){
	scopedMessage run("complementSeqs", std::cout, debug_);
	readVec::allReverseComplement(reads_, true);
	ret_json();
	readsJson_ = seqsToJson(reads_);
	response().out() << readsJson_;
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
