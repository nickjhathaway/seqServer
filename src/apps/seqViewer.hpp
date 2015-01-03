#pragma once
/*
 * seqViewer.hpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "apps/seqApp.hpp"


namespace bibseq {

class ssv: public bibseq::seqApp {
private:

	bib::FileCache mainPageHtml_;
	std::vector<bibseq::readObject> reads_;
	cppcms::json::value readsJson_;
	std::string rootName_;
	bool needsUpdate_ = false;
	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	ssv(cppcms::service& srv, std::map<std::string, std::string> config)
	: bibseq::seqApp(srv, config)
	, mainPageHtml_(make_path(config["resources"] + "ssv/mainPage.html"))
	, rootName_(config["name"])
	{
		mainPageHtml_.replaceStr("/ssv", rootName_);

		//main page
		dispMapRoot(&ssv::mainPage, this);
		dispMap(&ssv::seqData,this, "seqData");

		//general information
		dispMap(&ssv::rootName,this, "rootName");
		dispMap(&ssv::colorsData,this, "baseColors");
		dispMap_1arg(&ssv::getColors,this, "getColors", "(\\d+)");
		dispMap_1arg(&ssv::sort,this, "sort", "(\\w+)");
		mapper().root(rootName_);
		//read in data and set to the json
		readObjectIOOptions options(config["ioOptions"]);
		readObjectIO reader;
		reader.read(options);
		reads_ = reader.reads;
		readsJson_ = seqsToJson(reads_);
		needsUpdate_= false;
		std::cout << "Finished set up" << std::endl;
		std::cout << "Go to " << "localhost:" << config["port"] << config["name"] << std::endl;
	}

	static VecStr requiredOptions(){
		return VecStr{"name", "resources", "ioOptions", "js", "css"};
	}

	void seqData() {
		//std::cout << "getAllSampleNames" << std::endl;
		ret_json();
		if(needsUpdate_){
			readsJson_ = seqsToJson(reads_);
		}
		response().out() << readsJson_;
	}


	void rootName() {
		//std::cout << "rootName" << std::endl;
		ret_json();
		cppcms::json::value r;
		r = rootName_;
		response().out() << r;
	}

	void sort(std::string sortBy){
		readVecSorter::sortReadVector(reads_, sortBy);
		needsUpdate_ = true;
		response().out() << mainPageHtml_.get("/ssv", rootName_);
	}



	void showMinTree() {

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


	void minTreeData() {
	}

	void mainPage() {
		response().out() << mainPageHtml_.get("/ssv", rootName_);
	}



};

int seqViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


