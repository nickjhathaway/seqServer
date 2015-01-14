/*
 * seqApp.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "seqApp.hpp"
#include "seqServer/exception.h"

namespace bibseq {

bool seqApp::configTest(const MapStrStr & config,
		const VecStr & checks,
		const std::string nameOfClass){
	bool passed = true;
	VecStr missing;
	//additional checks
	for(const auto & check : checks){
		if(config.find(check) == config.end()){
			passed = false;
			missing.emplace_back(check);
			//warnings.emplace_back("Config doesn't contain " + check);
		}
	}
	//print warning messaging for anything missing from config
	if(!passed){
		//std::cerr << bib::bashCT::red << bib::bashCT::bold;
		//printVector(warnings, "\n", std::cerr);
		//std::cerr << bib::bashCT::reset << std::endl;
		throw configException{nameOfClass,missing};
	}
	return passed;
}

VecStr seqApp::requiredOptions()const{
	return VecStr{"js", "css", "name"};
}

seqApp::seqApp(cppcms::service& srv,
		std::map<std::string, std::string>  config):cppcms::application(srv)
				{
	//check configuration
	bool pass = configTest(config, requiredOptions(), "seqApp");
	//load js and css
	jsAndCss_.emplace("jsLibs", getLibFiles(config["js"], ".js"));
	jsAndCss_.emplace("jsOwn", getOwnFiles(config["js"], ".js"));
	jsAndCss_.emplace("cssLibs",getLibFiles(config["css"], ".css"));
	jsAndCss_.emplace("cssOwn",getOwnFiles(config["css"], ".css"));
	//js and css loading
	dispMap(&seqApp::jsLibs,this, "jsLibs");
	dispMap(&seqApp::jsOwn,this, "jsOwn");
	dispMap(&seqApp::cssLibs,this, "cssLibs");
	dispMap(&seqApp::cssOwn,this, "cssOwn");


	//general information
	dispMap(&seqApp::colorsData,this, "baseColors");

	dispMap_1arg(&seqApp::getColors,this, "getColors", "(\\d+)");

}

seqApp::~seqApp() {

}

void seqApp::jsLibs() {
	ret_js();
	auto search = jsAndCss_.find("jsLibs");
	response().out() << search->second.get();
}

void seqApp::jsOwn() {
	ret_js();
	auto search = jsAndCss_.find("jsOwn");
	response().out() << search->second.get();
}

void seqApp::cssLibs() {
	ret_css();
	auto search = jsAndCss_.find("cssLibs");
	response().out() << search->second.get();
}

void seqApp::cssOwn() {
	ret_css();
	auto search = jsAndCss_.find("cssOwn");
	response().out() << search->second.get();
}

void seqApp::colorsData() {
	ret_json();
	cppcms::json::value r;
	r["A"] = "#ff8787";
	r["a"] = "#e66e6e";

	r["C"] = "#afffaf";
	r["c"] = "#96dc96";

	r["G"] = "#ffffaf";
	r["g"] = "#dcdc91";

	r["T"] = "#87afff";
	r["t"] = "#6e96e6";

	r["-"] = "#e6e6e6";

	response().out() << r;
}

void seqApp::getColors(std::string num) {
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

} /* namespace bibseq */
