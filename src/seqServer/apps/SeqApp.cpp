/*
 * SeqApp.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "SeqApp.hpp"

namespace bibseq {

void SeqApp::checkConfigThrow() const {
	VecStr missing;
	for (const auto & required : requiredOptions()) {
		if (debug_) {
			std::cout << "Checking for required config option: " << required
					<< std::endl;
		}
		if (!config_.isMember(required)) {
			missing.emplace_back(required);
		}
	}
	if (!missing.empty()) {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__
				<< ": Error, missing the following required options: "
				<< bib::conToStr(missing, ", ") << "\n";
		ss << "given options are: " << bib::conToStr(config_.getMemberNames(), ", ")
				<< "\n";
		throw std::runtime_error { ss.str() };
	}
}

void SeqApp::cssHandler(std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	std::string body = cssFiles_->get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtCssHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqApp::jsHandler(std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	std::string body = jsFiles_->get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtJavascriptHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqApp::css() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "css" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> session) {
						cssHandler(session);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqApp::js() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "js" } }));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> session) {
						jsHandler(session);
					}));
	return resource;
}

SeqApp::SeqApp(const Json::Value & config) :
		config_(config) {
	checkConfigThrow();
	//load js
	auto jsFiles = getLibFiles(
			bib::files::make_path(config_["seqServerCore"].asString(), "js").string(),
			".js");
	addOtherVec(jsFiles,
			getOwnFiles(
					bib::files::make_path(config_["seqServerCore"].asString(), "js").string(),
					".js"));
	jsFiles_ = std::make_unique<bib::files::FilesCache>(jsFiles);
	//load css
	auto cssFiles =
			getLibFiles(
					bib::files::make_path(config_["seqServerCore"].asString(), "css").string(),
					".css");
	addOtherVec(cssFiles,
			getOwnFiles(
					bib::files::make_path(config_["seqServerCore"].asString(), "css").string(),
					".css"));
	cssFiles_ = std::make_unique<bib::files::FilesCache>(cssFiles);


	auto fontsDir = bib::files::make_path(config_["seqServerCore"].asString(), "fonts");
	auto fontsDirFiles = bib::files::filesInFolder(fontsDir);
	for(const auto & f : fontsDirFiles){
		fonts_.emplace(f.filename().string(), f);
	}

	//set root name
	rootName_ = config_["name"].asString();
	debug_ = config_["debug"].asBool();

	seqs_ = std::make_shared<SeqCache>(config_["workingDir"].asString());

	messFac_ = std::make_shared<LogMessageFactory>(std::cout, debug_);
}

std::vector<std::shared_ptr<restbed::Resource>> SeqApp::getAllResources() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	std::vector<std::shared_ptr<restbed::Resource>> ret;
	ret.emplace_back(js());
	ret.emplace_back(css());

	ret.emplace_back(getProteinColors());
	ret.emplace_back(getDNAColors());
	ret.emplace_back(getColors());

	//add fonts
	for(const auto & fontFile : fonts_){
		auto resource = std::make_shared<restbed::Resource>();
		resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
				{ "fonts" }, { fontFile.first} }));
		resource->set_method_handler("GET",
				std::function<void(const std::shared_ptr<restbed::Session>)>(
						[this,&fontFile](const std::shared_ptr<restbed::Session> & ses) {
							const auto request = ses->get_request();
							ses->close(restbed::OK, fonts_.at(fontFile.first).get());
						}));
		ret.emplace_back(resource);
	}

	ret.emplace_back(sort());
	ret.emplace_back(muscleAln());
	ret.emplace_back(translateToProtein());
	ret.emplace_back(complementSeqs());
	ret.emplace_back(minTreeDataDetailed());
	ret.emplace_back(removeGaps());
	ret.emplace_back(countBases());

	ret.emplace_back(deleteSeqs());



	ret.emplace_back(openSession());
	ret.emplace_back(closeSession());

	return ret;
}




SeqApp::~SeqApp() {

}

VecStr SeqApp::requiredOptions() const {
	return VecStr { "seqServerCore", "name", "workingDir" };
}

void SeqApp::getDNAColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::DNAColorsJson);
	session->close(restbed::OK, ColorFactory::DNAColorsJson, headers);
}

void SeqApp::getProteinColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(ColorFactory::AAColorsJson);
	session->close(restbed::OK, ColorFactory::AAColorsJson, headers);
}

void SeqApp::getColorsHandler(
		std::shared_ptr<restbed::Session> session) const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	uint32_t num = 0;
	const auto request = session->get_request();
	request->get_path_parameter("number", num);
	auto body = ColorFactory::getColors(num).toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);

}

std::shared_ptr<restbed::Resource> SeqApp::getDNAColors() const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
			{ "baseColors" } }));
	resource->set_method_handler("GET",
			std::function<void(const std::shared_ptr<restbed::Session>)>(
					[this](const std::shared_ptr<restbed::Session> & ses) {
						getDNAColorsHandler(ses);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqApp::getProteinColors() const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"proteinColors" } }));
	resource->set_method_handler("GET",
			std::function<void(const std::shared_ptr<restbed::Session>)>(
					[this](const std::shared_ptr<restbed::Session> & ses) {
						getProteinColorsHandler(ses);
					}));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqApp::getColors() const {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
			{ "getColors" }, { "number", UrlPathFactory::pat_nums_ } }));
	resource->set_method_handler("GET",
			std::function<void(const std::shared_ptr<restbed::Session>)>(
					[this](const std::shared_ptr<restbed::Session> & ses) {
						getColorsHandler(ses);
					}));
	return resource;
}

//

void SeqApp::addScripts(const bfs::path & dir) {
	auto files = bib::files::listAllFiles(dir.string(), false,
			{ std::regex(".*.js$") });
	for (const auto & file : files) {
		if (bfs::is_regular_file(file.first)) {
			if (bib::in(file.first.filename().string(), pages_)) {
				pages_.erase(file.first.filename().string());
			}
			pages_.emplace(file.first.filename().string(), bfs::absolute(file.first));
		}
	}
}

void SeqApp::addPages(const bfs::path & dir) {
	auto files = bib::files::listAllFiles(dir.string(), false,
			VecStr { ".html" });
	for (const auto & file : files) {
		if (bfs::is_regular_file(file.first)
				&& bib::endsWith(file.first.string(), ".html")) {
			if (bib::in(file.first.filename().string(), pages_)) {
				pages_.erase(file.first.filename().string());
			}
			pages_.emplace(file.first.filename().string(), bfs::absolute(file.first));
		}
	}
}

void SeqApp::sortPostHandler(std::shared_ptr<restbed::Session> session,
		const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	const std::string sortBy = request->get_path_parameter("sortBy");
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqData = seqsBySession_[sessionUID]->sort(uid, sortBy);
			} else {
				seqData = seqsBySession_[sessionUID]->sort(uid,positions, selected, sortBy);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}

	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::muscleAlnPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqData = seqsBySession_[sessionUID]->muscle(uid);
			} else {
				seqData = seqsBySession_[sessionUID]->muscle(uid, positions, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}
	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::removeGapsPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqData = seqsBySession_[sessionUID]->removeGaps(uid);
			} else {
				seqData = seqsBySession_[sessionUID]->removeGaps(uid,positions, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}
	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::complementSeqsPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value seqData;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqData = seqsBySession_[sessionUID]->rComplement(uid);
			} else {
				seqData = seqsBySession_[sessionUID]->rComplement(uid,positions, selected);
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}

	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::translateToProteinPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	uint32_t start = bib::lexical_cast<uint32_t>(postData["start"].asString());
	bool complement = false;
	bool reverse = false;
	Json::Value seqData;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqData = seqsBySession_[sessionUID]->translate(uid, complement,
						reverse, start);
			} else {
				seqData = seqsBySession_[sessionUID]->translate(uid, positions,
						complement, reverse, start);
				//seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["baseColor"] = bib::json::parse(ColorFactory::AAColorsJson);
			seqData["uid"] = uid + "_protein";
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}

	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}





void SeqApp::countBasesPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value ret;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (positions.empty()) {
				ret =  seqsBySession_[sessionUID]->countBases(uid);
			} else {
				ret =  seqsBySession_[sessionUID]->countBases(uid, positions);
			}
			ret["uid"] = uid;
			ret["sessionUID"] = bib::json::toJson(sessionUID);
			ret["positions"] = bib::json::toJson(positions);
			ret["selected"] = bib::json::toJson(selected);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}
	auto retBody = bib::json::writeAsOneLine(ret);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::deleteSeqsPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	Json::Value ret;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (positions.empty()) {
				//positions to delete is empty, nothing to delete
				ret["uid"] = uid;
				ret["sessionUID"] = bib::json::toJson(sessionUID);
				ret["positions"] = bib::json::toJson(positions);
				ret["selected"] = bib::json::toJson(selected);
			} else {
				seqsBySession_[sessionUID]->deleteSeqs(uid, positions);
				ret = seqsBySession_[sessionUID]->getJson(uid);
			}
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}
	auto retBody = bib::json::writeAsOneLine(ret);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}




void SeqApp::minTreeDataDetailedPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	std::vector<uint32_t> selected = parseJsonForSelected(postData);
	std::vector<uint32_t> positions = parseJsonForPosition(postData);
	const std::string uid = postData["uid"].asString();
	const uint32_t sessionUID = postData["sessionUID"].asUInt();
	const uint32_t gapOpenPen = postData["gapOpenPen"].asUInt();
	const uint32_t gapExtPen = postData["gapExtPen"].asUInt();
	const int32_t match = postData["match"].asInt();
	const int32_t mismatch = postData["mismatch"].asInt();
	const uint32_t numThreads = postData["numThreads"].asUInt();
	//uint32_t numThreads = 2;
	uint32_t numDiffs = bib::lexical_cast<uint32_t>(postData["numDiff"].asString());
	Json::Value seqData;
	std::unordered_map<std::string, std::unique_ptr<aligner>> aligners;
	std::mutex alignerLock;
	uint64_t maxSize = 0;
	if (bib::in(sessionUID, seqsBySession_)) {
		if (seqsBySession_[sessionUID]->containsRecord(uid)) {
			if (selected.empty()) {
				seqsBySession_[sessionUID]->getMaxLen(uid, maxSize);
			}else{
				seqsBySession_[sessionUID]->getMaxLen(uid, positions, maxSize);
			}
			aligner alignerObj(maxSize, gapScoringParameters(gapOpenPen, gapExtPen),
									substituteMatrix(match, mismatch));
			if (selected.empty()) {
				/**@todo add alignment caching */
				seqData = seqsBySession_[sessionUID]->minTreeDataDetailed(uid, numDiffs,
						alignerObj, aligners, alignerLock, numThreads);

				table infoTab(VecStr {"Comparison", "var1-name", "var2-name", "type", "var1-pos",
						"var1-seq", "var1-qual", "var2-pos", "var2-seq", "var2-qual",
						"totalDiffs" });
				for (const auto & node : seqData["nodes"]) {
					if ("indel" == node["type"].asString()) {
						infoTab.addRow(node["ref"].asString() + "-vs-" + node["seq"].asString(),
								node["ref"], node["seq"], node["type"],
								node["refPos"], node["refDisplay"], "NA", node["seqPos"],
								node["seqDisplay"], "NA",
								seqData["totalDiffs"][node["ref"].asString()
										+ node["seq"].asString()]);
					}
					if ("snp" == node["type"].asString()) {
						infoTab.addRow(node["ref"].asString() + "-vs-" + node["seq"].asString(),
								node["ref"], node["seq"], node["type"],
								node["refPos"], node["refBase"], node["refBaseQual"],
								node["seqPos"], node["seqBase"], node["seqBaseQual"],
								seqData["totalDiffs"][node["ref"].asString()
										+ node["seq"].asString()]);
					}
				}
				seqData["infoTab"] = tableToJsonByRow(infoTab, "Comparison");
			} else {
				seqData = seqsBySession_[sessionUID]->minTreeDataDetailed(uid,
						positions, numDiffs, alignerObj, aligners, alignerLock, numThreads);
				table infoTab(VecStr{"Comparison", "var1-name", "var2-name","type", "var1-pos","var1-seq", "var1-qual", "var2-pos", "var2-seq", "var2-qual", "totalDiffs"});
				for(const auto & node : seqData["nodes"]){
					if("indel" == node["type"].asString()){
						infoTab.addRow(node["ref"].asString() + "-vs-" + node["seq"].asString(),
								node["ref"], node["seq"], node["type"],
								node["refPos"], node["refDisplay"],"NA",
								node["seqPos"], node["seqDisplay"],"NA",
						seqData["totalDiffs"][node["ref"].asString() + node["seq"].asString()]);
					}
					if("snp" == node["type"].asString()){
						infoTab.addRow(node["ref"].asString() + "-vs-" + node["seq"].asString(),
								node["ref"], node["seq"], node["type"],
								node["refPos"], node["refBase"],node["refBaseQual"],
								node["seqPos"], node["seqBase"],node["seqBaseQual"],
								seqData["totalDiffs"][node["ref"].asString() + node["seq"].asString()]);
					}
				}
				seqData["infoTab"] = tableToJsonByRow(infoTab, "Comparison");
				seqData["selected"] = bib::json::toJson(selected);
			}
			seqData["uid"] = uid;
			seqData["sessionUID"] = bib::json::toJson(sessionUID);
		} else {
			std::cerr << "uid: " << uid << " is not currently in cache" << std::endl;
		}
	} else {
		std::cerr << "sessionUID: " << sessionUID
				<< " is not currently in seqsBySession_" << std::endl;
	}

	auto retBody = bib::json::writeAsOneLine(seqData);
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::sortHandler(std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	//std::string sortBy
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						sortPostHandler(ses, body);
					}));
}

