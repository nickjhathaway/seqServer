/*
 * popClusterViewer.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: nickhathaway
 */

#include "popClusterViewerExp.hpp"

namespace bibseq {
template<typename T>
uint32_t getMismatches(const T & read1,
				const T & read2,
				aligner alignerObj, bool weightHomopolymers){
	alignerObj.alignVec(read1.seqBase_,read2.seqBase_, false);
	alignerObj.profilePrimerAlignment(read1.seqBase_, read2.seqBase_, weightHomopolymers);
	return alignerObj.comp_.hqMismatches_;
};
pcvExp::pcvExp(cppcms::service& srv, std::map<std::string, std::string> config) :
		bibseq::seqApp(srv, config){
	bool pass = configTest(config, requiredOptions(), "pcvExp");
	if(pass){
		std::cout << "Passed config test" << std::endl;
	}else{
		std::cout << "Didn't pass config test " << std::endl;
	}
	rootName_ = config["name"];
	mainDir_ = config["mainDir"];
	projectName_ = config["projectName"];
	pages_.emplace("mainPageHtml",
			make_path(config["resources"] + "pcvExp/mainPage.html"));
	pages_.emplace("redirectPage",
			make_path(config["resources"] + "html/redirectPage.html"));
	pages_.emplace("individualSample",
			make_path(config["resources"] + "pcvExp/individualSample.html"));
	for (auto & fCache : pages_) {
		fCache.second.replaceStr("/ssv", rootName_);
	}
	//main page
	dispMapRoot(&pcvExp::mainPage, this);

	dispMap(&pcvExp::getProjectName, this, "projectName");
	dispMap(&pcvExp::getPopInfo, this, "popInfo");
	dispMap(&pcvExp::getPosSeqData, this, "popSeqData");
	dispMap(&pcvExp::getPopProtenData, this, "popProteinData");
	dispMap(&pcvExp::getProteinColors, this, "proteinColors");

	dispMap_1arg(&pcvExp::getSampInfo, this, "sampInfo", "(\\w+)");
	dispMap(&pcvExp::getSampleNames, this, "sampleNames");

	dispMap(&pcvExp::showMinTree, this, "showMinTree");
	dispMap(&pcvExp::getMinTreeData, this, "minTreeData");

	dispMap_1arg(&pcvExp::individualSamplePage, this, "individualSamplePage", "(\\w+)");
	dispMap_1arg(&pcvExp::getSeqData, this, "seqData", "(\\w+)");
	dispMap_1arg(&pcvExp::getProteinData, this, "proteinData", "(\\w+)");
	dispMap_1arg(&pcvExp::showMinTreeForSample, this, "showMinTreeForSample", "(\\w+)");
	dispMap_1arg(&pcvExp::getMinTreeDataForSample, this, "minTreeDataForSample", "(\\w+)");


	mapper().root(rootName_);

	//read samp table
	/*VecStr delFromPop { "popInputClusterCnt", "hapPopFrac", "hapSumSampPopFrac",
			"hapMeanSampFrac", "hapClusterCNT" };
	VecStr delFromSamp = catenateVectors(delFromPop, VecStr { "popReadCntTot",
			"hapReadFrac", "hapConsesus", "cConsensus", "sInputCluster", "cReadFrac",
			"ReadCnt", "cChiReadCnt", "cChiClusCnt", "cChiRepCnt", "cInputNames",
			"R1totalCntExcluded", "R1totalFracExcluded", "R1.ClusCnt" });*/
	sampTable_ = table(mainDir_ + "selectedClustersInfo.tab.txt", "\t", true);
	/*for(const auto & d : delFromSamp){
		sampTable_.deleteColumn(d);
	}*/
	//get samp names
	auto sampCounts = countVec(sampTable_.getColumn("s_Name"));
	sampleNames_ = getVectorOfMapKeys(sampCounts);
	//read pop table


	popTable_ = table(mainDir_ + "population/populationCluster.tab.txt", "\t", true);
	/*for(const auto & d : delFromPop){
		popTable_.deleteColumn(d);
	}*/
	popTable_.sortTable("popUID", false);
	auto names = popTable_.getColumn("popUID");
	auto proteins = popTable_.getColumn("protein");
	for(auto pos : iter::range(names.size())){
		popReadsTranslated_.emplace_back(seqInfo(names[pos], proteins[pos]));
	}
	//read pop seqs
	auto files = bib::files::listAllFiles(mainDir_ + "population/", false, {std::regex{".*\\.fastq"}});
	readObjectIO reader;
	reader.read("fastq", files.begin()->first.string(), true);
	popReads_ = reader.reads;
	for(auto & read : popReads_){
		read.seqBase_.name_ = read.seqBase_.name_.substr(0, read.seqBase_.name_.find("_"));
	}
	std::unordered_map<std::string, std::vector<uint32_t>> proteinCheck;
	for(const auto & pEnum : iter::enumerate(popReadsTranslated_)){
		proteinCheck[pEnum.element.seqBase_.seq_].emplace_back(pEnum.index);
	}
	for(const auto & pc : proteinCheck){
		std::cout << pc.first << " : " << vectorToString(pc.second, ",") << std::endl;
	}
	/**todo make safter for non fastq pop clustering */



	std::cout << "Finished set up" << std::endl;
}
void pcvExp::mainPage(){
	auto search = pages_.find("mainPageHtml");
	response().out() << search->second.get("/ssv", rootName_);
}

//json
void pcvExp::getProjectName(){
	ret_json();
	cppcms::json::value ret = projectName_;
	response().out() << ret;
}

void pcvExp::getSampleNames(){
	ret_json();
	cppcms::json::value ret = sampleNames_;
	response().out() << ret;
}

void pcvExp::getSampInfo(std::string sampNames){
	ret_json();
	cppcms::json::value ret;
	auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
	auto containsSampName = [&sampToks](const std::string & str) {
		return bib::in(str, sampToks);
	};
	auto trimedTab = sampTable_.extractByComp("s_Name", containsSampName);
	ret = tableToJsonRowWise(trimedTab);
	auto popCounts = bibseq::countVec(trimedTab.getColumn("h_popUID"));
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

void pcvExp::getPosSeqData(){
	ret_json();
	auto ret = seqsToJson(popReads_);
	response().out() << ret;
}

void pcvExp::getPopInfo(){
	ret_json();
	auto ret = tableToJsonRowWise(popTable_);
	response().out() << ret;
}

void pcvExp::getProteinColors(){
	ret_json();
	cppcms::json::value ret;
  ret["A"] = "#14b814";
  ret["*"] = "#e6e6e6";
  ret["L"] = "#14c86e";
  ret["F"] = "#13d0a1";
  ret["C"] = "#13d8d8";
  ret["D"] = "#12ade0";
  ret["E"] = "#117de8";
  ret["G"] = "#134aef";
  ret["H"] = "#1919f0";
  ret["I"] = "#541ff2";
  ret["K"] = "#8c25f4";
  ret["M"] = "#c32bf5";
  ret["N"] = "#f631f6";
  ret["P"] = "#f838c8";
  ret["Q"] = "#f93e9c";
  ret["R"] = "#fa4572";
  ret["S"] = "#fb4b4b";
  ret["T"] = "#fc7c52";
  ret["V"] = "#fdab58";
  ret["W"] = "#fed65f";
  ret["Y"] = "#ffff66";
	response().out() << ret;
}

void pcvExp::getPopProtenData(){
	ret_json();
	auto ret = seqsToJson(popReadsTranslated_);
	response().out() << ret;
}

void pcvExp::getMinTreeData(){
	ret_json();
	if(!calculatedTreeData_){
		//get min tree data
		uint64_t maxLength = 0;
		readVec::getMaxLength(popReads_, maxLength);
		aligner alignerObj(maxLength, gapScoringParameters(5,1), substituteMatrix::createDegenScoreMatrix(2, -2));
	  std::function<uint32_t(const readObject & ,
	  		const readObject &, aligner, bool)> misFun = getMismatches<readObject>;
		auto misDistances = getDistanceCopy(popReads_, 2, misFun,
				alignerObj, true);
	  readDistGraph<uint32_t> graphMis(misDistances, popReads_);
		std::vector<std::string> popNames;
	  for(const auto & n : graphMis.nodes_){
	  	popNames.emplace_back(n->name_);
	  }
		auto popColors = bib::njhColors(popReads_.size());
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
		std::unordered_map<std::string,bib::color> nameColors;
		for(auto pos : iter::range(popNames.size())){
			nameColors[popNames[pos]] = popColorsStrs[pos];
		}
		minTreeData_ = graphMis.toJsonMismatchGraphAll(bib::color("#000000"), nameColors);
	}
	response().out() << minTreeData_;
}

void pcvExp::showMinTree(){
	response().out() << genHtmlStrForPsuedoMintree(rootName_ + "/minTreeData");
}

void pcvExp::individualSamplePage(std::string sampName){
	scopedMessage mess(bib::err::F() << "individualSamplePage; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, sampleNames_)){
		auto search = pages_.find("individualSample");
		response().out() << search->second.get("/ssv", rootName_);
	}else{
		auto search = pages_.find("redirectPage");
		std::cout << "SampName: " << sampName << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}
}

void pcvExp::getSeqData(std::string sampName){
	scopedMessage mess(bib::err::F()<< "getSeqData; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, sampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "originals/" + sampName + ".fastq";
		if(fexists(fileName)){
			ret_json();
			readObjectIO reader;
			reader.read("fastq", fileName, true);
			response().out() << seqsToJson(reader.reads);
		}else{
			std::cout << "File " << fileName << " does not exist" << std::endl;
		}
	}else{
		std::cout << "SampName: " << sampName << " not found" << std::endl;
	}
}

void pcvExp::getProteinData(std::string sampName){
  bool forceStartM = false;
  bool transcribeToRNAFirst = false;
  uint64_t start = 0;
	scopedMessage mess(bib::err::F()<< "getProteinData; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, sampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "originals/" + sampName + ".fastq";
		if(fexists(fileName)){
			ret_json();
			readObjectIO reader;
			reader.read("fastq", fileName, true);
			readVec::convertReadsToProteinFromcDNA(reader.reads, transcribeToRNAFirst,
			                                           start, forceStartM);
			response().out() << seqsToJson(reader.reads);
		}else{
			std::cout << "File " << fileName << " does not exist" << std::endl;
		}
	}else{
		std::cout << "SampName: " << sampName << " not found" << std::endl;
	}
}

void pcvExp::getMinTreeDataForSample(std::string sampName){
	scopedMessage mess(bib::err::F()<< "getMinTreeDataForSample; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, sampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "originals/" + sampName + ".fastq";
		if(fexists(fileName)){
			ret_json();
			readObjectIO reader;
			reader.read("fastq", fileName, true);
			uint64_t maxLength = 0;
			readVec::getMaxLength(reader.reads, maxLength);
			aligner alignerObj(maxLength, gapScoringParameters(5,1), substituteMatrix::createDegenScoreMatrix(2, -2));
		  std::function<uint32_t(const readObject & ,
		  		const readObject &, aligner, bool)> misFun = getMismatches<readObject>;
			auto misDistances = getDistanceCopy(reader.reads, 2, misFun,
					alignerObj, true);
		  readDistGraph<uint32_t> graphMis(misDistances, reader.reads);
			std::vector<std::string> popNames;
		  for(const auto & n : graphMis.nodes_){
		  	popNames.emplace_back(n->name_);
		  }
			auto popColors = bib::njhColors(reader.reads.size());
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
			std::unordered_map<std::string,bib::color> nameColors;
			for(auto pos : iter::range(popNames.size())){
				nameColors[popNames[pos]] = popColorsStrs[pos];
			}
			response().out() << graphMis.toJsonMismatchGraphAll(bib::color("#000000"), nameColors);
		}else{
			std::cout << "File " << fileName << " does not exist" << std::endl;
		}
	}else{
		std::cout << "SampName: " << sampName << " not found" << std::endl;
	}
}
//
void pcvExp::showMinTreeForSample(std::string sampName){
	std::cout << "showMinTreeForSample; " << "sampName: " << sampName << std::endl;
	if(bib::in(sampName, sampleNames_)){
		response().out() << genHtmlStrForPsuedoMintree(rootName_ + "/minTreeDataForSample/" + sampName);
	}else{
		auto search = pages_.find("redirectPage");
		std::cout << "SampName: " << sampName << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}
}

int popClusteringViewerExp(std::map<std::string, std::string> inputCommands){
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
  appConfig["mainDir"] = mainDir;
  appConfig["resources"] = resourceDirName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  appConfig["projectName"] = projectName;
  std::cout << "localhost:"  << port << name << std::endl;
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<pcvExp>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
} /* namespace bibseq */
