/*
 * SeqViewerRestbed.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "SeqViewerRestbed.hpp"

namespace bibseq {


SeqViewerRestbed::SeqViewerRestbed(const Json::Value & config) : SeqAppRestbed(config){

	protein_ = config_["protein"].asBool();
	pages_.emplace("mainPageHtml",
				bib::files::make_path(config["resources"], "ssv/mainPage.html"));

	//read in data and set to the json
	SeqIOOptions options(config["ioOptions"].asString());
	SeqInput reader(options);
	reader.openIn();
	auto reads = reader.readAllReads<readObject>();
	seqs_->addToCache(rootName_.substr(1), std::make_shared<std::vector<readObject>>(reads));
	if(debug_){
		std::cout << "Finished set up" << std::endl;
	}
}


void SeqViewerRestbed::mainPageHandler(std::shared_ptr<restbed::Session> session){
	auto body = pages_.at("mainPageHtml").get();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtHtmlHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqViewerRestbed::seqDataHandler(std::shared_ptr<restbed::Session> session){
	auto sesUid = startSeqCacheSession();

	auto seqData = seqsBySession_[sesUid]->getJson(rootName_.substr(1));
	seqData["sessionUID"] = bib::json::toJson(sesUid);
	if(protein_){
		seqData["seqType"] = bib::json::toJson("protein");
		seqData["baseColor"] = bib::json::parse(ColorFactory::AAColorsJson);
	}else{
		seqData["seqType"] = bib::json::toJson("dna");
		seqData["baseColor"] = bib::json::parse(ColorFactory::DNAColorsJson);
	}

	auto body = seqData.toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqViewerRestbed::seqTypeHandler(std::shared_ptr<restbed::Session> session){
	Json::Value r;
	if(protein_){
		r = "protein";
	}else{
		r = "dna";
	}
	auto body = r.toStyledString();
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);
}


std::shared_ptr<restbed::Resource> SeqViewerRestbed::seqData(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, {"seqData"}}));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqViewerRestbed::seqDataHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqViewerRestbed::mainPage(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }}));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqViewerRestbed::mainPageHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqViewerRestbed::seqType(){
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, {"seqType"}}));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					std::bind(&SeqViewerRestbed::seqTypeHandler, this,
							std::placeholders::_1)));
	return resource;
}

std::vector<std::shared_ptr<restbed::Resource>> SeqViewerRestbed::getAllResources(){
	auto ret = super::getAllResources();
	ret.emplace_back(mainPage());
	ret.emplace_back(seqData());
	ret.emplace_back(seqType());
	return ret;
}


VecStr SeqViewerRestbed::requiredOptions() const{
	return catenateVectors(super::requiredOptions(), VecStr{"resources", "ioOptions"});
}

void error_handler(const int statusCode, const std::exception& exception,
		const std::shared_ptr<restbed::Session>& session) {
	std::cerr << "statusCode: " << statusCode << std::endl;
	std::cerr << exception.what() << std::endl;
	if(session->is_open()){
		session->close(statusCode, exception.what(), { { "Server", "Restbed" } });
	}
}

int seqViewerRestbed(const bib::progutils::CmdArgs & inputCommands){
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "ssv";
	std::string resourceDirName = bib::files::make_path(seqServer::getSeqServerInstallDir(),
			"etc/resources").string();
	bool protein = false;
	bibseq::seqSetUp setUp(inputCommands);
	setUp.setOption(protein, "--protein", "Viewing Protein");
	setUp.setOption(resourceDirName, "-resourceDirName",
			"Name of the resource Directory where the js and hmtl is located",
			!bfs::exists(resourceDirName));
	bib::appendAsNeeded(resourceDirName, "/");
	setUp.processDefaultReader(true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.processDebug();
	setUp.processVerbose();
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  //
  Json::Value appConfig;

  appConfig["name"] =  bib::json::toJson(name);
  appConfig["port"] = bib::json::toJson( estd::to_string(port));
  auto optsJson = setUp.pars_.ioOptions_.toJson();
  appConfig["ioOptions"] =  bib::json::toJson(optsJson.toStyledString());
  appConfig["resources"] = bib::json::toJson(resourceDirName);
  appConfig["js"] = bib::json::toJson(resourceDirName + "js/");
  appConfig["css"] = bib::json::toJson(resourceDirName + "css/");
  appConfig["debug"] =  bib::json::toJson(setUp.pars_.debug_);
  appConfig["protein"] = bib::json::toJson(protein);

  if(setUp.pars_.verbose_){
    std::cout << "localhost:"  << port << name << std::endl;
  }

	SeqViewerRestbed viewerModel(appConfig);

	auto resources = viewerModel.getAllResources();

	auto settings = std::make_shared<restbed::Settings>();
	settings->set_port(port);
	//settings->set_root(name);
	settings->set_default_header("Connection", "close");

	restbed::Service service;
	service.set_error_handler(error_handler);
	for(const auto & resource : resources){
		service.publish(resource);
	}
	try {
		service.start(settings);
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


} /* namespace bibseq */
