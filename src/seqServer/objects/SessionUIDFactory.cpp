/*
 * SessionUIDFactor.cpp
 *
 *  Created on: Aug 30, 2016
 *      Author: nick
 */


#include "SessionUIDFactory.hpp"

namespace bibseq {

void SessionUIDFactory::removeSessionUID(uint32_t sesUID){
	std::unique_lock<std::shared_timed_mutex> lock(mut_);
	if(!bib::in(sesUID, sessionUids_)){
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << ": Error, SessionUIDFactor  doesn't contain session uid " << sesUID << "\n";
		throw std::runtime_error{ss.str()};
	}
	removeElement(sessionUids_, sesUID);
}

uint32_t SessionUIDFactory::genSessionUID(){
	std::unique_lock<std::shared_timed_mutex> lock(mut_);
	uint32_t sesUID = gen_.unifRand<uint32_t>(0, std::numeric_limits<uint32_t>::max());
	while(bib::in(sesUID, sessionUids_)){
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
	return bib::in(sessionUID, sessionUids_);
}



}  // namespace bibseq
