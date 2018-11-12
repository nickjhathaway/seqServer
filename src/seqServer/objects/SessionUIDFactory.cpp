/*
 * SessionUIDFactor.cpp
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
#include "SessionUIDFactory.hpp"

namespace njhseq {

void SessionUIDFactory::removeSessionUID(uint32_t sesUID){
	std::unique_lock<std::shared_timed_mutex> lock(mut_);
	if(!njh::in(sesUID, sessionUids_)){
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << ": Error, SessionUIDFactor  doesn't contain session uid " << sesUID << "\n";
		throw std::runtime_error{ss.str()};
	}
	removeElement(sessionUids_, sesUID);
}

uint32_t SessionUIDFactory::genSessionUID(){
	std::unique_lock<std::shared_timed_mutex> lock(mut_);
	uint32_t sesUID = gen_.unifRand<uint32_t>(0, std::numeric_limits<uint32_t>::max());
	while(njh::in(sesUID, sessionUids_)){
		sesUID = gen_.unifRand<uint32_t>(0, std::numeric_limits<uint32_t>::max());
	}
	sessionUids_.emplace_back(sesUID);
	return sesUID;
}

const std::vector<uint32_t> & SessionUIDFactory::getUIDs(){
	std::shared_lock<std::shared_timed_mutex> lock(mut_);
	return sessionUids_;
}

bool SessionUIDFactory::hasSessionUID(uint32_t sessionUID){
	std::shared_lock<std::shared_timed_mutex> lock(mut_);
	return njh::in(sessionUID, sessionUids_);
}



}  // namespace njhseq