void SeqApp::muscleAlnHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						muscleAlnPostHandler(ses, body);
					}));
}

void SeqApp::removeGapsHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						removeGapsPostHandler(ses, body);
					}));
}

void SeqApp::complementSeqsHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						complementSeqsPostHandler(ses, body);
					}));

}

void SeqApp::translateToProteinHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						translateToProteinPostHandler(ses, body);
					}));

}

void SeqApp::minTreeDataDetailedHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						minTreeDataDetailedPostHandler(ses, body);
					}));
}

void SeqApp::countBasesHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
							countBasesPostHandler(ses, body);
					}));
}

void SeqApp::deleteSeqsHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
							deleteSeqsPostHandler(ses, body);
					}));
}




/*
 * 	void (std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
			void (std::shared_ptr<restbed::Session> session);
			std::shared_ptr<restbed::Resource> ()
 */

std::shared_ptr<restbed::Resource> SeqApp::sort() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "sort" }, {
			"sortBy", UrlPathFactory::pat_word_ } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						sortHandler(ses);
					}));

	return resource;

}

std::shared_ptr<restbed::Resource> SeqApp::countBases() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "countBases" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
		countBasesHandler(ses);
					}));

	return resource;

}

std::shared_ptr<restbed::Resource> SeqApp::deleteSeqs() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, { "deleteSeqs" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
		deleteSeqsHandler(ses);
					}));

	return resource;

}






