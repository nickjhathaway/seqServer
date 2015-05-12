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
		bibseq::seqApp(srv, config), config_(config){
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
	pages_.emplace("extractionStats",
			make_path(config["resources"] + "pcvExp/extractionStats.html"));
	pages_.emplace("subGroupsPage",
			make_path(config["resources"] + "pcvExp/subGroupsPage.html"));
	pages_.emplace("groupMainPage",
			make_path(config["resources"] + "pcvExp/groupMainPage.html"));
	for (auto & fCache : pages_) {
		fCache.second.replaceStr("/ssv", rootName_);
	}
	std::cout << "ExtractionDir: " << config["extractionDir"] << std::endl;
	if(config["extractionDir"] != ""){
		std::string nameDelim = "_extractor_";
		auto dirs = getNewestDirs(config["extractionDir"], nameDelim);
		for(const auto & dir : dirs){
			std::cout << "Dir: " << dir << std::endl;
		}
		extractInfo_ = collectExtractionInfo(config["extractionDir"],config["indexToDir"], config["sampNames"]);
	}



	//main page
	dispMapRoot(&pcvExp::mainPage, this);

	dispMap(&pcvExp::getProjectName, this, "projectName");

	dispMap(&pcvExp::getPopInfo, this, "popInfo");
	dispMap(&pcvExp::getPosSeqData, this, "popSeqData");
	dispMap(&pcvExp::getPopProtenData, this, "popProteinData");
	dispMap_1arg(&pcvExp::getSampInfo, this, "sampInfo", "(\\w+)");
	dispMap(&pcvExp::getSampleNames, this, "sampleNames");

	//group info
	dispMap(&pcvExp::getGroupNames, this, "getGroupNames");
	dispMap_2arg(&pcvExp::getGroupPopInfo, this, "groupPopInfo", "(\\w+)/(\\w+)");
	dispMap_2arg(&pcvExp::getGroupPopSeqData, this, "groupPopSeqData", "(\\w+)/(\\w+)");
	dispMap_2arg(&pcvExp::getGroupPopProtenData, this, "groupPopProteinData", "(\\w+)/(\\w+)");
	dispMap_3arg(&pcvExp::getGroupSampInfo, this, "groupSampInfo", "(\\w+)/(\\w+)/(\\w+)");
	dispMap_2arg(&pcvExp::getGroupSampleNames, this, "groupSampleNames", "(\\w+)/(\\w+)");

	dispMap_2arg(&pcvExp::showGroupMainPage, this, "showGroupMainPage", "(\\w+)/(\\w+)");
	dispMap_1arg(&pcvExp::showSubGroupsPage, this, "showSubGroupsPage", "(\\w+)");
	dispMap_1arg(&pcvExp::getSubGroupsForGroup, this, "getSubGroupsForGroup", "(\\w+)");
	dispMap_1arg(&pcvExp::getGroupPopInfos, this, "getGroupPopInfos", "(\\w+)");

	dispMap(&pcvExp::getProteinColors, this, "proteinColors");


	dispMap(&pcvExp::getAllSampleNames, this, "allSampleNames");

	dispMap(&pcvExp::getSampleNamesEncoding, this, "sampleNamesEncoding");
	dispMap(&pcvExp::getEncodingForSampleNames, this, "encodingForSampleNames");


	dispMap(&pcvExp::showMinTree, this, "showMinTree");
	dispMap(&pcvExp::getMinTreeData, this, "minTreeData");


	dispMap(&pcvExp::showExtractionInfo, this, "showExtractionInfo");
	dispMap(&pcvExp::getIndexExtractionInfo, this, "getIndexExtractionInfo");
	dispMap_1arg(&pcvExp::getSampleExtractionInfo, this, "getSampleExtractionInfo", "(\\w+)");


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
	clusteredSampleNames_ = getVectorOfMapKeys(sampCounts);
	//read pop table


	popTable_ = table(mainDir_ + "population/populationCluster.tab.txt", "\t", true);
	/*for(const auto & d : delFromPop){
		popTable_.deleteColumn(d);
	}*/
	popTable_.sortTable("h_popUID", false);
	auto names = popTable_.getColumn("h_popUID");
	auto proteins = popTable_.getColumn("h_Protein");
	for(auto pos : iter::range(names.size())){
		popReadsTranslated_.emplace_back(seqInfo(names[pos], proteins[pos]));
	}
	//read pop seqs
	auto files = bib::files::listAllFiles(mainDir_ + "population/", false, {std::regex{".*\\.fastq"}});
	readObjectIO reader;
	reader.read("fastq", files.begin()->first.string(), true);
	popReads_ = reader.reads;
	for(auto & read : popReads_){
		read.seqBase_.name_ = read.seqBase_.name_.substr(0, read.seqBase_.name_.rfind("_"));
	}
	std::unordered_map<std::string, std::vector<uint32_t>> proteinCheck;
	for(const auto & pEnum : iter::enumerate(popReadsTranslated_)){
		proteinCheck[pEnum.element.seqBase_.seq_].emplace_back(pEnum.index);
	}
	/*for(const auto & pc : proteinCheck){
		std::cout << pc.first << " : " << vectorToString(pc.second, ",") << std::endl;
	}*/
	/**todo make safter for non fastq pop clustering */

	//encode sample names
	std::set<std::string> allUniSampNames(clusteredSampleNames_.begin(), clusteredSampleNames_.end());
	if(!extractInfo_.profileBySampTab_.content_.empty()){
		for(const auto & samp : extractInfo_.profileBySampTab_.getColumn("Sample")){
			allUniSampNames.emplace(samp);
		}
	}
	allSampleNames_ = std::vector<std::string>(allUniSampNames.begin(), allUniSampNames.end());
	for(const auto & pos : iter::range(allSampleNames_.size())){
		codedNumToSampName_[pos] = allSampleNames_[pos];
		sampNameToCodedNum_[allSampleNames_[pos]] = pos;
	}

	if(bfs::exists(mainDir_ + "/groups")){
		auto sampFiles = bib::files::listAllFiles(mainDir_ + "/groups", true, VecStr{"sampFile.tab.txt"});
		for(const auto & sampF : sampFiles){
			auto toks = tokenizeString(sampF.first.string(), "/");
			groupInfosDirNames_[toks[toks.size() - 3]][toks[toks.size() - 2]] = sampF.first.parent_path().string();
		}
	}

	std::cout << "Finished set up" << std::endl;
}

