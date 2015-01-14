/*
 * popClusterViewer.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: nickhathaway
 */

#include "popClusterViewer.hpp"

namespace bibseq {
pcv::pcv(cppcms::service& srv, std::map<std::string, std::string> config) :
		bibseq::seqApp(srv, config){
	bool pass = configTest(config, requiredOptions(), "pcv");
	if(pass){
		std::cout << "Passed config test" << std::endl;
	}else{
		std::cout << "Didn't pass config test " << std::endl;
	}
	rootName_ = config["name"];
	mainDir_ = config["mainDir"];
	projectName_ = config["projectName"];
	pages_.emplace("mainPageHtml",
			make_path(config["resources"] + "pcv/mainPage.html"));
	for (auto & fCache : pages_) {
		fCache.second.replaceStr("/ssv", rootName_);
	}
	//main page
	dispMapRoot(&pcv::mainPage, this);

	dispMap(&pcv::getProjectName, this, "projectName");
	dispMap(&pcv::getPopInfo, this, "popInfo");
	dispMap(&pcv::getPosSeqData, this, "popSeqData");
	dispMap_1arg(&pcv::getSampInfo, this, "sampInfo", "(\\w+)");
	dispMap(&pcv::getSampleNames, this, "sampleNames");

	mapper().root(rootName_);

	//read samp table
	sampTable_ = table(mainDir_ + "selectedClustersInfo.tab.txt", "\t", true);
	//get samp names
	auto sampCounts = countVec(sampTable_.getColumn("sName"));
	sampleNames_ = getVectorOfMapKeys(sampCounts);
	//read pop table
	popTable_ = table(mainDir_ + "population/populationCluster.tab.txt", "\t", true);
	//read pop seqs
	auto files = bib::files::listAllFiles(mainDir_ + "population/", false, {std::regex{".*\\.fastq"}});
	readObjectIO reader;
	reader.read("fastq", files.begin()->first.string(), true);
	popReads_ = reader.reads;
	/**todo make safter for non fastq pop clustering */
	std::cout << "Finished set up" << std::endl;
}
void pcv::mainPage(){
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}

//json
void pcv::getProjectName(){
	ret_json();
	cppcms::json::value ret = projectName_;
	response().out() << ret;
}

void pcv::getSampleNames(){
	ret_json();
	cppcms::json::value ret = sampleNames_;
	response().out() << ret;
}

void pcv::getSampInfo(std::string sampNames){
	ret_json();
	cppcms::json::value ret;
	auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
	auto containsSampName = [&sampToks](const std::string & str) {
		return bib::in(str, sampToks);
	};
	auto trimedTab = sampTable_.extractByComp("sName", containsSampName);
	ret = tableToJsonRowWise(trimedTab);
	auto popCounts = bibseq::countVec(trimedTab.getColumn("popUID"));
	auto popColors = bib::njhColors(popCounts.size());
	bibseq::VecStr popColorsStrs(popColors.size());
	uint32_t count = 0;
	uint32_t halfCount = 0;
	for(const auto & cPos : iter::range(popColors.size())) {
		uint32_t pos = 0;
		if(cPos %2 == 0) {
			pos = popColors.size()/2 + halfCount;
			++halfCount;
		} else {
			pos = count;
			++count;
		}
		popColorsStrs[cPos] = "#" + popColors[pos].hexStr_;
	}
	ret["popColors"] = popColorsStrs;
	response().out() << ret;
}

void pcv::getPosSeqData(){
	ret_json();
	auto ret = seqsToJson(popReads_);
	response().out() << ret;
}

void pcv::getPopInfo(){
	ret_json();
	auto ret = tableToJsonRowWise(popTable_);
	response().out() << ret;
}

int popClusteringViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string mainDir = "";
	uint32_t port = 9881;
	std::string name = "pcv";
	std::string resourceDirName = "";
	std::string projectName = "";
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}
	setUp.setOption(mainDir, "-mainDir", "Name of the Master Result Directory", true);
	if(mainDir.back() != '/'){
		mainDir.append("/");
	}
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.setOption(projectName, "-projectName", "Name of the Project", true);
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);
  //
  std::map<std::string, std::string> appConfig;
  appConfig["name"] = name;
  appConfig["dir"] = mainDir;
  appConfig["resources"] = resourceDirName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  appConfig["projectName"] = projectName;
  std::cout << "localhost:"  << port << name << std::endl;
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<pcv>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
} /* namespace bibseq */
