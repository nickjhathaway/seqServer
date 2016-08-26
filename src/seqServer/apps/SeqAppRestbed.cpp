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


void SeqAppRestbed::cssOwnHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("cssOwn")->second.get("/ssv", root_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::cssLibsHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("cssLibs")->second.get("/ssv", root_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsOwnHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("jsOwn")->second.get("/ssv", root_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsLibsHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("jsLibs")->second.get("/ssv", root_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssOwn() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "cssOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssLibs(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "cssLibs" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssLibsHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsOwn(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "jsOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::jsOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsLibs(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "jsLibs" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::jsLibsHandler, this,
							std::placeholders::_1)));
	return resource;
}


SeqAppRestbed::SeqAppRestbed(
		const Json::Value & config) :
		config_(config) {
	checkConfigThrow();
	//load js and css
	jsAndCss_.emplace("jsLibs",  getLibFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("jsOwn",   getOwnFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("cssLibs", getLibFiles(config_["css"].asString(), ".css"));
	jsAndCss_.emplace("cssOwn",  getOwnFiles(config_["css"].asString(), ".css"));
	//set root name
	root_ = config_["name"].asString();





	/*
	dispMap_1word(&seqApp::sort,this, "sort");
	dispMap(&seqApp::muscleAln,this, "muscle");
	dispMap(&seqApp::removeGaps,this, "removeGaps");
	dispMap(&seqApp::complementSeqs,this, "complement");
	dispMap(&seqApp::translateToProtein,this, "translate");
	dispMap(&seqApp::minTreeDataDetailed,this, "minTreeDataDetailed");
	*/

	//general information
	//dispMap(&seqApp::colorsData,this, "baseColors");
	//dispMap(&seqApp::getProteinColors, this, "proteinColors");

	//dispMap_1arg(&seqApp::getColors,this, "getColors", "(\\d+)");

}

std::vector<std::shared_ptr<restbed::Resource>> SeqAppRestbed::getAllResources(){
	std::vector<std::shared_ptr<restbed::Resource>> ret;
	ret.emplace_back(jsOwn());
	ret.emplace_back(jsLibs());
	ret.emplace_back(cssLibs());
	ret.emplace_back(cssOwn());

	ret.emplace_back(getProteinColors());
	ret.emplace_back(getDNAColors());
	ret.emplace_back(getColors());

	return ret;
}

SeqAppRestbed::~SeqAppRestbed(){

}

VecStr SeqAppRestbed::requiredOptions() const{
	return VecStr { "js", "css", "name" };
}


void SeqAppRestbed::getDNAColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::DNAColorsJson);
	session->close(restbed::OK, ColorFactory::DNAColorsJson, headers);
}

void SeqAppRestbed::getProteinColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::AAColorsJson);
	session->close(restbed::OK, ColorFactory::AAColorsJson, headers);
}

void SeqAppRestbed::getColorsHandler(
		std::shared_ptr<restbed::Session> session) const{
	uint32_t num = 0;
	const auto request = session->get_request();
	request->get_path_parameter("number",num);
	auto body =  ColorFactory::getColors(num).toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK,body, headers);

}

std::shared_ptr<restbed::Resource> SeqAppRestbed::getDNAColors() const{
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_ }, { "colorsData" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getDNAColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::getProteinColors() const{
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "proteinColors" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getProteinColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::getColors() const{
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { root_, "" }, { "getColors" }, {"number", UrlPathFactory::pat_nums_} }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}


void SeqAppRestbed::addPages(const bfs::path & dir){
	auto files = bib::files::listAllFiles(dir.string(), false, VecStr{".html"});
	for(const auto & file : files){
		if(bfs::is_regular_file(file.first)
			&& bib::endsWith(file.first.string(), ".html")){
			auto base = bfs::path(bfs::basename(file.first));
			base.replace_extension("");
			if(bib::in(base.string(), pages_)){
				pages_.erase(base.string());
			}
			pages_.emplace(base.string(), bfs::absolute(file.first));
		}
	}
}


std::string SeqAppRestbed::messStrFactory(const std::string & funcName) {
	return bib::err::F() << "[" << getCurrentDate() << "] " << funcName;
}

std::string SeqAppRestbed::messStrFactory(const std::string & funcName,
		const MapStrStr & args) {
	VecStr argsVec;
	for (const auto & kv : args) {
		argsVec.emplace_back(kv.first + " = " + kv.second);
	}
	std::string argStrs = messStrFactory(funcName) + " ["
			+ vectorToString(argsVec, ", ") + "]";
	return argStrs;
}



}  // namespace bibseq
