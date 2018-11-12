#pragma once
/*
 * SeqViewerRestbed.hpp
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
#include "seqServer.h"



namespace njhseq {

class SeqViewer: public SeqApp {

	typedef njhseq::SeqApp super;

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

class SeqViewerRunner : public njh::progutils::ProgramRunner {
 public:
	SeqViewerRunner();

	static int RunSeqViewer(const njh::progutils::CmdArgs & inputCommands);

};






} /* namespace njhseq */