void pcvExp::getGroupNames(){
	ret_json();
	cppcms::json::value ret = getVectorOfMapKeys(groupInfosDirNames_);
	response().out() << ret;
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
	cppcms::json::value ret = clusteredSampleNames_;
	response().out() << ret;
}

void pcvExp::getAllSampleNames(){
	ret_json();
	cppcms::json::value ret = allSampleNames_;
	response().out() << ret;
}

void pcvExp::getSampleNamesEncoding(){
	ret_json();
	cppcms::json::value ret;
	for(const auto & sampName : sampNameToCodedNum_){
		ret[sampName.first] = sampName.second;
	}
	response().out() << ret;
}

void pcvExp::getEncodingForSampleNames(){
	ret_json();
	cppcms::json::value ret;
	for(const auto & sampName : sampNameToCodedNum_){
		ret[sampName.second] = sampName.first;
	}
	response().out() << ret;
}

void pcvExp::getSampInfo(std::string sampNames){
	ret_json();
	cppcms::json::value ret;

	auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
	auto sampNameToCodedNum = sampNameToCodedNum_;
	auto containsSampName = [&sampToks, &sampNameToCodedNum](const std::string & str) {
		return bib::in(estd::to_string(sampNameToCodedNum[str]), sampToks);
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

std::string pcvExp::decodeSampEncoding(const std::string& sampName){
	return codedNumToSampName_[bib::lexical_cast<uint32_t>(sampName)];
}

void pcvExp::individualSamplePage(std::string sampName){
	sampName = decodeSampEncoding(sampName);
	bib::scopedMessage mess(bib::err::F() << "individualSamplePage; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, clusteredSampleNames_)){
		auto search = pages_.find("individualSample");
		response().out() << search->second.get("/ssv", rootName_);
	}else{
		auto search = pages_.find("redirectPage");
		std::cout << "SampName: " << sampName << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}
}

void pcvExp::getSeqData(std::string sampName){
	sampName = decodeSampEncoding(sampName);
	bib::scopedMessage mess(bib::err::F()<< "getSeqData; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, clusteredSampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "final/" + sampName + ".fastq";
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
	sampName = decodeSampEncoding(sampName);
  bool forceStartM = false;
  bool transcribeToRNAFirst = false;
  uint64_t start = 0;
	bib::scopedMessage mess(bib::err::F()<< "getProteinData; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, clusteredSampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "final/" + sampName + ".fastq";
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
	sampName = decodeSampEncoding(sampName);
	bib::scopedMessage mess(bib::err::F()<< "getMinTreeDataForSample; " << "sampName: " << sampName, std::cout, true);
	if(bib::in(sampName, clusteredSampleNames_)){
		auto fileName = bib::files::appendAsNeededRet(mainDir_, "/") + "final/" + sampName + ".fastq";
		if(fexists(fileName)){
			auto search = sampleMinTreeDataCache_.find(sampName);
			Json::Value graphJsonData;
			if(search == sampleMinTreeDataCache_.end()){
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
				graphJsonData = graphMis.toJsonMismatchGraphAll(bib::color("#000000"), nameColors);
				sampleMinTreeDataCache_[sampName] = graphJsonData;
			}else{
				graphJsonData = search->second;
			}
			response().out() << graphJsonData;
		}else{
			std::cout << "File " << fileName << " does not exist" << std::endl;
		}
	}else{
		std::cout << "SampName: " << sampName << " not found" << std::endl;
	}
}
//
void pcvExp::showMinTreeForSample(std::string sampName){
	std::string encodedName = sampName;
	sampName = decodeSampEncoding(sampName);
	std::cout << "showMinTreeForSample; " << "sampName: " << sampName << std::endl;
	if(bib::in(sampName, clusteredSampleNames_)){
		response().out() << genHtmlStrForPsuedoMintree(rootName_ + "/minTreeDataForSample/" + encodedName);
	}else{
		auto search = pages_.find("redirectPage");
		std::cout << "SampName: " << sampName << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}
}


void pcvExp::showExtractionInfo(){
	bib::scopedMessage mess(bib::err::F()<< "showExtractionInfo", std::cout, true);
	auto search = pages_.find("extractionStats");
	response().out() << search->second.get("/ssv", rootName_);
}

void pcvExp::getIndexExtractionInfo(){
	bib::scopedMessage mess(bib::err::F()<< "getIndexExtractionInfo", std::cout, true);
	if(extractInfo_.allStatsTab_.content_.empty()){
		std::cerr << "getIndexExtractionInfo: Extraction Info not loaded" << std::endl;
	}else{
		ret_json();
		auto statsCopy = extractInfo_.allStatsTab_;
		statsCopy.trimElementsAtFirstOccurenceOf("(");
		auto ret = tableToJsonRowWise(statsCopy);
		response().out() << ret;
	}
}

void pcvExp::getSampleExtractionInfo(std::string sampNames){
	bib::scopedMessage mess(bib::err::F()<< "getSampleExtractionInfo", std::cout, true);
	if(extractInfo_.allStatsTab_.content_.empty()){
		std::cerr << "getSampleExtractionInfo: Extraction Info not loaded" << std::endl;
	}else{
		ret_json();
		auto sampTabCopy = extractInfo_.profileBySampTab_;
		sampTabCopy.trimElementsAtFirstOccurenceOf("(");

		auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
		printVector(sampToks);
		auto sampNameToCodedNum = sampNameToCodedNum_;
		auto containsSampName = [&sampToks, &sampNameToCodedNum](const std::string & str) {
			return bib::in(estd::to_string(sampNameToCodedNum[str]), sampToks);
		};

		sampTabCopy = sampTabCopy.extractByComp("Sample", containsSampName);
		//sampTabCopy.outPutContentOrganized(std::cout);

		for(auto & row : sampTabCopy.content_){
			row[sampTabCopy.getColPos("Sample")] = row[sampTabCopy.getColPos("Sample")] + "_" +  row[sampTabCopy.getColPos("MidName")];
		}
		auto ret = tableToJsonRowWise(sampTabCopy);
		response().out() << ret;
	}
}

void pcvExp::getGroupPopInfo(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "getGroupPopInfo", std::cout, true);
	if(setUpGroup(group, subGroup)){
		ret_json();
		auto ret = tableToJsonRowWise(groupInfos_[group][subGroup].popTable_);
		response().out() << ret;
	}else{
		//auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << ":" << subGroup<< " not found, redirecting" << std::endl;
		//response().out() << search->second.get("/ssv", rootName_);
	}
}
void pcvExp::getGroupPopSeqData(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "getGroupPopSeqData", std::cout, true);
	if(setUpGroup(group, subGroup)){
		ret_json();
		auto ret = seqsToJson(groupInfos_[group][subGroup].popReads_);
		response().out() << ret;
	}else{
		//auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << ":" << subGroup<< " not found, redirecting" << std::endl;
		//response().out() << search->second.get("/ssv", rootName_);
	}
}
void pcvExp::getGroupPopProtenData(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "getGroupPopProtenData", std::cout, true);
	if(setUpGroup(group, subGroup)){
		ret_json();
		auto ret = seqsToJson(groupInfos_[group][subGroup].popReadsTranslated_);
		response().out() << ret;
	}else{
		//auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << ":" << subGroup<< " not found, redirecting" << std::endl;
		//response().out() << search->second.get("/ssv", rootName_);
	}

}
void pcvExp::getGroupSampInfo(std::string group, std::string subGroup, std::string sampNames){
	bib::scopedMessage mess(bib::err::F()<< "getGroupSampInfo", std::cout, true);
	if(setUpGroup(group, subGroup)){
		ret_json();
		cppcms::json::value ret;

		auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
		auto sampNameToCodedNum = sampNameToCodedNum_;
		auto containsSampName = [&sampToks, &sampNameToCodedNum](const std::string & str) {
			return bib::in(estd::to_string(sampNameToCodedNum[str]), sampToks);
		};
		auto trimedTab = groupInfos_[group][subGroup].sampTable_.extractByComp("s_Name", containsSampName);
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
	}else{
		//auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << ":" << subGroup<< " not found, redirecting" << std::endl;
		//response().out() << search->second.get("/ssv", rootName_);
	}

}

void pcvExp::getGroupSampleNames(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "getGroupSampleNames", std::cout, true);
	if(setUpGroup(group, subGroup)){
		ret_json();
		cppcms::json::value ret = groupInfos_[group][subGroup].clusteredSampleNames_;
		response().out() << ret;
	}else{
		//auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << ":" << subGroup<< " not found, redirecting" << std::endl;
		//response().out() << search->second.get("/ssv", rootName_);
	}
}
void pcvExp::getSubGroupsForGroup(std::string group){
	bib::scopedMessage mess(bib::err::F()<< "getSubGroupsForGroup", std::cout, true);
	std::cout << "getSubGroupsForGroup: GroupName: " << group  << "\n";
	auto search = groupInfosDirNames_.find(group);
	if(search == groupInfosDirNames_.end()){
		std::cout << "No group: " << group << "\n";
		std::cout << "options: " << vectorToString(getVectorOfMapKeys(groupInfosDirNames_), ", ") << "\n";
		std::cout << "group: " << group << " not found" << std::endl;
	}else{
		auto keys = getVectorOfMapKeys(search->second);
		ret_json();
		cppcms::json::value ret = keys;
		response().out() << ret;
	}
}
void pcvExp::showGroupMainPage(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "showGroupMainPage", std::cout, true);
	std::cout << "showGroupMainPage: GroupName: " << group << " subGroup: " << subGroup << "\n";
	auto search = groupInfosDirNames_.find(group);
	if(search == groupInfosDirNames_.end()){
		std::cout << "No group: " << group << "\n";
		std::cout << "options: " << vectorToString(getVectorOfMapKeys(groupInfosDirNames_), ", ") << "\n";
		auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}else{
		auto subSearch = search->second.find(subGroup);
		if(subSearch == search->second.end()){
			std::cout << "No subgroup: " << subGroup << " in group: " << group << "\n";
			std::cout << "options: " << vectorToString(getVectorOfMapKeys(search->second), ", ") << "\n";
			auto search = pages_.find("redirectPage");
			std::cout << "group: " << group << " not found, redirecting" << std::endl;
			response().out() << search->second.get("/ssv", rootName_);
		}else{
			auto search = pages_.find("groupMainPage");
			response().out() << search->second.get("/ssv", rootName_);
		}
	}
}
void pcvExp::showSubGroupsPage(std::string group){
	bib::scopedMessage mess(bib::err::F()<< "showSubGroupsPage", std::cout, true);
	std::cout << "showSubGroupsPage: GroupName: " << group << "\n";
	auto search = groupInfosDirNames_.find(group);
	if(search == groupInfosDirNames_.end()){
		std::cout << "No group: " << group << "\n";
		std::cout << "options: " << vectorToString(getVectorOfMapKeys(groupInfosDirNames_), ", ") << "\n";
		auto search = pages_.find("redirectPage");
		std::cout << "group: " << group << " not found, redirecting" << std::endl;
		response().out() << search->second.get("/ssv", rootName_);
	}else{
		auto search = pages_.find("subGroupsPage");
		response().out() << search->second.get("/ssv", rootName_);
	}
}

