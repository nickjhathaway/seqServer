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


class SeqCache {
public:

	class CacheRecord {
	public:

		CacheRecord(const std::string & uid, const SeqIOOptions & ioOpts);

		CacheRecord(const CacheRecord & other);
		CacheRecord(CacheRecord && other);

		const std::string uid_;
		SeqIOOptsWithTime ioOpts_;




		std::shared_ptr<std::vector<readObject>> reads_;
	private:
		uint32_t blockSize_ = 10000;
		uint32_t blockStart_ = 0;
		std::function<void(CacheRecord &)> updateFunc_;

	public:

		void readsToNull();

		bool readsAreNull();

		void addUpdateFunc(std::function<void(CacheRecord &)> updateFunc);

		void setBlockSize(const uint32_t blockSize);
		uint32_t getBlockSize() const;
		uint32_t getBlockStart() const;

		std::shared_ptr<std::vector<readObject>> get();
		std::shared_ptr<std::vector<readObject>> get(uint32_t blockStart);

		void reload(bool force = false);


		void sort(const std::string & sortOption);
		void muscle();
		void removeGaps();
		void rComplement();

		void sort(const std::string & sortOption, std::vector<uint32_t> positions);
		void muscle(const std::vector<uint32_t> & positions);
		void removeGaps(const std::vector<uint32_t> & positions);
		void rComplement(const std::vector<uint32_t> & positions);

		void erase(std::vector<uint32_t> positions);

		void ensureNonEmptyReads();

		/**@brief turn seqs one and off with a function, if func returns true seq is turned on, false turned off
		 *
		 * @param func function to control turning off and on seqs
		 */
		void toggleSeqs(std::function<bool(const readObject &)> func);

		friend class SeqCache;
	};

	SeqCache(const bfs::path & workingDir);
	SeqCache(const SeqCache & other);
	SeqCache(SeqCache && other);

	~SeqCache();

	void setWorkingDir(const bfs::path & dir);
	bfs::path workingDir_;
	std::unordered_map<std::string, CacheRecord> cache_;
private:




	VecStr currentCache_;
	uint32_t cachePos_ = 0;
	//uint32_t cacheSizeLimit_ = 10;

	std::shared_timed_mutex mut_;



	bool containsRecordNoLock(const std::string & uid);
	void addToCacheNoCheck(const std::string & uid,
			const SeqIOOptions & ioOpts);
	void updateCacheNoCheck(const std::string & uid,
			const SeqIOOptions & ioOpts);
public:

	bool containsRecord(const std::string & uid);

	void addToCache(const std::string & uid,
			const SeqIOOptions & ioOpts);
	void updateCache(const std::string & uid,
			const SeqIOOptions & ioOpts);
	void updateAddCache(const std::string & uid,
			const SeqIOOptions & ioOpts);

	CacheRecord & getRecord(const std::string & uid);

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
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected,
			const std::string & sortOption);
	Json::Value muscle(const std::string & uid,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected);
	Json::Value removeGaps(const std::string & uid,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected);
	Json::Value translate(const std::string & uid,
			const std::vector<uint32_t> & positions, bool complement, bool reverse,
			uint64_t start);
	Json::Value rComplement(const std::string & uid,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected);
	Json::Value minTreeDataDetailed(const std::string & uid,
			const std::vector<uint32_t> & positions, uint32_t numDiff);
	Json::Value getJson(const std::string & uid,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected);

};





} /* namespace bibseq */




