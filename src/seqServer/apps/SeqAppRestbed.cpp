/*
 * SeqAppRestbed.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "SeqAppRestbed.hpp"

namespace bibseq {

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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	std::string body = jsAndCss_.find("cssOwn")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::cssLibsHandler(std::shared_ptr<restbed::Session> session) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	std::string body = jsAndCss_.find("cssLibs")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsOwnHandler(std::shared_ptr<restbed::Session> session) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	std::string body = jsAndCss_.find("jsOwn")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqAppRestbed::jsLibsHandler(std::shared_ptr<restbed::Session> session) {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	std::string body = jsAndCss_.find("jsLibs")->second.get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssOwn() {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "cssOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::cssLibs() {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "cssLibs" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::cssLibsHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsOwn() {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "jsOwn" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::jsOwnHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::jsLibs() {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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

	ret.emplace_back(openSession());
	ret.emplace_back(closeSession());

	return ret;
}

SeqAppRestbed::~SeqAppRestbed() {

}

VecStr SeqAppRestbed::requiredOptions() const {
	return VecStr { "js", "css", "name" };
}

void SeqAppRestbed::getDNAColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::DNAColorsJson);
	session->close(restbed::OK, ColorFactory::DNAColorsJson, headers);
}

void SeqAppRestbed::getProteinColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::AAColorsJson);
	session->close(restbed::OK, ColorFactory::AAColorsJson, headers);
}

void SeqAppRestbed::getColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	uint32_t num = 0;
	const auto request = session->get_request();
	request->get_path_parameter("number", num);
	auto body = ColorFactory::getColors(num).toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);

}

std::shared_ptr<restbed::Resource> SeqAppRestbed::getDNAColors() const {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->sort(uid,           sortBy);
				}else{
					seqData = seqsBySession_[sessionUID]->sort(uid, selected, sortBy);
					seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["uid"] = uid;
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	if(debug_){
		std::cout << postData << std::endl;
	}
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->muscle(uid);
				}else{
					seqData = seqsBySession_[sessionUID]->muscle(uid, selected);
					seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["uid"] = uid;
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->removeGaps(uid);
				}else{
					seqData = seqsBySession_[sessionUID]->removeGaps(uid, selected);
					seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["uid"] = uid;
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->rComplement(uid);
				}else{
					seqData = seqsBySession_[sessionUID]->rComplement(uid, selected);
					seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["uid"] = uid;
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	uint32_t start =  bib::lexical_cast<uint32_t>(postData["start"].asString());
  bool complement = false;
  bool reverse = false;
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->translate(uid,           complement, reverse, start);
				}else{
					seqData = seqsBySession_[sessionUID]->translate(uid, selected, complement, reverse, start);
					//seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["baseColor"] = bib::json::parse(ColorFactory::AAColorsJson);
				seqData["uid"] = uid + "_protein";
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
  uint32_t numDiffs = bib::lexical_cast<uint32_t>(postData["numDiff"].asString());
	Json::Value seqData;
	if(bib::in(sessionUID, seqsBySession_)){
		if(seqsBySession_[sessionUID]->containsRecord(uid)){
			if(seqsBySession_[sessionUID]->recordValid(uid)){
				if(selected.empty()){
					seqData = seqsBySession_[sessionUID]->minTreeDataDetailed(uid, numDiffs);
				}else{
					seqData = seqsBySession_[sessionUID]->minTreeDataDetailed(uid, selected, numDiffs);
					seqData["selected"] = bib::json::toJson(selected);
				}
				seqData["uid"] = uid;
				seqData["sessionUID"] = bib::json::toJson(sessionUID);
			}else{
				std::cerr << "uid: " << uid << " is not currently valid" << std::endl;
			}
		}else{
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	}else{
		std::cerr << "sessionUID: " << sessionUID << " is not currently in seqsBySession_" << std::endl;
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

}

std::shared_ptr<restbed::Resource> SeqAppRestbed::muscleAln(){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
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
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"minTreeDataDetailed" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::minTreeDataDetailedHandler, this,
							std::placeholders::_1)));
	return resource;
}


void SeqAppRestbed::closeSessionPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	uint32_t sessionUID = postData["sessionUID"].asUInt();

	sesUIDFac_.removeSessionUID(sessionUID);
	if(debug_){
		std::cout << "Removed: " << sessionUID << std::endl;
		std::cout << "IDs left: " << std::endl;
		printVector(sesUIDFac_.getUIDs());
	}

	if(bib::in(sessionUID, seqsBySession_)){
		seqsBySession_.erase(sessionUID);
	}
	auto retBody = bib::json::toJson(sesUIDFac_.getUIDs()).toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqAppRestbed::closeSessionHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					std::bind(&SeqAppRestbed::closeSessionPostHandler, this,
							std::placeholders::_1, std::placeholders::_2)));
}

uint32_t SeqAppRestbed::startSeqCacheSession() {
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto ret = sesUIDFac_.genSessionUID();
	seqsBySession_[ret] = std::make_unique<seqCache>(*seqs_);
	return ret;
}


void SeqAppRestbed::openSessionHandler(std::shared_ptr<restbed::Session> session){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	Json::Value ret;
	ret["sessionUID"] = startSeqCacheSession();
	auto body = ret.toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::closeSession(){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"closeSession" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::closeSessionHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqAppRestbed::openSession(){
	bib::scopedMessage mess(messStrFactory(__PRETTY_FUNCTION__), std::cout, debug_);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"openSession" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqAppRestbed::openSessionHandler, this,
							std::placeholders::_1)));
	return resource;
}


std::string SeqAppRestbed::genHtmlDoc(std::string rName,
		bib::files::FileCache & cache) {
	bib::lstrip(rName, '/');
	std::string header = "<!DOCTYPE HTML>\n"
			"<html>\n"
			"	<meta charset=\"utf-8\">\n"
			"  <head>\n"
			"		<script type=\"text/javascript\" src=\"/" + rName + "/jsLibs\"></script>\n"
					"		<script type=\"text/javascript\" src=\"/" + rName + "/jsOwn\"></script>\n"
					"		<link rel=\"stylesheet\" type=\"text/css\" href=\"/" + rName + "/cssLibs\">\n"
					"		<link rel=\"stylesheet\" type=\"text/css\" href=\"/" + rName + "/cssOwn\">\n"
					"  </head>\n"
					"  <body>\n"
					"    <script>\n";
	std::string body = cache.get();

	std::string footer = "    </script> \n"
			"  </body>\n"
			"</html>\n";
	return header + body + footer;
}




}  // namespace bibseq