void pcvExp::getGroupPopInfos(std::string group){
	bib::scopedMessage mess(bib::err::F()<< "getGroupPopInfos", std::cout, true);
	std::cout << "getGroupPopInfos: GroupName: " << group << "\n";
	auto search = groupInfosDirNames_.find(group);
	cppcms::json::value ret;
	ret_json();
	if(search == groupInfosDirNames_.end()){
		std::cout << "No group: " << group << "\n";
		std::cout << "options: " << vectorToString(getVectorOfMapKeys(groupInfosDirNames_), ", ") << "\n";
		std::cout << "group: " << group << " not found, redirecting" << std::endl;
	}else{
		auto keys = getVectorOfMapKeys(search->second);
		for(const auto & k : keys){
			setUpGroup(group, k);
		}
		VecStr groupInfoColNames { "g_GroupName", "p_TotalInputReadCnt",
				"p_TotalInputClusterCnt", "p_TotalPopulationSampCnt",
				"p_TotalUniqueHaplotypes", "p_meanMoi", "p_medianMoi", "p_minMoi",
				"p_maxMoi" };
		table outTab(groupInfoColNames);
		for(const auto & k : keys){
			outTab.rbind(groupInfos_[group][k].popTable_.getColumns(groupInfoColNames));
		}
		outTab = outTab.getUniqueRows();
		outTab.sortTable("g_GroupName", false);
		ret = tableToJsonRowWise(outTab);
	}
	response().out() << ret;
}

