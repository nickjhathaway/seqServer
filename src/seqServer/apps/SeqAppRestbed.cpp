/*
 * SeqAppRestbed.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "SeqAppRestbed.hpp"

namespace bibseq {

const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#e6e6e6\","
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
		"\"u\":\"#6e96e6\"}" };

const std::string ColorFactory::AAColorsJson { "{\"*\":\"#e6e6e6\","
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
		"\"Y\":\"#ffff66\"}" };

Json::Value ColorFactory::getColors(uint32_t num) {
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

void SeqAppRestbed::checkConfigThrow() const {
	VecStr missing;
	for (const auto & required : requiredOptions()) {
		if(debug_){
			std::cout << "Checking for required config option: " << required << std::endl;
		}
		if (!config_.isMember(required)) {
			missing.emplace_back(required);
		}
	}
	if (!missing.empty()) {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__
				<< ": Error, missing the following required options: "
				<< bib::conToStr(requiredOptions(), ", ") << "\n";
		ss << "given options are: " << bib::conToStr(config_.getMemberNames(), ", ")
				<< "\n";
		throw std::runtime_error { ss.str() };
	}
}

void SeqAppRestbed::cssOwnHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("cssOwn")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::cssLibsHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("cssLibs")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsOwnHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("jsOwn")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsLibsHandler(std::shared_ptr<restbed::Session> session) {
	std::string body = jsAndCss_.find("jsLibs")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssOwn() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "cssOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssLibs() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "cssLibs" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssLibsHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsOwn() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "jsOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::jsOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsLibs() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "jsLibs" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::jsLibsHandler, this,
							std::placeholders::_1)));
	return resource;
}

SeqAppRestbed::SeqAppRestbed(const Json::Value & config) :
		config_(config), seqs_(std::make_shared<seqCache>()){
	checkConfigThrow();
	//load js and css
	jsAndCss_.emplace("jsLibs", getLibFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("jsOwn", getOwnFiles(config_["js"].asString(), ".js"));
	jsAndCss_.emplace("cssLibs", getLibFiles(config_["css"].asString(), ".css"));
	jsAndCss_.emplace("cssOwn", getOwnFiles(config_["css"].asString(), ".css"));
	//set root name
	rootName_ = config_["name"].asString();
	debug_ = config_["debug"].asBool();


}

std::vector<std::shared_ptr<restbed::Resource>> SeqAppRestbed::getAllResources() {
	std::vector<std::shared_ptr<restbed::Resource>> ret;
	ret.emplace_back(jsOwn());
	ret.emplace_back(jsLibs());
	ret.emplace_back(cssLibs());
	ret.emplace_back(cssOwn());

	ret.emplace_back(getProteinColors());
	ret.emplace_back(getDNAColors());
	ret.emplace_back(getColors());

	ret.emplace_back(sort());
	ret.emplace_back(muscleAln());
	ret.emplace_back(translateToProtein());
	ret.emplace_back(complementSeqs());
	ret.emplace_back(minTreeDataDetailed());
	ret.emplace_back(removeGaps());

	return ret;
}

SeqAppRestbed::~SeqAppRestbed() {

}

VecStr SeqAppRestbed::requiredOptions() const {
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
		std::shared_ptr<restbed::Session> session) const {
	uint32_t num = 0;
	const auto request = session->get_request();
	request->get_path_parameter("number", num);
	auto body = ColorFactory::getColors(num).toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);

}

std::shared_ptr<restbed::Resource> SeqAppRestbed::getDNAColors() const {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "baseColors" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getDNAColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::getProteinColors() const {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "proteinColors" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getProteinColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::getColors() const {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "getColors" }, {
			"number", UrlPathFactory::pat_nums_ } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::getColorsHandler, this,
							std::placeholders::_1)));
	return resource;
}

void SeqAppRestbed::addPages(const bfs::path & dir) {
	auto files = bib::files::listAllFiles(dir.string(), false,
			VecStr { ".html" });
	for (const auto & file : files) {
		if (bfs::is_regular_file(file.first)
				&& bib::endsWith(file.first.string(), ".html")) {
			auto base = bfs::path(bfs::basename(file.first));
			base.replace_extension("");
			if (bib::in(base.string(), pages_)) {
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



std::vector<uint32_t> parseJsonForSelected(const Json::Value & postData) {
	std::vector<uint32_t> selected { };
	if (postData.isMember("selected")) {
		selected = bib::json::jsonArrayToVec(postData["selected"],
				std::function<uint32_t(const Json::Value &)>(
						[](const Json::Value & val)->uint32_t {return val.asUInt();}));
	}
	return selected;
}

void SeqAppRestbed::sortPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	const std::string sortBy = request->get_path_parameter("sortBy");
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->sort(uid, sortBy);
			}else{
				seqData = seqs_->sort(uid, selected, sortBy);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::muscleAlnPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->muscle(uid);
			}else{
				seqData = seqs_->muscle(uid, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}


void SeqAppRestbed::removeGapsPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->removeGaps(uid);
			}else{
				seqData = seqs_->removeGaps(uid, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::complementSeqsPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->rComplement(uid);
			}else{
				seqData = seqs_->rComplement(uid, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::translateToProteinPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
	uint32_t start =  bib::lexical_cast<uint32_t>(postData["start"].asString());
  bool complement = false;
  bool reverse = false;
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->translate(uid,           complement, reverse, start);
			}else{
				seqData = seqs_->translate(uid, selected, complement, reverse, start);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::minTreeDataDetailedPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::string uid = postData["uid"].asString();
  uint32_t numDiffs = bib::lexical_cast<uint32_t>(postData["numDiff"].asString());
	Json::Value seqData;
	if(seqs_->containsRecord(uid)){
		if(seqs_->recordValid(uid)){
			if(selected.empty()){
				seqData = seqs_->minTreeDataDetailed(uid, numDiffs);
			}else{
				seqData = seqs_->minTreeDataDetailed(uid, selected, numDiffs);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
		}else{
			std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
		}
	}else{
		std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
	}

	auto retBody = seqData.toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::sortHandler(std::shared_ptr<restbed::Session> session) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	//std::string sortBy
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::sortPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));
}

void SeqAppRestbed::muscleAlnHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::muscleAlnPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));

}

void SeqAppRestbed::removeGapsHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::removeGapsPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));

}

void SeqAppRestbed::complementSeqsHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::complementSeqsPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));

}

void SeqAppRestbed::translateToProteinHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::translateToProteinPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));

}

void SeqAppRestbed::minTreeDataDetailedHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::minTreeDataDetailedPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));

}


std::shared_ptr<restbed::Resource> SeqAppRestbed::sort(){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "sort" }, {"sortBy", UrlPathFactory::pat_word_} }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::sortHandler, this,
							std::placeholders::_1)));
	return resource;

} //std::string sortBy

std::shared_ptr<restbed::Resource> SeqAppRestbed::muscleAln(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "muscle" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::muscleAlnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::removeGaps(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "removeGaps" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::removeGapsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::complementSeqs(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "complement" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::complementSeqsHandler, this,
							std::placeholders::_1)));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqAppRestbed::translateToProtein() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "translate" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::translateToProteinHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::minTreeDataDetailed() {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"minTreeDataDetailed" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::minTreeDataDetailedHandler, this,
							std::placeholders::_1)));
	return resource;
}




}  // namespace bibseq
