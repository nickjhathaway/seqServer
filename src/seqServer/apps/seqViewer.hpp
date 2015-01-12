#pragma once
/*
 * seqViewer.hpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "seqServer/apps/seqApp.hpp"


namespace bibseq {

class ssv: public bibseq::seqApp {
private:

	typedef bibseq::seqApp super;

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

	virtual VecStr requiredOptions() const {
		return VecStr{"resources", "ioOptions"};
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
		auto search = pages_.find("mainPageHtml");
		response().out() << search->second.get("/ssv", rootName_);
	}



	void showMinTree() {

	}

	void minTreeData() {
	}

	void mainPage() {
		auto search = pages_.find("mainPageHtml");
		response().out() << search->second.get("/ssv", rootName_);
	}



};

int seqViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


