/*
 * seqApp.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "seqApp.hpp"

namespace bibseq {

bool configTest(const MapStrStr & config, const VecStr & additionalChecks){
	bool passed = true;
	VecStr warnings;
	//check for root name
	if(config.find("name") == config.end()){
		passed = false;
		warnings.emplace_back("Config doesn't contain name");
	}
	//check for js resources
	if(config.find("js") == config.end()){
		passed = false;
		warnings.emplace_back("Config doesn't contain js");
	}
	//check for css resources
	if(config.find("css") == config.end()){
		passed = false;
		warnings.emplace_back("Config doesn't contain css");
	}
	//additional checks
	for(const auto & check : additionalChecks){
		if(config.find(check) == config.end()){
			passed = false;
			warnings.emplace_back("Config doesn't contain " + check);
		}
	}
	//print warning messaging for anything missing from config
	if(!passed){
		std::cerr << bib::bashCT::red << bib::bashCT::bold;
		printVector(warnings, "\n", std::cerr);
		std::cerr << bib::bashCT::reset << std::endl;
	}
	return passed;
}

seqApp::seqApp(cppcms::service& srv,
		std::map<std::string, std::string>  config):cppcms::application(srv), jsLibs_(getLibFiles(config["js"], ".js")),
				jsOwn_(getOwnFiles(config["js"], ".js")),cssLibs_(getLibFiles(config["css"], ".css")),
				cssOwn_(getOwnFiles(config["css"], ".css"))
				{
	//js and css loading
	dispMap(&seqApp::jsLibs,this, "jsLibs");
	dispMap(&seqApp::jsOwn,this, "jsOwn");
	dispMap(&seqApp::cssLibs,this, "cssLibs");
	dispMap(&seqApp::cssOwn,this, "cssOwn");

}

seqApp::~seqApp() {

}

void seqApp::jsLibs() {
	ret_js();
	response().out() << jsLibs_.get();
}

void seqApp::jsOwn() {
	ret_js();
	response().out() << jsOwn_.get();
}

void seqApp::cssLibs() {
	ret_css();
	response().out() << cssLibs_.get();
}

void seqApp::cssOwn() {
	ret_css();
	response().out() << cssOwn_.get();
}



} /* namespace bibseq */
