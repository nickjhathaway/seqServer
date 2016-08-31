#pragma once
/*
 * SessionUIDFactor.hpp
 *
 *  Created on: Aug 30, 2016
 *      Author: nick
 */



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
