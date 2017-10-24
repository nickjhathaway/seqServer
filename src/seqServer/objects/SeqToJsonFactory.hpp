#pragma once
/*
 * SeqToJsonFactory.hpp
 *
 *  Created on: Aug 15, 2016
 *      Author: nick
 */

#include "seqServer/utils.h"
#include "seqServer/objects/ColorFactory.hpp"


namespace bibseq {

class SeqToJsonFactory {
public:

	template<typename T>
	static Json::Value seqsToJson(const std::vector<T> & reads,
			const std::string & uid) {
		Json::Value ret;
		auto& seqs = ret["seqs"];
		uint64_t maxLen = 0;
		ret["uid"] = uid;
		ret["selected"] = bib::json::toJson(std::vector<uint32_t> { });
		uint32_t count = 0;
		for (const auto & pos : iter::range<uint32_t>(reads.size())) {
			if(getSeqBase(reads[pos]).on_){
				bibseq::readVec::getMaxLength(getSeqBase(reads[pos]), maxLen);
				seqs[count] = bib::json::toJson(getSeqBase(reads[pos]));
				seqs[count]["position"] = pos;
				seqs[count]["selected"] = count;
				++count;
			}
		}
		//find number of reads
		ret["numReads"] = bib::json::toJson(count);
		// get the maximum length
		ret["maxLen"] = bib::json::toJson(maxLen);
		//default seq type is dna @todo could check seq alphabet to determine this
		ret["seqType"] = bib::json::toJson("dna");
		ret["baseColor"] = bib::json::parse(ColorFactory::DNAColorsJson);
		return ret;
	}

	template<typename T>
	static Json::Value seqsToJson(const std::vector<T> & reads,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected,
			const std::string & uid) {
		Json::Value ret;
		auto& seqs = ret["seqs"];
		//find number of reads
		ret["numReads"] = bib::json::toJson(positions.size());
		// get the maximum length and make sure pos is not greater than read vec size
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
		if(selected.size() != positions.size()){
			std::stringstream ss;
			ss << __PRETTY_FUNCTION__ << ": error, size of positions and size of selected aren't equal" << "\n";
			ss << "size of positions: " << positions.size() << ", size of selected: " << selected.size() << "\n";
			throw std::runtime_error{ss.str()};
		}
		ret["maxLen"] = bib::json::toJson(maxLen);
		ret["uid"] = uid;
		ret["selected"] = bib::json::toJson(selected);
		ret["positions"] = bib::json::toJson(positions);
		uint32_t count = 0;
		for (const auto & posIndex : iter::range(positions.size())) {
			seqs[count] = bib::json::toJson(getSeqBase(reads[positions[posIndex]]));
			seqs[count]["position"] = positions[posIndex];
			seqs[count]["selected"] = selected[posIndex];
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
			const std::string & sortOption, std::vector<uint32_t> positions,
			std::vector<uint32_t> selected, const std::string & uid) {
		bib::sort(selected);
		readVecSorter::sortReadVector(reads, positions, sortOption);
		return seqsToJson(reads, positions, selected, uid);
	}

	template<typename T>
	static Json::Value muscle(
			std::vector<T> & reads, const std::string & uid) {
		bib::for_each(reads, [](T & read) {getSeqBase(read).removeGaps();});
		Muscler musclerOperator;
		musclerOperator.muscleSeqs(reads);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value muscle(
			std::vector<T> & reads,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected,
			const std::string & uid) {
		bib::for_each_pos(reads, positions,
				[](T & read) {getSeqBase(read).removeGaps();});
		Muscler musclerOperator;
		musclerOperator.muscleSeqs(reads, positions);
		return seqsToJson(reads, positions, selected, uid);
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
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected,
			const std::string & uid) {
		bib::for_each_pos(reads, positions,
				[](T & read) {getSeqBase(read).removeGaps();});
		return seqsToJson(reads, positions, selected, uid);
	}

	template<typename T>
	static Json::Value rComplement(
			std::vector<T> & reads, const std::string & uid) {
		readVec::allReverseComplement(reads, true);
		return seqsToJson(reads, uid);
	}

	template<typename T>
	static Json::Value rComplement(std::vector<T> & reads,
			const std::vector<uint32_t> & positions,
			const std::vector<uint32_t> & selected, const std::string & uid) {
		bib::for_each_pos(reads, positions,
				[]( T & read) {getSeqBase(read).reverseComplementRead(true,true);});
		return seqsToJson(reads, positions, selected, uid);
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
			const std::vector<uint32_t> & positions,
			const std::string & uid,
			bool complement, bool reverse, uint64_t start) {
		std::vector<baseReadObject> ret;
		for (const auto & readPos : positions) {
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
			const std::vector<T> & reads,
			const std::string & uid,
			uint32_t numDiff) {
		std::vector<T> selReads;
		for (const auto & seq : reads) {
			if(getSeqBase(seq).on_){
				selReads.emplace_back(seq);
			}
		}
		if (numDiff > 0) {
			comparison cutOff;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
			return genDetailMinTreeData(selReads, 2, cutOff, true);
		} else {
			return genDetailMinTreeData(selReads, 2);
		}
	}

	template<typename T>
	static Json::Value minTreeDataDetailed(const std::vector<T> & reads,
			const std::vector<uint32_t> & positions, const std::string & uid,
			uint32_t numDiff) {
		std::vector<T> selReads;
		for (const auto & pos : positions) {
			selReads.emplace_back(reads[pos]);
		}
		if (numDiff > 0) {
			comparison cutOff;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
			return genDetailMinTreeData(selReads, 2, cutOff, true);
		} else {
			return genDetailMinTreeData(selReads, 2);
		}
	}

	template<typename T>
	static Json::Value minTreeDataDetailed(
			const std::vector<T> & reads,
			const std::string & uid,
			aligner & alignerObj,
			std::unordered_map<std::string, std::unique_ptr<aligner>>& aligners,
			std::mutex & alignerLock,
			uint32_t numThreads,
			uint32_t numDiff,
			bool justBest) {
		std::vector<T> selReads;
		for (const auto & seq : reads) {
			if(getSeqBase(seq).on_){
				selReads.emplace_back(seq);
			}
		}
		comparison cutOff;
		bool settingLimits = false;
		if (numDiff > 0) {
			settingLimits = true;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
		}
		bool doTies = true;
		return genDetailMinTreeData(selReads, alignerObj, aligners, alignerLock,
				numThreads, cutOff, settingLimits, justBest, doTies);
	}

	template<typename T>
	static Json::Value minTreeDataDetailed(const std::vector<T> & reads,
			const std::vector<uint32_t> & positions, const std::string & uid,
			aligner & alignerObj,
			std::unordered_map<std::string, std::unique_ptr<aligner>>& aligners,
			std::mutex & alignerLock,
			uint32_t numThreads,
			uint32_t numDiff,
			bool justBest) {
		std::vector<T> selReads;
		for (const auto & pos : positions) {
			selReads.emplace_back(reads[pos]);
		}
		comparison cutOff;
		bool settingLimits = false;
		if (numDiff > 0) {
			settingLimits = true;
			cutOff.distances_.overLappingEvents_ = numDiff + 1;
		}
		bool doTies = true;
		return genDetailMinTreeData(selReads, alignerObj, aligners, alignerLock,
				numThreads, cutOff, settingLimits, justBest, doTies);
	}

};


} /* namespace bibseq */

