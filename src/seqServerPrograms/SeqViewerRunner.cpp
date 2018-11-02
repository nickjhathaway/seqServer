/*
 * SeqViewer.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */
//
// seqServer - A library for analyzing sequence data
// Copyright (C) 2012-2018 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
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
#include "SeqViewerRunner.hpp"

#include "seqServer/apps/pars.h"

namespace bibseq {


SeqViewer::SeqViewer(const Json::Value & config) : SeqApp(config){

	protein_ = config_["protein"].asBool();
	pages_.emplace("mainPageJs",
					bib::files::make_path(config["resources"], "ssv/ssv.js"));

	//read in data and set to the json
	seqs_->addToCache(rootName_.substr(1), SeqIOOptions(config["ioOptions"].asString()));
	if(debug_){
		std::cout << "Finished set up" << std::endl;
	}
}


void SeqViewer::mainPageHandler(std::shared_ptr<restbed::Session> session){
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto body = genHtmlDoc(rootName_, pages_.at("mainPageJs"));
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateTxtHtmlHeader(body);
	session->close(restbed::OK, body, headers);
}

void SeqViewer::seqDataHandler(std::shared_ptr<restbed::Session> session){
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
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

	auto body = bib::json::writeAsOneLine(seqData);
	const std::multimap<std::string, std::string> headers =
			HeaderFactory::initiateAppJsonHeader(body);
	session->close(restbed::OK, body, headers);
}

std::shared_ptr<restbed::Resource> SeqViewer::seqData(){
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }, {"seqData"}}));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> session) {
						seqDataHandler(session);
					}));
	return resource;
}

std::shared_ptr<restbed::Resource> SeqViewer::mainPage(){
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path(
			UrlPathFactory::createUrl( { { rootName_ }}));
	resource->set_method_handler("GET",
			std::function<void(std::shared_ptr<restbed::Session>)>(
					[this](std::shared_ptr<restbed::Session> session) {
						mainPageHandler(session);
					}));
	return resource;
}


std::vector<std::shared_ptr<restbed::Resource>> SeqViewer::getAllResources(){
	auto mess = messFac_->genLogMessage(__PRETTY_FUNCTION__);
	auto ret = super::getAllResources();
	ret.emplace_back(mainPage());
	ret.emplace_back(seqData());
	return ret;
}


VecStr SeqViewer::requiredOptions() const{
	return concatVecs(super::requiredOptions(), VecStr{"resources", "ioOptions"});
}

void error_handler(const int statusCode, const std::exception& exception,
		const std::shared_ptr<restbed::Session>& session) {
	std::cerr << "statusCode: " << statusCode << std::endl;
	std::cerr << exception.what() << std::endl;
	if(session != nullptr && session->is_open()){
		session->close(statusCode, exception.what(), { { "Server", "Restbed" } });
	}
}

SeqViewerRunner::SeqViewerRunner()
    : bib::progutils::ProgramRunner(
    		std::map<std::string, funcInfo>{
					 addFunc("seqViewer", SeqViewerRunner::RunSeqViewer, false)
           },//
          "SeqViewerRunner") {}


int SeqViewerRunner::RunSeqViewer(const bib::progutils::CmdArgs & inputCommands){
	std::string clusDir = "";
	SeqAppCorePars corePars;
	std::string bindAddress  ="127.0.0.1";
	corePars.port_ = 8881;
	corePars.name_ = "ssv";
	std::string resourceDirName = bib::files::make_path(seqServer::getSeqServerInstallDir(),
			"etc/resources").string();
	bool protein = false;
	bibseq::seqSetUp setUp(inputCommands);
	setUp.processDebug();
	setUp.processVerbose();
	setUp.setOption(bindAddress, "--bindAddress", "Bind Address");
	setUp.setOption(protein, "--protein", "Viewing Protein");
	setUp.setOption(resourceDirName, "--resourceDirName",
			"Name of the resource Directory where the js and html is located",
			!bfs::exists(resourceDirName));
	bib::appendAsNeeded(resourceDirName, "/");
	setUp.processReadInNames(true);

	corePars.setCoreOptions(setUp);
	setUp.description_ = "Start an HTML viewer on a sequence file";
	setUp.examples_.emplace_back("MASTERPROGRAM SUBPROGRAM --fasta input.fasta");
	setUp.examples_.emplace_back("MASTERPROGRAM SUBPROGRAM --fastq input.fastq --port 8882 --name ssv2");
	setUp.finishSetUp(std::cout);
	bib::files::checkExistenceThrow(setUp.pars_.ioOptions_.firstName_, __PRETTY_FUNCTION__);
	if(bfs::is_directory(setUp.pars_.ioOptions_.firstName_)){
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << ", error " << setUp.pars_.ioOptions_.firstName_ << " is a directory not a file " << "\n";
		throw std::runtime_error{ss.str()};
	}
  //
  Json::Value appConfig;
  corePars.addCoreOpts(appConfig);
  auto optsJson = setUp.pars_.ioOptions_.toJson();
  appConfig["ioOptions"] =  bib::json::toJson(optsJson.toStyledString());
  appConfig["resources"] = bib::json::toJson(resourceDirName);
  appConfig["protein"] = bib::json::toJson(protein);

  if(setUp.pars_.verbose_){
    std::cout <<  corePars.getAddress()<< std::endl;
  }

	SeqViewer viewerModel(appConfig);

	auto resources = viewerModel.getAllResources();

	auto settings = std::make_shared<restbed::Settings>();

	settings->set_port(corePars.port_);
	//settings->set_root(name);
	settings->set_bind_address(bindAddress);
	settings->set_default_header("Connection", "close");
	settings->set_worker_limit(4);
	restbed::Service service;
	service.set_error_handler(error_handler);
	for (const auto & resource : resources) {
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
