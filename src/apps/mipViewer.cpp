/*
 * mipViewer.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "mipViewer.hpp"

namespace bibseq {
uint64_t countFastqSeqs(const std::string & filename, bool processed, bool verbose){
	uint64_t ret = 0;
	if(bfs::exists(filename)){
		readObjectIO reader;
		readObject read;
		std::ifstream inFile(filename);
		while(reader.readNextFastqStream(inFile,reader.SangerQualOffset, read, processed)){
			ret += std::round(read.seqBase_.cnt_);
		}
	}else if(verbose){
		std::cout << "File: " << filename << "doesn't exist, returning 0" << std::endl;
	}
	return ret;
}

void appendSlash(std::string & dirName){
	if(dirName.back() != '/'){
		dirName.append("/");
	}
}

std::string appendSlashRet(std::string dirName){
	appendSlash(dirName);
	return dirName;
}



VecStr processMipExtractInfoFile(table info){
	//table ret{VecStr{"matchingExtArm", "UnmatachedExtArm", "FailedLigationArm",
		//"readsUsed","readsFailingBarcodeFiltering", "readsFailingMinlen", "readsFailingQuality"}};
	//trim off (%) so elements can be treated as numbers
	info.trimElementsAtFirstOccurenceOf("(");
	uint32_t unmatched = bib::lexical_cast<uint32_t>(info.content_.back()[1]);
	info.content_.erase(info.content_.end()-1);
	uint32_t readsUsed = getSumOfVecStr<uint32_t>(info.getColumn("readsUsed"));
	uint32_t readsFailingBarCodeFiltering = getSumOfVecStr<uint32_t>(info.getColumn("readsNotUsed"));
	uint32_t failedMinLen = getSumOfVecStr<uint32_t>(info.getColumnsStartWith("failedMinLen").getColumn(0));
	uint32_t failedQuality = getSumOfVecStr<uint32_t>(info.getColumnsStartWith("failed_q").getColumn(0));

	uint32_t failedLigationArm = getSumOfVecStr<uint32_t>(info.getColumn("failedLigationArm"));
	uint32_t readsMatchingExtArm = getSumOfVecStr<uint32_t>(info.getColumn("readNumber"));
	return toVecStr(readsMatchingExtArm, unmatched, failedLigationArm,
			readsUsed, readsFailingBarCodeFiltering, failedMinLen, failedQuality);
}

table getSampleStats(const std::string & dirName, bool verbose){
	auto sampDirs = bib::files::listAllFiles(dirName, false, VecStr{"samp"});
	table finalInfo{VecStr{"sampleName","totalInitial","assembled", "discarded", "unassembled",
		"matchingExtArm", "UnmatachedExtArm", "FailedLigationArm",
		"readsUsed","readsFailingBarcodeFiltering", "readsFailingMinlen", "readsFailingQuality"}};
	for(const auto & d : sampDirs){
		if(d.second){
			auto pathName = d.first.string();
			appendSlash(pathName);
			auto sName = d.first.filename().string();
			if(verbose){
				std::cout << bib::bashCT::bold << bib::bashCT::addColor(210) << "Processing:"
						<< bib::bashCT::addColor(105) << sName
						<< bib::bashCT::reset << std::endl;
			}
			//bib::scopedStopWatch watch(sName, true);
			auto assembled = countFastqSeqs(pathName + sName + ".assembled.fastq", false, verbose);
			auto discarded = countFastqSeqs(pathName + sName + ".discarded.fastq", false, verbose);
			auto unassembled = countFastqSeqs(pathName + sName + ".unassembled.forward.fastq", false, verbose);
			uint32_t total = assembled + discarded + unassembled;
			if(assembled > 0){
				auto extractionDirs = bib::files::listAllFiles(pathName, false, VecStr{sName + ".assembled_mip"});
				std::map<bfs::path, bool, std::greater<bfs::path>> eDirs;
				for(const auto & ed : extractionDirs){
					if(ed.second){
						eDirs.emplace(ed);
					}
				}
				if (!eDirs.empty()) {
					auto resultsDir = appendSlashRet(eDirs.begin()->first.string());
					table info { resultsDir + "info.txt", "\t", true };
					auto processedInfo = processMipExtractInfoFile(info);
					finalInfo.content_.emplace_back(
							catenateVectors(
									toVecStr(sName, total, assembled, discarded, unassembled),
									processedInfo));
				}else{
					std::cout << "Couldn't find latest analysis file for sample " << sName << std::endl;
				}
			}
		}
	}
	return finalInfo;
}

miv::miv(cppcms::service& srv, std::map<std::string, std::string> config)
	: bibseq::seqApp(srv, config)
	, mainPageHtml_(make_path(config["resources"] + "mip/mainPage.html"))
	, oneGeneInfoHtml_(make_path(config["resources"] + "mip/oneGeneView.html"))
	, oneSampAllMipInfoHtml_(make_path(config["resources"] + "mip/oneSampAllMipInfo.html"))
	, oneMipInfoHtml_(make_path(config["resources"] + "mip/oneMipInfo.html"))
	, allSampsInfoHtml_(make_path(config["resources"] + "mip/allSampsInfo.html"))
	, oneSampInfoHtml_(make_path(config["resources"] + "mip/oneSampInfo.html"))
	, minTreeViewHtml_(make_path(config["resources"] + "mip/minTreeView.html"))
	, initialSamplereadAmountStatsHtml_(make_path(config["resources"] + "mip/initialSamplereadAmountStats.html"))
	, redirectPageHtml_(make_path(config["resources"] + "html/redirectPage.html"))
	, rootName_(config["name"])
	, clusteringDir_(config["clusDir"])
	{
		mainPageHtml_.replaceStr("/ssv", rootName_);
		oneGeneInfoHtml_.replaceStr("/ssv", rootName_);
		oneSampAllMipInfoHtml_.replaceStr("/ssv", rootName_);
		oneMipInfoHtml_.replaceStr("/ssv", rootName_);
		allSampsInfoHtml_.replaceStr("/ssv", rootName_);
		oneSampInfoHtml_.replaceStr("/ssv", rootName_);
		minTreeViewHtml_.replaceStr("/ssv", rootName_);
		//popInfoHtml_.replaceStr("/ssv", rootName_);
		initialSamplereadAmountStatsHtml_.replaceStr("/ssv", rootName_);
		redirectPageHtml_.replaceStr("/ssv", rootName_);
		//main page
		dispMapRoot(&miv::mainPage, this);

		dispMap(&miv::showInitialReadStats, this, "showInitialReadStats");
		dispMap(&miv::getInitialReadStats, this,"getInitialReadStats");

		dispMap(&miv::geneNames, this, "geneNames");
		dispMap(&miv::getAllSampleNames, this,"allSampNames");
		//gene page
		dispMap_1arg(&miv::showGeneInfo,this, "geneInfo", "(\\w+)");
		dispMap_1arg(&miv::mipNames,this, "mipNames", "(\\w+)");
		//one samp all mips page
		dispMap_1arg(&miv::showOneSampAllMip,this, "oneSampAllMipInfo", "(\\w+)");
		dispMap_2arg(&miv::oneSampAllMipData,this, "oneSampAllMipData", "(\\w+)/(\\w+)");
		dispMap_1arg(&miv::sampMipNamesData,this, "sampMipNames", "(\\w+)");
		//show one mip target info and sample names
		dispMap_1arg(&miv::showMipInfo,this, "mipInfo", "(\\w+)");
		dispMap_1arg(&miv::mipSampleNames,this, "mipSampleNames", "(\\w+)");
		//show the data table with all sample information
		dispMap_1arg(&miv::showAllSampInfo,this, "allSamps", "(\\w+)");
		dispMap_2arg(&miv::allSampsInfoData,this, "allSampsInfo", "(\\w+)/(\\w+)");
		//show the mip target info for one sample
		dispMap_2arg(&miv::showOneSampleInfo,this, "oneSampInfo", "(\\w+)/(\\w+)");
		dispMap_2arg(&miv::oneSampInitSeqData,this, "oneSampInitSeqData", "(\\w+)/(\\w+)");
		dispMap_2arg(&miv::oneSampFinalSeqData,this, "oneSampFinalSeqData", "(\\w+)/(\\w+)");
		dispMap_2arg(&miv::oneSampTabData,this, "oneSampTabData", "(\\w+)/(\\w+)");
		dispMap_2arg(&miv::getBarcodeInfoPerSamplePerMip,this, "barcodeInfo", "(\\w+)/(\\w+)");
		//show the minimum spanning tree for one sample info
		dispMap_2arg(&miv::showMinTree,this, "showMinTree", "(\\w+)/(\\w+)");
		dispMap_2arg(&miv::minTreeData,this, "minTreeData", "(\\w+)/(\\w+)");
		//show the Population information for one mip target
		dispMap_1arg(&miv::popInfoData,this, "popInfo", "(\\w+)");
		dispMap_1arg(&miv::popSeqData,this, "popSeqData", "(\\w+)");

		//general information
		dispMap(&miv::rootName,this, "rootName");
		dispMap(&miv::colorsData,this, "baseColors");
		dispMap_1arg(&miv::getColors,this, "getColors", "(\\d+)");
		mapper().root(rootName_);
		appendSlash(clusteringDir_);
		std::regex analysisPat {".*Analysis\\b"};
		auto files = bib::files::listAllFiles(clusteringDir_, false, {analysisPat} );
		printOutMapContents(files, "\t", std::cout );
		bfs::path analysisFolder = "";

		if(files.empty()){
			std::cout << "Error, need a folder that ends with Analysis that contains the analysis folders"  << std::endl;
			exit(1);
		}else{
			analysisFolder = files.begin()->first;
		}
		std::regex mipFolderPat{".*_\\d+\\b"};
		std::cout << "analysis folder: " << analysisFolder << std::endl;
		auto mipFinalAnalysisFolders = bib::files::listAllFiles(analysisFolder.string(), false, {mipFolderPat});
		for(const auto & mip : mipFinalAnalysisFolders){
			if(mip.second){
				//load analysis
				if(bfs::is_directory(appendSlashRet(mip.first.string()) + "analysis")){
					mipAnalysisFolders_[mip.first.filename().string()] = bfs::path(appendSlashRet(mip.first.string()) + "analysis");
				}else{
					std::cerr << "Error, did not find a folder called analysis under "
							<< appendSlashRet(mip.first.string()) << " for mip target "
							<< mip.first.filename().string()<<std::endl;
					exit(1);
				}
				//load analysis without barcdoes
				if(bfs::is_directory(appendSlashRet(mip.first.string()) + "withoutBarcodes")){
					mipAnalysisWithOutBarcodesFolders_[mip.first.filename().string()] = bfs::path(appendSlashRet(mip.first.string()) + "withoutBarcodes");
				}else{
					std::cerr << "Error, did not find a folder called withoutBarcodes under "
							<< appendSlashRet(mip.first.string()) << " for mip target "
							<< mip.first.filename().string()<<std::endl;
					exit(1);
				}
			}
		}
		stats_ = getSampleStats(clusteringDir_, false);
		auto readsUsed = stats_.getColumn("readsUsed");
		auto sampNames = stats_.getColumn("sampleName");
		for(const auto & e : iter::enumerate(readsUsed) ){
			if(bib::lexical_cast<uint32_t>(e.element) > 0){
				std::string sampName = sampNames[e.index];
				auto extractionDirs = bib::files::listAllFiles(clusteringDir_ + sampName, false, VecStr{sampName + ".assembled_mip"});
				std::map<bfs::path, bool, std::greater<bfs::path>> eDirs;
				for(const auto & ed : extractionDirs){
					if(ed.second){
						eDirs.emplace(ed);
					}
				}
				if (!eDirs.empty()) {
					auto resultsDir = appendSlashRet(eDirs.begin()->first.string());
					sampAnalysisFolders_[sampName] = (eDirs.begin()->first);
					auto mipFolders = bib::files::listAllFiles(eDirs.begin()->first.string(), false, {mipFolderPat});
					for(const auto & m : mipFolders){
						if(m.second){
							sampleMipAnalysisFolders_[sampName][m.first.filename().string()] = m.first;
						}
					}
				}else{
					std::cout << "Couldn't find latest analysis file for sample " << sampName << std::endl;
				}
			}
		}
		for(const auto & mipAnalysis : mipAnalysisFolders_) {
			if(!bfs::exists(bfs::path(mipAnalysis.second.string() + "/selectedClustersInfo.tab.txt"))){
				continue;
			}
			auto tab = bibseq::table(mipAnalysis.second.string() + "/selectedClustersInfo.tab.txt", "\t", true );
			auto expNames = tab.getColumn("popUID");
			if(expNames.empty()) {
				continue;
			}
			//std::cout << bib::bashCT::red << "expnames " << std::endl;
			//std::cout << bib::conToStr(expNames) << std::endl;
			//std::cout << bib::bashCT::reset;
			auto expName = expNames.front().substr(0,expNames.front().find("_"));
			auto targetName = expNames.front().substr(0,expNames.front().find("."));
			tab.columnNames_.insert(tab.columnNames_.begin(), "geneName");
			tab.columnNames_.insert(tab.columnNames_.begin(), "mipName");
			for(auto & row : tab.content_) {
				row.insert(row.begin(), expName);
				row.insert(row.begin(), targetName);
			}
			auto split = tab.splitTableOnColumn("Sample");
			sampNamesForMip_[mipAnalysis.first] = getVectorOfMapKeys(split);
			//std::cout << bib::conToStr(tab.columnNames_, ",") << std::endl;
			for(const auto & s : split) {
				auto search = allInfoBySample_.find(s.first);
				if(search == allInfoBySample_.end()) {
					allInfoBySample_[s.first] = s.second;
				} else {
					allInfoBySample_[s.first].rbind(s.second);
				}
			}
		}
		std::cout << "Finished set up" << std::endl;
	}

void miv::showInitialReadStats(){
	response().out() << initialSamplereadAmountStatsHtml_.get("/ssv", rootName_);
}

void miv::getInitialReadStats(){
	ret_json();
	response().out() << tableToJsonRowWise(stats_);
}

void miv::getAllSampleNames() {
	//std::cout << "getAllSampleNames" << std::endl;
	ret_json();
	cppcms::json::value r;
	auto samps = bib::getVecOfMapKeys(allInfoBySample_);
	bibseq::sort(samps);
	//std::cout <<bib::conToStr( samps , ",")<< std::endl;
	r = samps;
	//std::cout << "before resposne out" << std::endl;
	response().out() << r;
}


void miv::rootName() {
	//std::cout << "rootName" << std::endl;
	ret_json();
	cppcms::json::value r;
	r = rootName_;
	response().out() << r;
}

void miv::mipNames(std::string geneName) {
	ret_json();
	cppcms::json::value ret;
	auto mipNames = bibseq::getVectorOfMapKeys(mipAnalysisFolders_);
	bibseq::sort(mipNames);
	uint32_t count = 0;
	for(const auto & mPos : iter::range(mipNames.size())) {
		if(bibseq::beginsWith(mipNames[mPos], geneName)) {
			ret[count] = mipNames[mPos];
			++count;
		}
	}
	response().out() << ret;
}

void miv::geneNames() {
	//std::cout << "geneNames" << std::endl;
	ret_json();
	cppcms::json::value ret;
	auto mipNames = bibseq::getVectorOfMapKeys(mipAnalysisFolders_);
	bibseq::sort(mipNames);
	std::set<std::string> geneNames;
	for(const auto & mPos : iter::range(mipNames.size())) {
		geneNames.emplace(mipNames[mPos].substr(0,mipNames[mPos].find_last_of("_")));
	}
	for(const auto & gEnum : iter::enumerate(geneNames)) {
		ret[gEnum.index] = gEnum.element;
	}
	response().out() << ret;
}

void miv::showGeneInfo(std::string geneName) {
	auto mipNames = bibseq::getVectorOfMapKeys(mipAnalysisFolders_);
	bibseq::sort(mipNames);
	uint32_t count = 0;
	for(const auto & mPos : iter::range(mipNames.size())) {
		if(bibseq::beginsWith(mipNames[mPos], geneName)) {
			++count;
		}
	}
	if(0 == count){
		std::cout << "Gene name wasn't found :" << geneName << std::endl;
		response().out() << redirectPageHtml_.get("/ssv", rootName_);
	}else{
		response().out() << oneGeneInfoHtml_.get("/ssv", rootName_);
	}
}

void miv::showOneSampAllMip(std::string sampName) {
	if(allInfoBySample_.find(sampName) == allInfoBySample_.end()){
		std::cout << "Sample Name wasn't found: " << sampName << std::endl;
		response().out() << redirectPageHtml_.get("/ssv", rootName_);
	}else{
		response().out() << oneSampAllMipInfoHtml_.get("/ssv", rootName_);
	}
}

void miv::sampMipNamesData(std::string sampName) {
	ret_json();
	cppcms::json::value ret;
	auto mipNames = allInfoBySample_[sampName].getColumn("mipName");
	auto mipCounts = bibseq::countVec(mipNames);
	auto singleMipNames = bib::getVecOfMapKeys(mipCounts);
	bibseq::sort(singleMipNames);
	ret["mipNames"] = singleMipNames;
	response().out() << ret;
}

void miv::oneSampAllMipData(std::string sampName, std::string mipNames) {
	std::cout << "oneSampAllMipData - sampName: " << sampName << " mipNames: " << mipNames << std::endl;
	ret_json();
	auto mipTab = allInfoBySample_[sampName];
	auto mipToks = bibseq::tokenizeString(mipNames, "DELIM");
	auto containsMipName = [&mipToks](const std::string & str) {
		return bibseq::in(str, mipToks);
	};
	//std::cout << bibseq::vectorToString(sampToks,",")<< std::endl;
	auto trimedTab = mipTab.extractByComp("mipName", containsMipName);
	trimedTab.sortTable("mipName", true);
	auto ret = tableToJsonRowWise(trimedTab);
	auto outMipNames = trimedTab.getColumn("mipName");
	auto geneNames = trimedTab.getColumn("geneName");

	auto mipCounts = bibseq::countVec(outMipNames);
	auto geneCounts = bibseq::countVec(geneNames);
	auto singleMipNames = bib::getVecOfMapKeys(mipCounts);
	ret["mipNames"] = singleMipNames;
	auto singleGeneNames = bib::getVecOfMapKeys(geneCounts);
	ret["geneNames"] = singleGeneNames;
	auto & genInfo = ret["geneInfo"];
	for(const auto & g : geneNames) {
		genInfo[g] = bibseq::getTargetsAtPositions(singleMipNames, bibseq::getPositionsOfTargetStartsWith(singleMipNames, g));
	}
	std::unordered_map<std::string,uint32_t> mipClusIdCounts;
	for(const auto & m : trimedTab.getColumn("clusterID")) {
		++mipClusIdCounts[m];
	}
	auto outColors = bib::njhColors(mipClusIdCounts.size());
	bibseq::VecStr outColorsStrs;
	outColorsStrs.reserve(outColors.size());
	for(const auto & c : outColors) {
		outColorsStrs.emplace_back("#" + c.hexStr_);
	}
	ret["popColors"] = outColorsStrs;
	response().out() << ret;
}

void miv::showMipInfo(std::string mipName) {
	if(mipAnalysisFolders_.find(mipName) == mipAnalysisFolders_.end()){
		std::cout << "Could not find " << mipName << std::endl;
		response().out() << redirectPageHtml_.get("/ssv", rootName_);
	}else{
		response().out() << oneMipInfoHtml_.get("/ssv", rootName_);
	}

}

void miv::showOneSampleInfo(std::string mipName ,std::string sampName) {
	std::cout << "showOneSampleInfo " << std::endl;
	std::cout << mipName << " " << sampName << std::endl;
	auto search = sampleMipAnalysisFolders_.find(sampName);
	bool redirect = false;
	if(search == sampleMipAnalysisFolders_.end()){
		std::cout << "showOneSampleInfo: Couldn't find sampName, " << sampName << ", redirecting" << std::endl;
		redirect = true;
	}else{
		auto mSearch = search->second.find(mipName);
		if(mSearch == search->second.end()){
			std::cout << "showOneSampleInfo: Couldn't find mipName, " << mipName << std::endl;
			redirect = true;
		}else{

		}
	}
	if(redirect){
		std::cout << "couldn't find data" << std::endl;
		response().out() << redirectPageHtml_.get("/ssv", rootName_);
	}else{
		response().out() << oneSampInfoHtml_.get("/ssv", rootName_);
	}
}

void miv::showAllSampInfo(std::string mipName) {
	if(mipAnalysisFolders_.find(mipName) == mipAnalysisFolders_.end()){
		std::cout << "Could not find mipname " << mipName << std::endl;
		response().out() << redirectPageHtml_.get("/ssv", rootName_);
	}else{
		response().out() << allSampsInfoHtml_.get("/ssv", rootName_);
	}

}

void miv::showMinTree(std::string mipName, std::string sampname) {

	response().out() << minTreeViewHtml_.get("/ssv", rootName_);
}

void miv::popInfoData(std::string mipName) {
	ret_json();
	auto search = mipAnalysisFolders_.find(mipName);
	cppcms::json::value ret;
	if(search == mipAnalysisFolders_.end()) {
		std::cout << "Couldn't find mipName: " << mipName << std::endl;
	} else {
		ret = tableToJsonRowWise(bibseq::table(appendSlashRet(search->second.string()) + "population/populationCluster.tab.txt", "\t", true));
	}
	response().out() << ret;
}

void miv::getColors(std::string num) {
	ret_json();
	cppcms::json::value ret;
	auto outColors = bib::njhColors(std::stoi(num));
	bibseq::VecStr outColorsStrs;
	outColorsStrs.reserve(outColors.size());
	for(const auto & c : outColors) {
		outColorsStrs.emplace_back("#" + c.hexStr_);
	}
	ret["colors"] = outColorsStrs;
	response().out() << ret;
}

void miv::allSampsInfoData(std::string mipName, std::string sampNames) {
	std::cout << "allSampsInfoData - mipName: " << mipName << " sampNames: " << sampNames << std::endl;
	ret_json();
	cppcms::json::value ret;
	auto search = mipAnalysisFolders_.find(mipName) ;
	if(search != mipAnalysisFolders_.end() ){
		auto sampTab = bibseq::table(search->second.string() + "/selectedClustersInfo.tab.txt", "\t", true);
		auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
		auto containsSampName = [&sampToks](const std::string & str) {
			return bibseq::in(str, sampToks);
		};
		auto trimedTab = sampTab.extractByComp("Sample", containsSampName);
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

	}else{
		std::cout << "Coludn't find mipName: " << mipName << std::endl;
	}
	response().out() << ret;
}
void miv::mipSampleNames(std::string mipName) {
	ret_json();
	cppcms::json::value ret;
	auto sampNames = sampNamesForMip_[mipName];
	bibseq::sort(sampNames);
	ret = sampNames;
	response().out() << ret;
}

void miv::colorsData() {
	ret_json();
	cppcms::json::value r;
	r["A"] = "#ff8787";
	r["a"] = "#ff8787";

	r["C"] = "#afffaf";
	r["c"] = "#afffaf";

	r["G"] = "#ffffaf";
	r["g"] = "#ffffaf";

	r["T"] = "#87afff";
	r["t"] = "#87afff";

	r["-"] = "e6e6e6";

	response().out() << r;
}

void miv::popSeqData(std::string mipName) {
	ret_json();
	//if reads haven't been read yet, read them in
	cppcms::json::value ret;
	auto search = mipAnalysisFolders_.find(mipName);
	if(search != mipAnalysisFolders_.end()){
		if(popReads_.find(mipName) == popReads_.end()) {
			bibseq::readObjectIO reader;
			reader.read("fastq", appendSlashRet(search->second.string()) + "population/" + mipName + ".fastq",false);
			popReads_[mipName] = reader.reads;
		}
		ret = seqsToJson(popReads_[mipName]);
	}else{
		std::cout << "popSeqData: " << "couldn't find mipName: " << mipName << std::endl;
	}
	response().out() << ret;
}

void miv::minTreeData(std::string mipName, std::string sampname) {
	cppcms::json::value ret;
	auto search = mipAnalysisFolders_.find(mipName);
	if(search != mipAnalysisFolders_.end()){
		auto dotFileName = appendSlashRet(search->second.string()) + "dotFiles/" + sampname + ".dot";
		if(bfs::exists(bfs::path(dotFileName))){
			ret = bibseq::dotToJson(dotFileName);
		}else{
			std::cout << "minTreeData: " << "couldn't find dot file " << dotFileName << std::endl;
		}
	}else{
		std::cout << "minTreeData: " << "couldn't find mipName: " << mipName << std::endl;
	}
	ret_json();
	response().out() << ret;
}

void miv::oneSampTabData(std::string mipName, std::string sampname) {
	std::cout << "oneSampTabData - mipName: " << mipName << " sampName: " << sampname << std::endl;
	ret_json();
	cppcms::json::value ret;
	auto search = mipAnalysisFolders_.find(mipName);
	if(search != mipAnalysisFolders_.end()){
		auto tab = bibseq::table(search->second.string() + "/selectedClustersInfo.tab.txt", "\t", true);
		auto outTab = tab.getRows("sName", sampname);
		ret = tableToJsonRowWise(outTab);
	}else{
		std::cout << "oneSampTabData: " << "couldn't find mipName: " << mipName << std::endl;
	}
	response().out() << ret;
}

void miv::oneSampInitSeqData(std::string mipName, std::string sampName) {
	ret_json();
	cppcms::json::value ret;
	//if reads haven't been read yet, read them in
	auto search = sampleMipAnalysisFolders_.find(sampName);
	if(search == sampleMipAnalysisFolders_.end()){
		std::cout << "oneSampInitSeqData: Couldn't find sampName, " << sampName << ", redirecting" << std::endl;
	}else{
		auto mSearch = search->second.find(mipName);
		if(mSearch == search->second.end()){
			std::cout << "oneSampInitSeqData: Couldn't find mipName, " << mipName << std::endl;
		}else{
			auto fileName = appendSlashRet(mSearch->second.string()) + mipName + "_clustered.fastq";
			if(bfs::exists(bfs::path(fileName))){
				if(clusteredReads_[mipName][sampName].empty()) {
					bibseq::readObjectIO reader;
					reader.read("fastq", fileName,false);
					clusteredReads_[mipName][sampName] = reader.reads;
				}
				ret = seqsToJson(clusteredReads_[mipName][sampName]);
			}else{
				std::cout << "oneSampInitSeqData: " << " couldn't find clustered file: " << fileName << std::endl;
			}
		}
	}
	response().out() << ret;
}

void miv::oneSampFinalSeqData(std::string mipName, std::string sampName) {
	ret_json();
	//if reads haven't been read yet, read them in
	cppcms::json::value ret;
	auto search = mipAnalysisFolders_.find(mipName);
	if(search != mipAnalysisFolders_.end()){
		auto fileName = appendSlashRet(search->second.string()) + "final/" + sampName + ".fastq";
		if(bfs::exists(bfs::path(fileName))){
			if(filteredReads_[mipName][sampName].empty()) {
				bibseq::readObjectIO reader;
				reader.read("fastq", fileName,false);
				filteredReads_[mipName][sampName] = reader.reads;
			}
			ret = seqsToJson(filteredReads_[mipName][sampName]);
		}else{
			std::cout << "oneSampFinalSeqData: " << " couldn't find file " << fileName << std::endl;
		}
	}else{
		std::cout << "oneSampFinalSeqData: " << "couldn't find mipName: " << mipName << std::endl;
	}
	response().out() << ret;
}


void miv::getBarcodeInfoPerSamplePerMip(std::string mipName, std::string sampName){
	ret_json();
	std::cout << "getBarcodeInfoPerSamplePerMip" << std::endl;
	cppcms::json::value ret;
	//if reads haven't been read yet, read them in
	auto search = sampleMipAnalysisFolders_.find(sampName);
	if(search == sampleMipAnalysisFolders_.end()){
		std::cout << "getBarcodeInfoPerSamplePerMip: Couldn't find sampName, " << sampName << ", redirecting" << std::endl;
	}else{
		auto mSearch = search->second.find(mipName);
		if(mSearch == search->second.end()){
			std::cout << "getBarcodeInfoPerSamplePerMip: Couldn't find mipName, " << mipName << std::endl;
		}else{
			auto mainMipAnalysisDir = appendSlashRet(mSearch->second.string()) + "/";
			auto barcodeDir = mainMipAnalysisDir + "barcodeInfo/";
			auto barcodeCoverageDir = barcodeDir + "barcodeCoverageInfo/";
			//final cluster info
			{
				auto fileName = mainMipAnalysisDir + "info.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					ret ["clusterBarInfo"] = tableToJsonRowWise(table(fileName, "\t", true));
				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}

			//barcode coverage after
			{
				auto fileName = barcodeCoverageDir + "coverageAfterFiltering.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					ret ["coverageAfter"] = tableToJsonRowWise(table(fileName, "\t", true));
				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}
			//barcode performance
			{
				auto fileName = barcodeCoverageDir + "barcodePerformance.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					ret ["performance"] = tableToJsonRowWise(table(fileName, "\t", true));
				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}
			//barcode coverage before
			{
				auto fileName = barcodeCoverageDir + "coverageBeforeFiltering.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					ret ["coverageBefore"] = tableToJsonRowWise(table(fileName, "\t", true));
				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}
			//barcode afterIden
			{
				auto fileName = barcodeCoverageDir + "uniqueSeqsAfterIden.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					auto idenTab = table(fileName, "\t", true);
					if(!idenTab.content_.empty()){
						ret ["uniqueIdentical"] = tableToJsonRowWise(idenTab);
					}else{
						std::cout << "idenTab is empty" << std::endl;
					}

				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}
			//barcode afterClus
			{
				auto fileName = barcodeCoverageDir + "uniqueSeqsAfterClus.tab.txt";
				if(bfs::exists(bfs::path(fileName))){
					auto clusTab = table(fileName, "\t", true);
					if(!clusTab.content_.empty()){
						ret ["uniqueCluster"] = tableToJsonRowWise(clusTab);
					}else{
						std::cout << "idenTab is empty" << std::endl;
					}
				}else{
					std::cout << "getBarcodeInfoPerSamplePerMip: " << " couldn't find clustered file: " << fileName << std::endl;
				}
			}
		}
	}
	std::cout << ret << std::endl;
	response().out() << ret;
}

void miv::mainPage() {
	response().out() << mainPageHtml_.get("/ssv", rootName_);
}

int mipViewer(std::map<std::string, std::string> inputCommands){
	bibseq::seqSetUp setUp(inputCommands);
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "miv";
	std::string resourceDirName = "";
	setUp.setOption(resourceDirName, "-resourceDirName", "Name of the resource Directory where the js and hmtl is located", true);
	if(resourceDirName.back() != '/'){
		resourceDirName.append("/");
	}
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
  appConfig["resources"] = resourceDirName;
  appConfig["js"] = resourceDirName + "js/";
  appConfig["css"] = resourceDirName + "css/";
  auto pass = configTest(appConfig, miv::requiredOptions());
  if(!pass){
  	exit(1);
  }
	try {
		cppcms::service app(config);
		app.applications_pool().mount(
				cppcms::applications_factory<miv>(appConfig));
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
} /* namespace bibseq */