std::shared_ptr<restbed::Resource> SeqApp::muscleAln() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "muscle" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						muscleAlnHandler(ses);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqApp::removeGaps() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
			{ "removeGaps" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						removeGapsHandler(ses);
					}));

	return resource;
}
std::shared_ptr<restbed::Resource> SeqApp::complementSeqs() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
			{ "complement" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						complementSeqsHandler(ses);
					}));
	return resource;
}
std::shared_ptr<restbed::Resource> SeqApp::translateToProtein() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, { "translate" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						translateToProteinHandler(ses);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqApp::minTreeDataDetailed() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"minTreeDataDetailed" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						minTreeDataDetailedHandler(ses);
					}));
	return resource;
}

void SeqApp::closeSessionPostHandler(
		std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto postData = bib::json::parse(std::string(body.begin(), body.end()));
	uint32_t sessionUID = postData["sessionUID"].asUInt();
	try {
		sesUIDFac_.removeSessionUID(sessionUID);
		if (debug_) {
			std::cout << "Removed: " << sessionUID << std::endl;
			std::cout << "IDs left: " << std::endl;
			printVector(sesUIDFac_.getUIDs());
			if (bib::in(sessionUID, seqsBySession_)) {
				if(bib::beginsWith(seqsBySession_[sessionUID]->workingDir_.string(), "/tmp/")){
					bib::files::rmDirForce(seqsBySession_[sessionUID]->workingDir_.string());
				}
				seqsBySession_.erase(sessionUID);
			}
		}
	} catch (std::exception & e) {
		std::cerr << bib::bashCT::red << e.what() << bib::bashCT::reset
				<< std::endl;
	}

	auto retBody = bib::json::toJson(sesUIDFac_.getUIDs()).toStyledString();
	std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(retBody);
	headers.emplace("Connection", "close");
	session->close(restbed::OK, retBody, headers);
}

