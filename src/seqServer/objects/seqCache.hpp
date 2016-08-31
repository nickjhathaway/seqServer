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
				const std::shared_ptr<std::vector<readObject>> & reads);
		cacheRecord(const cacheRecord & other);
		cacheRecord(cacheRecord && other);

		std::string uid_;
		std::shared_ptr<std::vector<readObject>> reads_;
		/*
	private:
			uint32_t blockSize_ = 10000;
			uint32_t blockStart_ = 0;
	public:
		void setBlockSize(const uint32_t blockSize) {
			blockSize_ = blockSize;
		}*/

	};
	seqCache();
	seqCache(const seqCache & other);
	seqCache(seqCache && other);
private:

	std::unordered_map<std::string, cacheRecord> cache_;
	VecStr currentCache_;
	uint32_t cachePos_ = 0;
	uint32_t cacheSizeLimit_ = 10;

	std::shared_timed_mutex mut_;

	bool recordValidNoLock(const std::string & uid);
	bool containsRecordNoLock(const std::string & uid);
	void addToCacheNoCheck(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
	void updateCacheNoCheck(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
public:
	bool recordValid(const std::string & uid);
	bool containsRecord(const std::string & uid);

	void addToCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
	void updateCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
	void updateAddCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);

	std::shared_ptr<std::vector<readObject>> getRecord(const std::string & uid);

	/*These currently assume cache is valid and is current in cache_ */
	Json::Value sort(const std::string & uid, const std::string & sortOption);
	Json::Value muscle(const std::string & uid);
	Json::Value removeGaps(const std::string & uid);
	Json::Value rComplement(const std::string & uid);
	Json::Value translate(const std::string & uid, bool complement, bool reverse,
			uint64_t start);
	Json::Value minTreeDataDetailed(const std::string & uid, uint32_t numDiff);
	Json::Value getJson(const std::string & uid);

	Json::Value sort(const std::string & uid,
			const std::vector<uint32_t> & positions, const std::string & sortOption);
	Json::Value muscle(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value removeGaps(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value translate(const std::string & uid,
			const std::vector<uint32_t> & positions, bool complement, bool reverse,
			uint64_t start);
	Json::Value rComplement(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value minTreeDataDetailed(const std::string & uid,
			const std::vector<uint32_t> & positions, uint32_t numDiff);
	Json::Value getJson(const std::string & uid,
			const std::vector<uint32_t> & positions);

};





} /* namespace bibseq */




