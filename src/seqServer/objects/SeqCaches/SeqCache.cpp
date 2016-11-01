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
 * utils.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "SeqCache.hpp"
#include "seqServer/objects/SeqToJsonFactory.hpp"


namespace bibseq {

SeqCache::CacheRecord::CacheRecord(const std::string & uid,
		const SeqIOOptions & ioOpts) :
		uid_(uid), ioOpts_(ioOpts) {
}

SeqCache::CacheRecord::CacheRecord(const CacheRecord & other) :

		uid_(other.uid_),
		ioOpts_(other.ioOpts_.opts_),
		reads_(nullptr == other.reads_ ? nullptr : std::make_shared<std::vector<readObject>>(*other.reads_)),

		blockSize_(other.blockSize_),
		blockStart_(other.blockStart_),
		updateFunc_(other.updateFunc_){
	ioOpts_.setTime(other.ioOpts_.getTime());
}

SeqCache::CacheRecord::CacheRecord(CacheRecord && other) :
		uid_(other.uid_),
		ioOpts_(other.ioOpts_),
		reads_(std::move(other.reads_)),
		blockSize_(other.blockSize_),
		blockStart_(other.blockStart_),
		updateFunc_(std::move(other.updateFunc_)){
	ioOpts_.setTime(other.ioOpts_.getTime());
}

void SeqCache::CacheRecord::readsToNull(){
	reads_ = nullptr;
}

bool SeqCache::CacheRecord::readsAreNull(){
	return nullptr == reads_;
}

void SeqCache::CacheRecord::setBlockSize(const uint32_t blockSize) {
	blockSize_ = blockSize;
}

uint32_t SeqCache::CacheRecord::getBlockSize() const {
	return blockSize_;
}

uint32_t SeqCache::CacheRecord::getBlockStart() const {
	return blockStart_;
}
void SeqCache::CacheRecord::addUpdateFunc(std::function<void(CacheRecord &)> updateFunc){
	updateFunc_ = updateFunc;
}

void SeqCache::CacheRecord::reload(bool force){
	if(nullptr == reads_ || ioOpts_.outDated() || force){
		reads_ = std::make_shared<std::vector<readObject>>(ioOpts_.get<readObject>(blockStart_, blockSize_));
		if(nullptr != updateFunc_){
			updateFunc_(*this);
		}
	}
}

std::shared_ptr<std::vector<readObject>> SeqCache::CacheRecord::get(){
	reload();
	return reads_;
}

std::shared_ptr<std::vector<readObject>> SeqCache::CacheRecord::get(uint32_t blockStart){
	if(blockStart_ != blockStart){
		setBlockSize(blockStart);
		reads_ = nullptr;
	}
	return get();
}


void SeqCache::CacheRecord::sort(const std::string & sortOption){
	reload();
	readVecSorter::sortReadVector(*reads_, sortOption);
}

void SeqCache::CacheRecord::muscle(){
	reload();
	std::vector<uint32_t> positions;
	for(const auto pos : iter::range(reads_->size())){
		if((*reads_)[pos].seqBase_.on_){
			positions.emplace_back(pos);
		}
	}
	bib::for_each_pos(*reads_,positions, [](readObject & read) {getSeqBase(read).removeGaps();});
	sys::muscleSeqs(*reads_, positions);
}

void SeqCache::CacheRecord::removeGaps(){
	reload();
	bib::for_each(*reads_, [](readObject & read) {getSeqBase(read).removeGaps();});
}

void SeqCache::CacheRecord::rComplement(){
	reload();
	bib::for_each(*reads_,
			[]( readObject & read) {getSeqBase(read).reverseComplementRead(true,true);});
}

void SeqCache::CacheRecord::sort(const std::string & sortOption,
		std::vector<uint32_t> positions) {
	reload();
	bib::sort(positions);
	readVecSorter::sortReadVector(*reads_, positions, sortOption);
}

void SeqCache::CacheRecord::muscle(const std::vector<uint32_t> & positions) {
	reload();
	bib::for_each_pos(*reads_, positions,
			[](readObject & read) {getSeqBase(read).removeGaps();});
	sys::muscleSeqs(*reads_, positions);
}

void SeqCache::CacheRecord::removeGaps(
		const std::vector<uint32_t> & positions) {
	reload();
	bib::for_each_pos(*reads_, positions,
			[](readObject & read) {getSeqBase(read).removeGaps();});
}

void SeqCache::CacheRecord::rComplement(
		const std::vector<uint32_t> & positions) {
	reload();
	bib::for_each_pos(*reads_, positions,
			[]( readObject & read) {getSeqBase(read).reverseComplementRead(true,true);});
}

void SeqCache::CacheRecord::erase(std::vector<uint32_t> positions){
	reload();
	std::sort(positions.rbegin(), positions.rend());
	for(const auto & pos : positions){
		reads_->erase(reads_->begin() + pos);
	}
}

void SeqCache::CacheRecord::ensureNonEmptyReads() {
	uint32_t count = 0;
	if (nullptr != reads_) {
		for (const auto & seq : *reads_) {
			if (seq.seqBase_.on_) {
				++count;
			}
		}
		if (0 == count) {
			reads_->emplace_back(seqInfo { "No Seqs", "AACCGGTT" });
		} else {
			if ("No Seqs" == reads_->back().seqBase_.name_) {
				reads_->erase(reads_->end() - 1);
			}
		}
	}
}


void SeqCache::CacheRecord::toggleSeqs(std::function<bool(const readObject &)> func){
	reload();
	for(auto & seq : *reads_){
		if(func(seq)){
			seq.seqBase_.on_ = true;
		}else{
			seq.seqBase_.on_ = false;
		}
	}
}


SeqCache::SeqCache(const bfs::path & workingDir):workingDir_(workingDir) {

}

SeqCache::SeqCache(const SeqCache & other) :
		workingDir_(other.workingDir_),
		cache_(other.cache_),
		currentCache_(other.currentCache_),
		cachePos_(other.cachePos_) {
}

SeqCache::SeqCache(SeqCache && other) :
		workingDir_(other.workingDir_),
		cache_(std::move(other.cache_)),
		currentCache_(std::move(other.currentCache_)),
		cachePos_(other.cachePos_) {
}

SeqCache::~SeqCache(){

}

Json::Value SeqCache::getJson(const std::string & uid){
	return SeqToJsonFactory::seqsToJson(getRef((cache_.at(uid).get())), uid);
}

Json::Value SeqCache::sort(const std::string & uid, const std::string & sortOption){
	cache_.at(uid).sort(sortOption);
	return getJson(uid);
}

Json::Value SeqCache::muscle(const std::string & uid){
	cache_.at(uid).muscle();
	return getJson(uid);
}

Json::Value SeqCache::removeGaps(const std::string & uid){
	cache_.at(uid).removeGaps();
	return getJson(uid);
}

Json::Value SeqCache::rComplement(const std::string & uid) {
	cache_.at(uid).rComplement();
	return getJson(uid);
}

Json::Value SeqCache::minTreeDataDetailed(const std::string & uid,
		uint32_t numDiff) {
	return SeqToJsonFactory::minTreeDataDetailed(getRef((cache_.at(uid).get())),
			uid, numDiff);
}

Json::Value SeqCache::minTreeDataDetailed(const std::string & uid,
		const std::vector<uint32_t> & positions, uint32_t numDiff) {
	return SeqToJsonFactory::minTreeDataDetailed(getRef((cache_.at(uid).get())),
			positions, uid, numDiff);
}

Json::Value SeqCache::sort(const std::string & uid,
		const std::vector<uint32_t> & positions,
		const std::vector<uint32_t> & selected, const std::string & sortOption) {
	cache_.at(uid).sort(sortOption, positions);
	return getJson(uid, positions, selected);
}

Json::Value SeqCache::muscle(const std::string & uid,
		const std::vector<uint32_t> & positions,
		const std::vector<uint32_t> & selected) {
	cache_.at(uid).muscle(positions);
	return getJson(uid, positions, selected);
}

Json::Value SeqCache::removeGaps(const std::string & uid,
		const std::vector<uint32_t> & positions,
		const std::vector<uint32_t> & selected) {
	cache_.at(uid).removeGaps(positions);
	return getJson(uid, positions, selected);
}

Json::Value SeqCache::rComplement(const std::string & uid,
		const std::vector<uint32_t> & positions,
		const std::vector<uint32_t> & selected) {
	cache_.at(uid).rComplement(positions);
	return getJson(uid, positions, selected);
}

Json::Value SeqCache::getJson(const std::string & uid,
		const std::vector<uint32_t> & positions,
		const std::vector<uint32_t> & selected){
	return SeqToJsonFactory::seqsToJson(getRef((cache_.at(uid).get())), positions, selected, uid);
}

Json::Value SeqCache::translate(const std::string & uid,
		const std::vector<uint32_t> & positions, bool complement, bool reverse,
		uint64_t start){
	auto ret = SeqToJsonFactory::translate(getRef((cache_.at(uid).get())), positions, uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : ret["seqs"]){
		proteins->emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		proteins->back().seqBase_.cnt_ = j["cnt"].asDouble();
		proteins->back().seqBase_.frac_ = j["frac"].asDouble();
	}
	auto proteinFilename = bib::files::make_path(workingDir_, uid + "_protein");
	auto proteinOpts = SeqIOOptions::genFastaInOut(
			proteinFilename.string() + ".fasta", proteinFilename.string() + ".fasta",
			cache_.at(uid).ioOpts_.opts_.processed_);
	proteinOpts.out_.overWriteFile_ = true;
	SeqOutput::write(*proteins, proteinOpts);
	updateAddCache(uid + "_protein", proteinOpts);
	return ret;
}

Json::Value SeqCache::translate(const std::string & uid, bool complement,
		bool reverse, uint64_t start){
	auto ret = SeqToJsonFactory::translate(getRef((cache_.at(uid).get())), uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : ret["seqs"]){
		proteins->emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		proteins->back().seqBase_.cnt_ = j["cnt"].asDouble();
		proteins->back().seqBase_.frac_ = j["frac"].asDouble();
	}
	auto proteinFilename = bib::files::make_path(workingDir_, uid + "_protein");
	auto proteinOpts = SeqIOOptions::genFastaInOut(
			proteinFilename.string() + ".fasta", proteinFilename.string() + ".fasta",
			cache_.at(uid).ioOpts_.opts_.processed_);
	proteinOpts.out_.overWriteFile_ = true;
	SeqOutput::write(*proteins, proteinOpts);
	updateAddCache(uid + "_protein", proteinOpts);
	return ret;
}

void SeqCache::setWorkingDir(const bfs::path & dir){
	workingDir_ = dir;
}

bool SeqCache::containsRecordNoLock(const std::string & uid){
	return cache_.find(uid) != cache_.end();
}

void SeqCache::addToCacheNoCheck(const std::string & uid,
		const SeqIOOptions & ioOpts){
	cache_.emplace(uid, CacheRecord(uid, ioOpts));
	/*
	if (cachePos_ < currentCache_.size()) {
		cache_.at(currentCache_[cachePos_]).reads_ = nullptr;
		currentCache_[cachePos_] = uid;
		++cachePos_;
		if (cachePos_ >= cacheSizeLimit_) {
			cachePos_ = 0;
		}
	} else {
		++cachePos_;
		currentCache_.emplace_back(uid);
	}*/
}

void SeqCache::updateCacheNoCheck(const std::string & uid,
		const SeqIOOptions & ioOpts){
	cache_.erase(uid);
	addToCacheNoCheck(uid, ioOpts);
	/*
	if (!bib::in(uid, currentCache_)) {
		if (cachePos_ < currentCache_.size()) {
			cache_.at(currentCache_[cachePos_]).reads_ = nullptr;
			currentCache_[cachePos_] = uid;
			++cachePos_;
			if (cachePos_ >= cacheSizeLimit_) {
				cachePos_ = 0;
			}
		}
	}*/
}

void SeqCache::addToCache(const std::string & uid,
		const SeqIOOptions & ioOpts) {
	std::unique_lock <std::shared_timed_mutex> lock(mut_);
	//check to see if cache already exists
	if (containsRecordNoLock(uid)) {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << std::endl;
		std::cerr << "Cache already contains uid: " << uid
				<< ", should call update instead" << std::endl;
		throw std::runtime_error{ss.str()};
	} else {
		addToCacheNoCheck(uid, ioOpts);
	}
}

SeqCache::CacheRecord & SeqCache::getRecord(const std::string & uid){
	std::shared_lock<std::shared_timed_mutex> lock(mut_);
	auto ret = cache_.find(uid);
	ret->second.reload();
	return ret->second;
}

void SeqCache::updateAddCache(const std::string & uid,
		const SeqIOOptions & ioOpts) {
	std::unique_lock <std::shared_timed_mutex> lock(mut_);
	if (containsRecordNoLock(uid)) {
		updateCacheNoCheck(uid, ioOpts);
	} else {
		addToCacheNoCheck(uid, ioOpts);
	}
}

void SeqCache::updateCache(const std::string & uid,
		const SeqIOOptions & ioOpts) {
	std::unique_lock <std::shared_timed_mutex> lock(mut_);
	// check to see if cache exists
	if (containsRecordNoLock(uid)) {
		updateCacheNoCheck(uid, ioOpts);
	} else {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << std::endl;
		ss << "Cache doesn't contain uid: " << uid << ", nothing to update"
				<< std::endl;
		throw std::runtime_error { ss.str() };
	}
}

bool SeqCache::containsRecord(const std::string & uid) {
	std::shared_lock<std::shared_timed_mutex> lock(mut_);
	return containsRecordNoLock(uid);
}



} /* namespace bibseq */
