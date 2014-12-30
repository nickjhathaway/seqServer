#pragma once
/*
 * pearOutViewer.hpp
 *
 *  Created on: Dec 29, 2014
 *      Author: nickhathaway
 */

#include "apps/seqApp.hpp"
#include "utils.h"

namespace bibseq {
namespace bfs = boost::filesystem;

class pov: public bibseq::seqApp {
private:

	bib::FileCache mainPageHtml_;


	std::string rootName_;
	std::string dir_;
	std::unordered_map<std::string, std::vector<bfs::path>> files_;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	pov(cppcms::service& srv, std::map<std::string, std::string> config)
	: bibseq::seqApp(srv, config)
	, mainPageHtml_(make_path("../resources/pov/mainPage.html"))
	, rootName_(config["name"])
	, dir_(config["dir"])
	{
		mainPageHtml_.replaceStr("/ssv", rootName_);
		//main page
		dispMapRoot(&pov::mainPage, this);
		//general information
		dispMap_1arg(&pov::getColors,this, "getColors", "(\\d+)");

		mapper().root(rootName_);

		auto files = bib::files::listAllFiles(dir_, true, bibseq::VecStr {"pearOut_"});

		std::cout << "Finished set up" << std::endl;
	}

	static VecStr requiredOptions(){
		return VecStr{"name", "dir"};
	}

	void rootName() {
		//std::cout << "rootName" << std::endl;
		ret_json();
		cppcms::json::value r;
		r = rootName_;
		response().out() << r;
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

	void mainPage() {
		response().out() << mainPageHtml_.get("/ssv", rootName_);
	}



};

int pearOutViwer(std::map<std::string, std::string> inputCommands);


} /* namespace bibseq */


