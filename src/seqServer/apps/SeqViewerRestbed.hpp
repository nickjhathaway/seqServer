#pragma once
/*
 * SeqViewerRestbed.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "seqServer/apps/SeqAppRestbed.hpp"


namespace bibseq {

class SeqViewerRestbed: public SeqAppRestbed {

	typedef bibseq::SeqAppRestbed super;

	bool protein_;

	void seqDataHandler(std::shared_ptr<restbed::Session> session);
	void mainPageHandler(std::shared_ptr<restbed::Session> session);

public:

	SeqViewerRestbed(const Json::Value & config);


	virtual VecStr requiredOptions() const ;

	std::shared_ptr<restbed::Resource> seqData();
	std::shared_ptr<restbed::Resource> mainPage();

	virtual std::vector<std::shared_ptr<restbed::Resource>> getAllResources();


};



int seqViewerRestbed(const bib::progutils::CmdArgs & inputCommands);

} /* namespace bibseq */

