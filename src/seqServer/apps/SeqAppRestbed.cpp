/*
 * SeqAppRestbed.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "SeqAppRestbed.hpp"

namespace bibseq {


const std::string ColorFactory::DNAColorsJson{"{\"-\":\"#e6e6e6\","
		"\"A\":\"#ff8787\","
		"\"C\":\"#afffaf\","
		"\"G\":\"#ffffaf\","
		"\"N\":\"#AFAFAF\","
		"\"T\":\"#87afff\","
		"\"U\":\"#87afff\","
		"\"a\":\"#e66e6e\","
		"\"c\":\"#96dc96\","
		"\"g\":\"#dcdc91\","
		"\"n\":\"#7D7D7D\","
		"\"t\":\"#6e96e6\","
		"\"u\":\"#6e96e6\"}"};

const std::string ColorFactory::AAColorsJson{"{\"*\":\"#e6e6e6\","
		"\"-\":\"#e6e6e6\","
		"\"A\":\"#14b814\","
		"\"C\":\"#13d8d8\","
		"\"D\":\"#12ade0\","
		"\"E\":\"#117de8\","
		"\"F\":\"#13d0a1\","
		"\"G\":\"#134aef\","
		"\"H\":\"#1919f0\","
		"\"I\":\"#541ff2\","
		"\"K\":\"#8c25f4\","
		"\"L\":\"#14c86e\","
		"\"M\":\"#c32bf5\","
		"\"N\":\"#f631f6\","
		"\"P\":\"#f838c8\","
		"\"Q\":\"#f93e9c\","
		"\"R\":\"#fa4572\","
		"\"S\":\"#fb4b4b\","
		"\"T\":\"#fc7c52\","
		"\"V\":\"#fdab58\","
		"\"W\":\"#fed65f\","
		"\"X\":\"#999999\","
		"\"Y\":\"#ffff66\"}"};

Json::Value ColorFactory::getColors(uint32_t num){
	Json::Value ret;
	auto outColors = bib::njhColors(num);
	bibseq::VecStr outColorsStrs;
	outColorsStrs.reserve(outColors.size());
	for (const auto & c : outColors) {
		outColorsStrs.emplace_back("#" + c.hexStr_);
	}
	ret["colors"] = bib::json::toJson(outColorsStrs);
	return ret;
}
void SeqAppRestbed::checkConfigThrow() const{
	VecStr missing;
	for(const auto & required : requiredOptions()){
		if(!config_.isMember(required)){
			missing.emplace_back(required);
		}
	}
	if(!missing.empty()){
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << ": Error, missing the following required options: " << bib::conToStr(requiredOptions(), ", ") << "\n";
		ss << "given options are: " <<  bib::conToStr( config_.getMemberNames(), ", ") << "\n";
		throw std::runtime_error{ss.str()};
	}
}

SeqAppRestbed::SeqAppRestbed(const std::shared_ptr<restbed::Service> service,
		const Json::Value & config) :
		service_(service), config_(config) {
	checkConfigThrow();
	//load js and css
	jsAndCss_.emplace("jsLibs",  getLibFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("jsOwn",   getOwnFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("cssLibs", getLibFiles(config_["css"].asString(), ".css"));
	jsAndCss_.emplace("cssOwn",  getOwnFiles(config_["css"].asString(), ".css"));
	//set root name
	root_ = config_["name"].asString();



}

SeqAppRestbed::~SeqAppRestbed(){

}

VecStr SeqAppRestbed::requiredOptions() const{
	return VecStr { "js", "css", "name" };
}


void SeqAppRestbed::getDNAColorsHandler(
		const std::shared_ptr<restbed::Session> session) const {
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::DNAColorsJson);
	session->close(restbed::OK, ColorFactory::DNAColorsJson, headers);
}

void SeqAppRestbed::getProteinColorsHandler(
		const std::shared_ptr<restbed::Session> session) const {
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::AAColorsJson);
	session->close(restbed::OK, ColorFactory::AAColorsJson, headers);
}



}  // namespace bibseq
