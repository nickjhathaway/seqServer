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
 * seqCache.hpp
 *
 *  Created on: May 05, 2015
 *      Author: nickhathaway
 */

#include "seqServer/utils.h"

namespace bibseq {

class seqCache {
public:

	class cacheRecord {
	public:

		cacheRecord(const std::string & uid,
				const std::shared_ptr<std::vector<readObject>> & reads) :
				uid_(uid), reads_(reads) {
		}
		std::string uid_;
		std::shared_ptr<std::vector<readObject>> reads_;

	};

	std::unordered_map<std::string, cacheRecord > cache_;

	VecStr currentCache_;
	uint32_t cachePos_ = 0;
	uint32_t cacheSizeLimit_ = 10;

	bool recordValid(const std::string & uid)const;
	bool containsRecord(const std::string & uid)const;

	void addToCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads);
	void updateCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads);
	void updateAddCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads);

	std::shared_ptr<std::vector<readObject>> getRecord(const std::string & uid);

	/*These currently assume cache is valid and is current in cache_ */
	cppcms::json::value sort(const std::string & uid, const std::string & sortOption);
	cppcms::json::value muscle(const std::string & uid);
	cppcms::json::value removeGaps(const std::string & uid);
	cppcms::json::value rComplement(const std::string & uid);
	cppcms::json::value minTreeData(const std::string & uid);
	cppcms::json::value getJson(const std::string & uid);
};

class seqToJsonFactory{
public:
	template<typename T>
	static cppcms::json::value seqsToJson(const std::vector<T> & reads, const std::string & uid){
		//std::cout << "seqsToJson: start" << std::endl;
	  cppcms::json::value ret;
	  auto& seqs = ret["seqs"];
	  //find number of reads
	  ret["numReads"] = reads.size();
	  // get the maximum length
	  uint64_t maxLen = 0;
	  bibseq::readVec::getMaxLength(reads, maxLen);
	  ret["maxLen"] = maxLen;
	  ret["uid"] = uid;
	  for(const auto & pos : iter::range(reads.size())){
	  	seqs[pos]= jsonToCppcmsJson(bib::json::toJson(reads[pos].seqBase_));
	  }
	  //std::cout << "seqsToJson: stop" << std::endl;
	  return ret;
	}

	template<typename T>
	static cppcms::json::value sort(const std::shared_ptr<std::vector<T>> & reads, const std::string & sortOption, const std::string & uid){
		readVecSorter::sortReadVector(*reads, sortOption);
		return seqsToJson(*reads, uid);
	}
	template<typename T>
	static cppcms::json::value muscle(const std::shared_ptr<std::vector<T>> & reads, const std::string & uid){
		bib::for_each(*reads, [](readObject & read){ read.seqBase_.removeGaps();});
		sys::muscleSeqs(*reads);
		return seqsToJson(*reads, uid);
	}

	template<typename T>
	static cppcms::json::value removeGaps(const std::shared_ptr<std::vector<T>> & reads, const std::string & uid){
		bib::for_each(*reads, [](readObject & read){ read.seqBase_.removeGaps();});
		return seqsToJson(*reads, uid);
	}

	template<typename T>
	static cppcms::json::value rComplement(const std::shared_ptr<std::vector<T>> & reads, const std::string & uid){
		readVec::allReverseComplement(*reads, true);
		return seqsToJson(*reads, uid);
	}

	template<typename T>
	static cppcms::json::value minTreeData(const std::shared_ptr<std::vector<T>> & reads, const std::string & uid){
		/**@todo need to complete function */
		throw std::runtime_error{"seqToJsonFactory::minTreeData not yet implemented"};
		return seqsToJson(*reads, uid);
	}



};

} /* namespace bibseq */




