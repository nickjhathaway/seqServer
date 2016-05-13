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

#include "seqCache.hpp"

namespace bibseq {



Json::Value seqCache::getJson(const std::string & uid){
	return seqToJsonFactory::seqsToJson(getRef((cache_.at(uid).reads_)), uid);
}

Json::Value seqCache::sort(const std::string & uid, const std::string & sortOption){
	return seqToJsonFactory::sort(getRef((cache_.at(uid).reads_)), sortOption, uid);
}

Json::Value seqCache::muscle(const std::string & uid){
	return seqToJsonFactory::muscle(getRef((cache_.at(uid).reads_)), uid);
}

Json::Value seqCache::removeGaps(const std::string & uid){
	return seqToJsonFactory::removeGaps(getRef((cache_.at(uid).reads_)), uid);
}

Json::Value seqCache::rComplement(const std::string & uid){
	return seqToJsonFactory::rComplement(getRef((cache_.at(uid).reads_)), uid);
}

Json::Value seqCache::minTreeData(const std::string & uid){
	return seqToJsonFactory::minTreeData(getRef((cache_.at(uid).reads_)), uid);
}

Json::Value seqCache::minTreeData(const std::string & uid, const std::vector<uint32_t> & positions){
	return seqToJsonFactory::minTreeData(getRef((cache_.at(uid).reads_)),positions, uid);
}

Json::Value seqCache::minTreeDataDetailed(const std::string & uid, uint32_t numDiff){
	return seqToJsonFactory::minTreeDataDetailed(getRef((cache_.at(uid).reads_)), uid, numDiff);
}

Json::Value seqCache::minTreeDataDetailed(const std::string & uid, const std::vector<uint32_t> & positions, uint32_t numDiff){
	return seqToJsonFactory::minTreeDataDetailed(getRef((cache_.at(uid).reads_)),positions, uid, numDiff);
}

Json::Value seqCache::muscle(const std::string & uid,const std::vector<uint32_t> & positions){
	return seqToJsonFactory::muscle(getRef((cache_.at(uid).reads_)), positions, uid);
}
Json::Value seqCache::removeGaps(const std::string & uid, const std::vector<uint32_t> & positions){
	return seqToJsonFactory::removeGaps(getRef((cache_.at(uid).reads_)), positions, uid);
}
Json::Value seqCache::rComplement(const std::string & uid, const std::vector<uint32_t> & positions){
	return seqToJsonFactory::rComplement(getRef((cache_.at(uid).reads_)), positions, uid);
}
Json::Value seqCache::getJson(const std::string & uid, const std::vector<uint32_t> & positions){
	return seqToJsonFactory::seqsToJson(getRef((cache_.at(uid).reads_)), positions, uid);
}
Json::Value seqCache::translate(const std::string & uid,
		const std::vector<uint32_t> & positions, bool complement, bool reverse,
		uint64_t start){
	auto ret = seqToJsonFactory::translate(getRef((cache_.at(uid).reads_)), positions, uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : ret["seqs"]){
		(*proteins).emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		(*proteins).back().seqBase_.cnt_ = j["cnt"].asDouble();
		(*proteins).back().seqBase_.frac_ = j["frac"].asDouble();
	}
	updateAddCache(uid + "_protein", proteins);
	return ret;
}
Json::Value seqCache::translate(const std::string & uid, bool complement,
		bool reverse, uint64_t start){
	auto ret = seqToJsonFactory::translate(getRef((cache_.at(uid).reads_)), uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : ret["seqs"]){
		(*proteins).emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		(*proteins).back().seqBase_.cnt_ = j["cnt"].asDouble();
		(*proteins).back().seqBase_.frac_ = j["frac"].asDouble();
	}
	updateAddCache(uid + "_protein", proteins);
	return ret;
}



void seqCache::addToCache(const std::string & uid,
		const std::shared_ptr<std::vector<readObject>> & reads) {
	//check to see if cache already exists
	if (containsRecord(uid)) {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << std::endl;
		std::cerr << "Cache already contains uid: " << uid
				<< ", should call update instead" << std::endl;
		throw std::runtime_error{ss.str()};
	} else {
		cache_.emplace(uid, cacheRecord(uid, reads));
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
		}
	}
}

std::shared_ptr<std::vector<readObject>> seqCache::getRecord(const std::string & uid){
	return cache_.find(uid)->second.reads_;
}

void seqCache::updateAddCache(const std::string & uid,
		const std::shared_ptr<std::vector<readObject>> & reads) {
	if (containsRecord(uid)) {
		updateCache(uid, reads);
	} else {
		addToCache(uid, reads);
	}
}

void seqCache::updateCache(const std::string & uid,
		const std::shared_ptr<std::vector<readObject>> & reads) {
	// check to see if cache exists
	if (containsRecord(uid)) {
		cache_.find(uid)->second.reads_ = reads;
		if (!bib::in(uid, currentCache_)) {
			if (cachePos_ < currentCache_.size()) {
				cache_.at(currentCache_[cachePos_]).reads_ = nullptr;
				currentCache_[cachePos_] = uid;
				++cachePos_;
				if (cachePos_ >= cacheSizeLimit_) {
					cachePos_ = 0;
				}
			}
		}
	} else {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << std::endl;
		ss << "Cache doesn't contain uid: " << uid << ", nothing to update"
				<< std::endl;
		throw std::runtime_error { ss.str() };
	}
}

bool seqCache::recordValid(const std::string & uid)const{
	if(containsRecord(uid)){
		return nullptr != cache_.find(uid)->second.reads_;
	}else{
		return false;
	}
}

bool seqCache::containsRecord(const std::string & uid) const {
	return cache_.find(uid) != cache_.end();
}



} /* namespace bibseq */
