#pragma once
/*
 * SeqToJsonFactory.hpp
 *
 *  Created on: Aug 15, 2016
 *      Author: nick
 */

#include "seqServer/utils.h"

namespace bibseq {

class SeqToJsonFactory {
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
			seqs[pos]["position"] = pos;
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
						std::string(__PRETTY_FUNCTION__) + ": Error , out of range, pos: "
								+ estd::to_string(pos) + ", size: "
								+ estd::to_string(reads.size()) };
			}
			readVec::getMaxLength(reads[pos], maxLen);
		}
		ret["maxLen"] = bib::json::toJson(maxLen);
		ret["uid"] = uid;
		ret["selected"] = bib::json::toJson(positions);
		uint32_t count = 0;
		for (const auto & pos : positions) {
			seqs[count] = bib::json::toJson(getSeqBase(reads[pos]));
			seqs[count]["position"] = pos;
			++count;
		}
		return ret;
	}

	template<typename T>
	static Json::Value sort(std::vector<T> & reads,
			const std::string & sortOption, const std::string & uid) {
		readVecSorter::sortReadVector(reads, sortOption);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value sort(std::vector<T> & reads,
			const std::string & sortOption,
			std::vector<uint32_t> selected,
			const std::string & uid) {
		bib::sort(selected);
		readVecSorter::sortReadVector(reads, selected, sortOption);

		return seqsToJson(reads, selected, uid);
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
		return seqsToJson(reads, selected, uid);
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
		return seqsToJson(reads,selected, uid);
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
		return seqsToJson(reads,selected, uid);
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
		auto jsonRet = seqsToJson(ret, uid);
		jsonRet["seqType"] = bib::json::toJson("protein");
		return jsonRet;
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