void SeqApp::closeSessionHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	const auto request = session->get_request();
	auto heads = request->get_headers();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	session->fetch(content_length,
			std::function<
					void(std::shared_ptr<restbed::Session>, const restbed::Bytes & body)>(
					[this](std::shared_ptr<restbed::Session> ses, const restbed::Bytes & body) {
						closeSessionPostHandler(ses, body);
					}));
}

uint32_t SeqApp::startSeqCacheSession() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto ret = sesUIDFac_.genSessionUID();
	seqsBySession_[ret] = std::make_unique<SeqCache>(*seqs_);
	auto sesDir = bib::files::make_path(seqs_->workingDir_, "seqServerSes_ses-" + estd::to_string(ret));
	bib::files::makeDirP(bib::files::MkdirPar(sesDir.string()));
	seqsBySession_[ret]->setWorkingDir(sesDir);
	return ret;
}

void SeqApp::openSessionHandler(
		std::shared_ptr<restbed::Session> session) {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	Json::Value ret;
	ret["sessionUID"] = startSeqCacheSession();
	auto body = ret.toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqApp::closeSession() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ }, {
			"closeSession" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						closeSessionHandler(ses);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqApp::openSession() {
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(UrlPathFactory::createUrl( { { rootName_ },
			{ "openSession" } }));
	resource->set_method_handler("POST",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> ses) {
						openSessionHandler(ses);
					}));
	return resource;
}

