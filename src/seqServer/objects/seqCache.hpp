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
private:
	std::unordered_map<std::string, cacheRecord> cache_;
	VecStr currentCache_;
	uint32_t cachePos_ = 0;
	uint32_t cacheSizeLimit_ = 10;
public:
	bool recordValid(const std::string & uid) const;
	bool containsRecord(const std::string & uid) const;

	void addToCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
	void updateCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);
	void updateAddCache(const std::string & uid,
			const std::shared_ptr<std::vector<readObject>> & reads);

	std::shared_ptr<std::vector<readObject>> getRecord(const std::string & uid);

	/*These currently assume cache is valid and is current in cache_ */
	Json::Value sort(const std::string & uid,
			const std::string & sortOption);
	Json::Value muscle(const std::string & uid);
	Json::Value removeGaps(const std::string & uid);
	Json::Value rComplement(const std::string & uid);
	Json::Value translate(const std::string & uid, bool complement,
			bool reverse, uint64_t start);
	Json::Value minTreeData(const std::string & uid);
	Json::Value minTreeDataDetailed(const std::string & uid, uint32_t numDiff);
	Json::Value getJson(const std::string & uid);

	Json::Value muscle(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value removeGaps(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value translate(const std::string & uid,
			const std::vector<uint32_t> & positions, bool complement, bool reverse,
			uint64_t start);
	Json::Value rComplement(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value minTreeData(const std::string & uid,
			const std::vector<uint32_t> & positions);
	Json::Value minTreeDataDetailed(const std::string & uid,
			const std::vector<uint32_t> & positions, uint32_t numDiff);
	Json::Value getJson(const std::string & uid,
			const std::vector<uint32_t> & positions);

};



class seqToJsonFactory {
public:
	template<typename T>
	static Json::Value seqsToJson(const std::vector<T> & reads,
			const std::string & uid) {
		Json::Value ret;
		auto& seqs = ret["seqs"];
		//find number of reads
		ret["numReads"] = bib::json::toJson(reads.size());
		// get the maximum length
		uint64_t maxLen = 0;
		bibseq::readVec::getMaxLength(reads, maxLen);
		ret["maxLen"] = bib::json::toJson(maxLen);
		ret["uid"] = uid;
		ret["selected"] = bib::json::toJson(std::vector<uint32_t> { });
		//seqs.
		for (const auto & pos : iter::range<uint32_t>(reads.size())) {
			seqs[pos] = bib::json::toJson(getSeqBase(reads[pos]));
		}
		return ret;
	}

	template<typename T>
	static Json::Value seqsToJson(const std::vector<T> & reads,
			const std::vector<uint32_t> & positions,
			const std::string & uid) {
		Json::Value ret;
		auto& seqs = ret["seqs"];
		//find number of reads
		ret["numReads"] = bib::json::toJson(positions.size());
		// get the maximum length
		uint64_t maxLen = 0;
		for (auto pos : positions) {
			if (pos >= reads.size()) {
				throw std::out_of_range {
						"Error in bibseq::seqToJsonFactory::seqsToJson, out of range, pos: "
								+ estd::to_string(pos) + ", size: "
								+ estd::to_string(reads.size()) };
			}
			readVec::getMaxLength(reads[pos], maxLen);
		}
		ret["maxLen"] = bib::json::toJson(maxLen);
		ret["uid"] = uid;
		ret["selected"] = bib::json::toJson(std::vector<uint32_t> { });
		for (const auto & pos : positions) {
			seqs[pos] = bib::json::toJson(getSeqBase(reads[pos]));
		}
		return ret;
	}

	template<typename T>
	static Json::Value sort(std::vector<T> & reads,
			const std::string & sortOption, const std::string & uid) {
		if ("reverse" == sortOption) {
			bib::reverse(reads);
		} else {
			readVecSorter::sortReadVector(reads, sortOption);
		}
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value muscle(
			std::vector<T> & reads, const std::string & uid) {
		bib::for_each(reads, [](T & read) {getSeqBase(read).removeGaps();});
		sys::muscleSeqs(reads);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value muscle(
			std::vector<T> & reads,
			const std::vector<uint32_t> & selected, const std::string & uid) {
		bib::for_each_pos(reads, selected,
				[](T & read) {getSeqBase(read).removeGaps();});
		sys::muscleSeqs(reads, selected);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value removeGaps(
			std::vector<T> & reads, const std::string & uid) {
		bib::for_each(reads, [](T & read) {getSeqBase(read).removeGaps();});
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value removeGaps(
			std::vector<T> & reads,
			const std::vector<uint32_t> & selected,
			const std::string & uid) {
		bib::for_each_pos(reads, selected,
				[](T & read) {getSeqBase(read).removeGaps();});
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value rComplement(
			std::vector<T> & reads, const std::string & uid) {
		readVec::allReverseComplement(reads, true);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value rComplement(
			std::vector<T> & reads,
			const std::vector<uint32_t> & selected, const std::string & uid) {
		bib::for_each_pos(reads, selected,
				[]( T & read) {getSeqBase(read).reverseComplementRead(true,true);});
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value translate(
			std::vector<T> & reads,
			const std::vector<uint32_t> & selected, const std::string & uid,
			bool complement, bool reverse, uint64_t start) {
		std::vector<baseReadObject> ret;
		for (const auto & readPos : selected) {
			ret.emplace_back(
					baseReadObject(
							getSeqBase(reads[readPos]).translateRet(complement, reverse,
									start)));
		}
		return seqsToJson(ret, uid);
	}

	template<typename T>
	static Json::Value translate(
			std::vector<T> & reads, const std::string & uid,
			bool complement, bool reverse, uint64_t start) {
		std::vector<uint32_t> positions(reads.size());
		bib::iota<uint32_t>(positions, 0);
		return translate(reads, positions, uid, complement, reverse, start);
	}

	template<typename T>
	static Json::Value minTreeData(
			const std::vector<T> & reads, const std::string & uid) {
		return genMinTreeData(reads);
	}

	template<typename T>
	static Json::Value minTreeData(
			const std::vector<T> & reads,
			const std::vector<uint32_t> & selected, const std::string & uid) {
		std::vector<T> selReads;
		for (const auto & pos : selected) {
			selReads.emplace_back(reads[pos]);
		}
		return genMinTreeData(selReads);
	}

	template<typename T>
	static Json::Value minTreeDataDetailed(
			const std::vector<T> & reads, const std::string & uid,
			uint32_t numDiff) {
		if (numDiff > 0) {
			comparison cutOff;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
			return genDetailMinTreeData(reads, 2, cutOff, true);
		} else {
			return genDetailMinTreeData(reads, 2);
		}
	}

	template<typename T>
	static Json::Value minTreeDataDetailed(
			const std::vector<T> & reads,
			const std::vector<uint32_t> & selected, const std::string & uid,
			uint32_t numDiff) {
		std::vector<T> selReads;
		for (const auto & pos : selected) {
			selReads.emplace_back(reads[pos]);
		}
		if(numDiff > 0){
			comparison cutOff;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
			return genDetailMinTreeData(selReads,2, cutOff, true);
		}else{
			return genDetailMinTreeData(selReads,2);
		}
	}

};

} /* namespace bibseq */




