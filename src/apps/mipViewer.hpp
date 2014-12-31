#pragma once
/*
 * mipViewer.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "apps/seqApp.hpp"
#include "utils.h"

namespace bibseq {
namespace bfs = boost::filesystem;

class ssv: public bibseq::seqApp {
private:

	bib::FileCache mainPageHtml_;
	bib::FileCache oneGeneInfoHtml_;
	bib::FileCache oneSampAllMipInfoHtml_;
	bib::FileCache oneMipInfoHtml_;
	bib::FileCache allSampsInfoHtml_;
	bib::FileCache oneSampInfoHtml_;
	bib::FileCache minTreeViewHtml_;
	bib::FileCache popInfoHtml_;
	bib::FileCache redirectPageHtml_;

	std::unordered_map<std::string, std::vector<bibseq::readObject>> reads_;
	std::unordered_map<std::string, std::string> readsLocations_;

	std::unordered_map<std::string,
			std::unordered_map<std::string, std::vector<bibseq::readObject>>>initialReads_;
	std::unordered_map<std::string,std::unordered_map<std::string, std::string>> initialReadsLocations_;

	std::unordered_map<std::string,std::unordered_map<std::string,std::vector<bibseq::readObject>>> finalReads_;
	std::unordered_map<std::string,std::unordered_map<std::string, std::string>> finalReadsLocations_;

	std::unordered_map<std::string,std::unordered_map<std::string, std::string>> dotFilesLocations_;

	std::unordered_map<std::string, std::string> popInfoLocations_;
	std::unordered_map<std::string, std::string> allInfoLocations_;
	std::unordered_map<std::string, bibseq::table> allInfoBySample_;

	std::string rootName_;
	std::string clusteringDir_;
	std::unordered_map<std::string, std::vector<bfs::path>> files_;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	ssv(cppcms::service& srv, std::map<std::string, std::string> config)
	: bibseq::seqApp(srv, config)
	, mainPageHtml_(make_path(config["resources"] + "mip/mainPage.html"))
	, oneGeneInfoHtml_(make_path(config["resources"] + "mip/oneGeneView.html"))
	, oneSampAllMipInfoHtml_(make_path(config["resources"] + "mip/oneSampAllMipInfo.html"))
	, oneMipInfoHtml_(make_path(config["resources"] + "mip/oneMipInfo.html"))
	, allSampsInfoHtml_(make_path(config["resources"] + "mip/allSampsInfo.html"))
	, oneSampInfoHtml_(make_path(config["resources"] + "mip/oneSampInfo.html"))
	, minTreeViewHtml_(make_path(config["resources"] + "mip/minTreeView.html"))
	, popInfoHtml_(make_path(config["resources"] + "mip/popInfo.html"))
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
		popInfoHtml_.replaceStr("/ssv", rootName_);
		redirectPageHtml_.replaceStr("/ssv", rootName_);
		//main page
		//<std::remove_reference<decltype(*this)>::type>
		dispMapRoot(&ssv::mainPage, this);
		dispMap(&ssv::geneNames,this, "geneNames");
		dispMap(&ssv::getAllSampleNames, this,"allSampNames");
		//gene page
		dispMap_1arg(&ssv::showGeneInfo,this, "geneInfo", "(\\w+)");
		dispMap_1arg(&ssv::mipNames,this, "mipNames", "(\\w+)");
		//one samp all mips page
		dispMap_1arg(&ssv::showOneSampAllMip,this, "oneSampAllMipInfo", "(\\w+)");
		dispMap_2arg(&ssv::oneSampAllMipData,this, "oneSampAllMipData", "(\\w+)/(\\w+)");
		dispMap_1arg(&ssv::sampMipNamesData,this, "sampMipNames", "(\\w+)");
		//show one mip target info and sample names
		dispMap_1arg(&ssv::showMipInfo,this, "mipInfo", "(\\w+)");
		dispMap_1arg(&ssv::mipSampleNames,this, "mipSampleNames", "(\\w+)");
		//show the data table with all sample information
		dispMap_1arg(&ssv::showAllSampInfo,this, "allSamps", "(\\w+)");
		dispMap_2arg(&ssv::allSampsInfoData,this, "allSampsInfo", "(\\w+)/(\\w+)");
		//show the mip target info for one sample
		dispMap_2arg(&ssv::showOneSampleInfo,this, "oneSampInfo", "(\\w+)/(\\w+)");
		dispMap_2arg(&ssv::oneSampInitSeqData,this, "oneSampInitSeqData", "(\\w+)/(\\w+)");
		dispMap_2arg(&ssv::oneSampFinalSeqData,this, "oneSampFinalSeqData", "(\\w+)/(\\w+)");
		dispMap_2arg(&ssv::oneSampTabData,this, "oneSampTabData", "(\\w+)/(\\w+)");
		//show the minimum spanning tree for one sample info
		dispMap_2arg(&ssv::showMinTree,this, "showMinTree", "(\\w+)/(\\w+)");
		dispMap_2arg(&ssv::minTreeData,this, "minTreeData", "(\\w+)/(\\w+)");
		//show the Population information for one mip target
		dispMap_1arg(&ssv::showPopData,this, "pop", "(\\w+)");
		dispMap_1arg(&ssv::popInfoData,this, "popInfo", "(\\w+)");
		dispMap_1arg(&ssv::popSeqData,this, "popSeqData", "(\\w+)");

		//general information
		dispMap(&ssv::rootName,this, "rootName");
		dispMap(&ssv::colorsData,this, "baseColors");
		dispMap_1arg(&ssv::getColors,this, "getColors", "(\\d+)");



		mapper().root(rootName_);

		auto files = bib::files::listAllFiles(clusteringDir_, true, bibseq::VecStr {"analysis"});
		for(const auto & f : files) {
			auto toks = bibseq::tokenizeString(f.first.string(), "/");
			auto aPos = bibseq::find(toks,"analysis" );
			--aPos;
			files_[*aPos].emplace_back(f.first);
			//std::cout << (*aPos) + "_clustered.fastq" << std::endl;
			if(f.first.string().find((*aPos) + ".fastq" ) != std::string::npos &&
					f.first.string().find("population")!= std::string::npos) {
				readsLocations_[*aPos] = f.first.string();
			} else if (f.first.string().find("selectedClustersInfo.tab.txt")!= std::string::npos) {
				allInfoLocations_[*aPos] = f.first.string();
			} else if (f.first.string().find("populationCluster.tab.txt")!= std::string::npos) {
				popInfoLocations_[*aPos] = f.first.string();
			} else if (f.first.string().find("originals") != std::string::npos) {
				std::string samp = bib::files::removeExtension(toks.back());
				initialReadsLocations_[*aPos][samp] = f.first.string();
				//std::cout << f.first.string() << std::endl;
			} else if (f.first.string().find("final") != std::string::npos) {
				std::string samp = bib::files::removeExtension(toks.back());
				finalReadsLocations_[*aPos][samp] = f.first.string();
				//std::cout << f.first.string() << std::endl;
			} else if (f.first.string().find(".dot") != std::string::npos &&
					f.first.string().find(".pdf") == std::string::npos) {
				std::string samp = bib::files::removeExtension(toks.back());
				dotFilesLocations_[*aPos][samp] = f.first.string();
			}
		}
		for(const auto & allSampInfo : allInfoLocations_) {
			auto tab = bibseq::table(allSampInfo.second, "\t", true );
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

	static VecStr requiredOptions(){
		return VecStr{"name", "clusDir", "resources", "js", "css"};
	}

	void getAllSampleNames() {
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


	void rootName() {
		//std::cout << "rootName" << std::endl;
		ret_json();
		cppcms::json::value r;
		r = rootName_;
		response().out() << r;
	}

	void mipNames(std::string geneName) {
		ret_json();
		cppcms::json::value ret;
		auto mipNames = bibseq::getVectorOfMapKeys(files_);
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

	void geneNames() {
		//std::cout << "geneNames" << std::endl;
		ret_json();
		cppcms::json::value ret;
		auto mipNames = bibseq::getVectorOfMapKeys(files_);
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

	void showPopData(std::string mipName) {
		if(readsLocations_.find(mipName) == readsLocations_.end() ){
			std::cout << "Couldn't find mipname " << mipName << std::endl;
			response().out() << redirectPageHtml_.get("/ssv", rootName_);
		}else{
			response().out() << popInfoHtml_.get("/ssv", rootName_);
		}

	}

	void showGeneInfo(std::string geneName) {
		auto mipNames = bibseq::getVectorOfMapKeys(files_);
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

	void showOneSampAllMip(std::string sampName) {
		if(allInfoBySample_.find(sampName) == allInfoBySample_.end()){
			std::cout << "Sample Name wasn't found: " << sampName << std::endl;
			response().out() << redirectPageHtml_.get("/ssv", rootName_);
		}else{
			response().out() << oneSampAllMipInfoHtml_.get("/ssv", rootName_);
		}
	}

	void sampMipNamesData(std::string sampName) {
		ret_json();
		cppcms::json::value ret;
		auto mipNames = allInfoBySample_[sampName].getColumn("mipName");
		auto mipCounts = bibseq::countVec(mipNames);
		auto singleMipNames = bib::getVecOfMapKeys(mipCounts);
		bibseq::sort(singleMipNames);
		ret["mipNames"] = singleMipNames;
		response().out() << ret;
	}

	void oneSampAllMipData(std::string sampName, std::string mipNames) {
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

	void showMipInfo(std::string mipName) {
		if(dotFilesLocations_.find(mipName) == dotFilesLocations_.end()){
			std::cout << "Could not find " << mipName << std::endl;
			response().out() << redirectPageHtml_.get("/ssv", rootName_);
		}else{
			response().out() << oneMipInfoHtml_.get("/ssv", rootName_);
		}

	}

	void showOneSampleInfo(std::string mipName ,std::string sampName) {
		std::cout << "showOneSampleInfo" << std::endl;
		std::cout << mipName << " " << sampName << std::endl;
		if(initialReadsLocations_[mipName][sampName] == ""){
			std::cout << "couldn't find data" << std::endl;
			response().out() << redirectPageHtml_.get("/ssv", rootName_);
		}else{
			response().out() << oneSampInfoHtml_.get("/ssv", rootName_);
		}

	}

	void showAllSampInfo(std::string mipName) {
		if(allInfoLocations_.find(mipName) == allInfoLocations_.end()){
			std::cout << "Could not find mipname " << mipName << std::endl;
			response().out() << redirectPageHtml_.get("/ssv", rootName_);
		}else{
			response().out() << allSampsInfoHtml_.get("/ssv", rootName_);
		}

	}

	void showMinTree(std::string mipName, std::string sampname) {

		response().out() << minTreeViewHtml_.get("/ssv", rootName_);
	}

	void popInfoData(std::string mipName) {
		ret_json();
		auto ret = tableToJsonRowWise(bibseq::table(popInfoLocations_[mipName], "\t", true));
		response().out() << ret;
	}

	void getColors(std::string num) {
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

	void allSampsInfoData(std::string mipName, std::string sampNames) {
		std::cout << "allSampsInfoData - mipName: " << mipName << " sampNames: " << sampNames << std::endl;
		ret_json();
		auto sampTab = bibseq::table(allInfoLocations_[mipName], "\t", true);
		auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
		auto containsSampName = [&sampToks](const std::string & str) {
			return bibseq::in(str, sampToks);
		};
		//std::cout << bibseq::vectorToString(sampToks,",")<< std::endl;
		auto trimedTab = sampTab.extractByComp("Sample", containsSampName);
		auto ret = tableToJsonRowWise(trimedTab);
		auto popCounts = bibseq::countVec(trimedTab.getColumn("popUID"));
		auto popColors = bib::njhColors(popCounts.size());

		bibseq::VecStr popColorsStrs(popColors.size());
		uint32_t count = 0;
		uint32_t halfCount = 0;
		//std::cout << popColorsStrs.size() << std::endl;
		for(const auto & cPos : iter::range(popColors.size())) {

			uint32_t pos = 0;
			if(cPos %2 == 0) {
				pos = popColors.size()/2 + halfCount;
				++halfCount;
			} else {
				pos = count;
				++count;
			}
			//std::cout << "\tcPos:" << cPos << std::endl;
			//std::cout << "\tpos:" << pos << std::endl;
			popColorsStrs[cPos] = "#" + popColors[pos].hexStr_;
		}
		//auto gen = bibseq::randomGenerator();

		//bibseq::shuffle(popColorsStrs, gen.mtGen_);
		ret["popColors"] = popColorsStrs;

		response().out() << ret;
	}
	void mipSampleNames(std::string mipName) {
		ret_json();
		cppcms::json::value ret;
		auto sampNames = bibseq::getVectorOfMapKeys(dotFilesLocations_[mipName]);
		bibseq::sort(sampNames);
		ret = sampNames;
		response().out() << ret;
	}

	void colorsData() {
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

	void popSeqData(std::string mipName) {
		ret_json();
		//if reads haven't been read yet, read them in
		if(reads_.find(mipName) == reads_.end()) {
			bibseq::readObjectIO reader;
			reader.read("fastq", readsLocations_[mipName],false);
			reads_[mipName] = reader.reads;
		}
		response().out() << seqsToJson(reads_[mipName]);
	}

	void minTreeData(std::string mipName, std::string sampname) {

		auto ret = bibseq::dotToJson(dotFilesLocations_[mipName][sampname]);
		ret_json();
		response().out() << ret;
	}

	void oneSampTabData(std::string mipName, std::string sampname) {
		std::cout << "oneSampTabData - mipName: " << mipName << " sampName: " << sampname << std::endl;

		ret_json();
		auto tab = bibseq::table(allInfoLocations_[mipName], "\t", true);
		auto outTab = tab.getRows("sName", sampname);
		auto ret = tableToJsonRowWise(outTab);
		response().out() << ret;
	}

	void oneSampInitSeqData(std::string mipname, std::string sampname) {

		ret_json();
		//if reads haven't been read yet, read them in
		if(initialReads_[mipname][sampname].empty()) {
			bibseq::readObjectIO reader;
			reader.read("fastq", initialReadsLocations_[mipname][sampname],false);
			initialReads_[mipname][sampname] = reader.reads;
		}

		response().out() << seqsToJson(initialReads_[mipname][sampname]);
	}

	void oneSampFinalSeqData(std::string mipname, std::string sampname) {

		ret_json();
		//if reads haven't been read yet, read them in
		if(finalReads_[mipname][sampname].empty()) {
			bibseq::readObjectIO reader;
			reader.read("fastq", finalReadsLocations_[mipname][sampname],false);
			finalReads_[mipname][sampname] = reader.reads;
		}
		response().out() << seqsToJson(finalReads_[mipname][sampname]);
	}

	void mainPage() {
		response().out() << mainPageHtml_.get("/ssv", rootName_);
	}



};

int mipViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


