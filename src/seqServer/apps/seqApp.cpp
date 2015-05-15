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
		}
	}
	//print warning messaging for anything missing from config
	if(!passed){
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
	configTest(config, requiredOptions(), "seqApp");
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

	dispMap_2arg(&seqApp::sort,this, "sort", "(\\w+)/(\\w+)");
	dispMap_1arg(&seqApp::muscleAln,this, "muscle", "(\\w+)");
	dispMap_1arg(&seqApp::removeGaps,this, "removeGaps", "(\\w+)");
	dispMap_1arg(&seqApp::complementSeqs,this, "complement", "(\\w+)");


	//general information
	dispMap(&seqApp::colorsData,this, "baseColors");
	dispMap(&seqApp::getProteinColors, this, "proteinColors");

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


void seqApp::getProteinColors(){
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
	ret["-"] = "#e6e6e6";
	response().out() << ret;
}

void seqApp::sort(std::string uid, std::string sortBy){
	bib::scopedMessage mess("sort", std::cout, debug_);
	if(seqs_.containsRecord(uid)){
		if(seqs_.recordValid(uid)){
			ret_json();
			auto ret = seqs_.sort(uid, sortBy);
			ret["uid"] = uid;
			response().out() << ret;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}
}
void seqApp::muscleAln(std::string uid){
	bib::scopedMessage mess("muscleAln", std::cout, debug_);
	if(seqs_.containsRecord(uid)){
		if(seqs_.recordValid(uid)){
			ret_json();
			auto ret = seqs_.muscle(uid);
			ret["uid"] = uid;
			response().out() << ret;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}
}
void seqApp::removeGaps(std::string uid){
	bib::scopedMessage mess("removeGaps", std::cout, debug_);
	if(seqs_.containsRecord(uid)){
		if(seqs_.recordValid(uid)){
			ret_json();
			auto ret =  seqs_.removeGaps(uid);
			ret["uid"] = uid;
			response().out() << ret;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}
}
void seqApp::complementSeqs(std::string uid){
	bib::scopedMessage mess("complementSeqs", std::cout, debug_);
	if(seqs_.containsRecord(uid)){
		if(seqs_.recordValid(uid)){
			ret_json();
			auto ret =  seqs_.rComplement(uid);
			ret["uid"] = uid;
			response().out() << ret;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}
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