bool pcvExp::setUpGroup(std::string group, std::string subGroup){
	bib::scopedMessage mess(bib::err::F()<< "setUpGroup", std::cout, true);
	std::cout << "setUpGroup: GroupName: " << group << " subGroup: " << subGroup << "\n";
	auto search = groupInfosDirNames_.find(group);
	if(search == groupInfosDirNames_.end()){
		std::cout << "No group: " << group << "\n";
		std::cout << "options: " << vectorToString(getVectorOfMapKeys(groupInfosDirNames_), ", ") << "\n";
		return false;
	}else{
		auto subSearch = search->second.find(subGroup);
		if(subSearch == search->second.end()){
			std::cout << "No subgroup: " << subGroup << " in group: " << group << "\n";
			std::cout << "options: " << vectorToString(getVectorOfMapKeys(search->second), ", ") << "\n";
			return false;
		}else{
			if(groupInfos_[group].find(subGroup) == groupInfos_[group].end()){
				/**@todo should check to see if file exists*/
				table sampTab = table(subSearch->second + "/sampFile.tab.txt", "\t", true);
				table popTab = table(subSearch->second + "/popFile.tab.txt", "\t", true);
				auto popNames = popTab.getColumn("h_popUID");
				std::vector<readObject> popReads;
				std::vector<readObject> popTranslatedReads;
				for(const auto & read : popReads_){
					if(bib::in(read.seqBase_.name_, popNames)){
						popReads.emplace_back(read);
					}
				}
				for(const auto & read : popReadsTranslated_){
					if(bib::in(read.seqBase_.name_, popNames)){
						popTranslatedReads.emplace_back(read);
					}
				}
				groupInfos_[group][subGroup] = popInfo(sampTab, popTab, popReads, popTranslatedReads);
			}
			return true;
		}
	}
}


int popClusteringViewerExp(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string mainDir = "";
	std::string extractioinDir = "";
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
	setUp.setOption(name, "-name", "Name of root of the server");
	std::string extractionDir = "";
	std::string indexToDir = "";
	std::string sampNames = "";

	setUp.setOption(indexToDir, "-indexToDir", "File, first column is index name, second is the name of the file extraction was done on");
	setUp.setOption(extractionDir, "-extractionDir", "Name of the directory where extraction was done");
	setUp.setOption(sampNames, "-sampNames", "A file, first column is index name, second is sample name, the rest of the columns are MID names");

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
  appConfig["extractionDir"] = extractionDir;
  appConfig["indexToDir"] = indexToDir;
  appConfig["sampNames"] = sampNames;
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