std::string SeqApp::genHtmlDoc(std::string rName,
		bib::files::FileCache & cache) {
	bib::lstrip(rName, '/');
	std::string header = "<!DOCTYPE HTML>\n"
			"<html>\n"
			"	<meta charset=\"utf-8\">\n"
			"  <head>\n"
			"		<script type=\"text/javascript\" src=\"/" + rName + "/js\"></script>\n"
			"		<link rel=\"stylesheet\" type=\"text/css\" href=\"/" + rName + "/css\">\n"
			"   <style>\n"
			"     @font-face {\n"
			"       font-family: 'Glyphicons Halflings';\n"
			"       src: url('/" + rName + "/fonts/glyphicons-halflings-regular.eot');\n"
			"       src: url('/" + rName + "/fonts/glyphicons-halflings-regular.eot?#iefix') format('embedded-opentype'), url('/" + rName + "/fonts/glyphicons-halflings-regular.woff') format('woff'), url('/" + rName + "/fonts/glyphicons-halflings-regular.ttf') format('truetype'), url('/" + rName + "/fonts/glyphicons-halflings-regular.svg#glyphicons-halflingsregular') format('svg');\n"
			"     }\n"
			"			@font-face{\n"
			"				font-family:'FontAwesome';\n"
			"				src:url('/" + rName + "/fonts/fontawesome-webfont.eot?v=4.7.0');\n"
			"				src:url('/" + rName + "/fonts/fontawesome-webfont.eot?#iefix&v=4.7.0') format('embedded-opentype'),url('/" + rName + "/fonts/fontawesome-webfont.woff2?v=4.7.0') format('woff2'),url('/" + rName + "/fonts/fontawesome-webfont.woff?v=4.7.0') format('woff'),url('/" + rName + "/fonts/fontawesome-webfont.ttf?v=4.7.0') format('truetype'),url('/" + rName + "/fonts/fontawesome-webfont.svg?v=4.7.0#fontawesomeregular') format('svg');\n"
			"			}\n"
			"  </style>\n"
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
