#pragma once
/*
 * SeqViewerRestbed.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "seqServer.h"



namespace bibseq {

class SeqViewer: public SeqApp {

	typedef bibseq::SeqApp super;

	bool protein_;

	void seqDataHandler(std::shared_ptr<restbed::Session> session);
	void mainPageHandler(std::shared_ptr<restbed::Session> session);

public:

	SeqViewer(const Json::Value & config);


	virtual VecStr requiredOptions() const ;

	std::shared_ptr<restbed::Resource> seqData();
	std::shared_ptr<restbed::Resource> mainPage();

	virtual std::vector<std::shared_ptr<restbed::Resource>> getAllResources();


};

class SeqViewerRunner : public bib::progutils::ProgramRunner {
 public:
	SeqViewerRunner();

	static int RunSeqViewer(const bib::progutils::CmdArgs & inputCommands);

};






} /* namespace bibseq */

