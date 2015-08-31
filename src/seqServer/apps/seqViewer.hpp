#pragma once
//
// seqServer - A library for visualizing sequence results data
// Copyright (C) 2012, 2015 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
// Jeffrey Bailey <Jeffrey.Bailey@umassmed.edu>
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
/*
 * seqViewer.hpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "seqServer/apps/seqApp.hpp"


namespace bibseq {

class ssv: public bibseq::seqApp {
private:

	typedef bibseq::seqApp super;

	std::string rootName_;


	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	ssv(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const ;

	void seqData();

	void rootName();

	void showMinTree();

	void mainPage();

};

int seqViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


