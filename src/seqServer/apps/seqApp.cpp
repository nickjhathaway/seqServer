//
// seqServer - A library for visualizing sequence results data
// Copyright (C) 2012, 2015 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
// Jeffrey Bailey <Jeffrey.Bailey@umassmed.edu>
//
// This file is part of seqServer.
//
// seqServer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// seqServer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with seqServer.  If not, see <http://www.gnu.org/licenses/>.
//
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

	dispMap_1arg(&seqApp::sort,this, "sort", "(\\w+)");
	dispMap(&seqApp::muscleAln,this, "muscle");
	dispMap(&seqApp::removeGaps,this, "removeGaps");
	dispMap(&seqApp::complementSeqs,this, "complement");


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

	r["N"] = "#AFAFAF";
	r["n"] = "#7D7D7D";

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

void seqApp::sort(std::string sortBy){
	bib::scopedMessage mess("sort", std::cout, debug_);
	auto postData = request().post();
  auto postJson = bib::json::toJson(postData);
  std::string uid = postJson["uid"].asString();
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
void seqApp::muscleAln(){
	bib::scopedMessage mess("muscleAln", std::cout, debug_);
	auto postData = request().post();
  auto postJson = bib::json::toJson(postData);
  std::string uid = postJson["uid"].asString();
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
void seqApp::removeGaps(){
	bib::scopedMessage mess("removeGaps", std::cout, debug_);
	auto postData = request().post();
  auto postJson = bib::json::toJson(postData);
  std::string uid = postJson["uid"].asString();
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
void seqApp::complementSeqs(){
	bib::scopedMessage mess("complementSeqs", std::cout, debug_);
	auto postData = request().post();
  auto postJson = bib::json::toJson(postData);
  std::string uid = postJson["uid"].asString();
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
