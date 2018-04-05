#pragma once
/*
 * SessionUIDFactor.hpp
 *
 *  Created on: Aug 30, 2016
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

#include "seqServer/utils.h"



namespace bibseq {
class SessionUIDFactory {

	bib::randomGenerator gen_;
	std::vector<uint32_t> sessionUids_;
	std::shared_timed_mutex mut_;

public:

	void removeSessionUID(uint32_t sesUID);
	uint32_t genSessionUID();
	const std::vector<uint32_t> & getUIDs();
	bool hasSessionUID(uint32_t sessionUID);

};


}  // namespace bibseq
