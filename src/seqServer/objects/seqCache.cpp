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

std::unordered_map<std::string,bib::color> getColorsForNames(const VecStr & popNames){
	auto popColors = bib::njhColors(popNames.size());
	bibseq::VecStr popColorsStrs(popColors.size());
	uint32_t count = 0;
	uint32_t halfCount = 0;
	for(const auto & cPos : iter::range(popColors.size())) {
		uint32_t pos = 0;
		if(cPos %2 == 0) {
			pos = popColors.size()/2 + halfCount;
			++halfCount;
		} else {
			pos = count;
			++count;
		}
		popColorsStrs[cPos] = "#" + popColors[pos].hexStr_;
	}
	std::unordered_map<std::string,bib::color> nameColors;
	for(auto pos : iter::range(popNames.size())){
		nameColors[popNames[pos]] = popColorsStrs[pos];
	}
	return nameColors;
}

Json::Value genMinTreeData(const std::vector<readObject> & reads, aligner & alignerObj){
  std::function<uint32_t(const readObject & ,
  		const readObject &, aligner, bool)> misFun = getMismatches<readObject>;
	auto misDistances = getDistanceCopy(reads, 2, misFun,
			alignerObj, true);
  readDistGraph<uint32_t> graphMis(misDistances, reads);
	std::vector<std::string> popNames;
  for(const auto & n : graphMis.nodes_){
  	popNames.emplace_back(n->name_);
  }
  auto nameColors = getColorsForNames(popNames);
	return graphMis.toJsonMismatchGraphAll(bib::color("#000000"), nameColors);
}

Json::Value genMinTreeData(const std::vector<readObject> & reads) {
	uint64_t maxLength = 0;
	readVec::getMaxLength(reads, maxLength);
	aligner alignerObj(maxLength, gapScoringParameters(5, 1),
			substituteMatrix::createDegenScoreMatrix(2, -2));
	std::function<uint32_t(const readObject &, const readObject &, aligner, bool)> misFun =
			getMismatches<readObject>;
	auto misDistances = getDistanceCopy(reads, 2, misFun, alignerObj, true);
	readDistGraph<uint32_t> graphMis(misDistances, reads);
	std::vector<std::string> popNames;
	for (const auto & n : graphMis.nodes_) {
		popNames.emplace_back(n->name_);
	}
	auto nameColors = getColorsForNames(popNames);
	return graphMis.toJsonMismatchGraphAll(bib::color("#000000"), nameColors);
}

cppcms::json::value seqCache::getJson(const std::string & uid){
	return seqToJsonFactory::seqsToJson(*(cache_.at(uid).reads_), uid);
}

cppcms::json::value seqCache::sort(const std::string & uid, const std::string & sortOption){
	return seqToJsonFactory::sort(cache_.at(uid).reads_, sortOption, uid);
}

cppcms::json::value seqCache::muscle(const std::string & uid){
	return seqToJsonFactory::muscle(cache_.at(uid).reads_, uid);
}

cppcms::json::value seqCache::removeGaps(const std::string & uid){
	return seqToJsonFactory::removeGaps(cache_.at(uid).reads_, uid);
}

cppcms::json::value seqCache::rComplement(const std::string & uid){
	return seqToJsonFactory::rComplement(cache_.at(uid).reads_, uid);
}

cppcms::json::value seqCache::minTreeData(const std::string & uid){
	return seqToJsonFactory::minTreeData(cache_.at(uid).reads_, uid);
}

cppcms::json::value seqCache::minTreeData(const std::string & uid, const std::vector<uint64_t> & positions){
	return seqToJsonFactory::minTreeData(cache_.at(uid).reads_,positions, uid);
}

cppcms::json::value seqCache::muscle(const std::string & uid,const std::vector<uint64_t> & positions){
	return seqToJsonFactory::muscle(cache_.at(uid).reads_, positions, uid);
}
cppcms::json::value seqCache::removeGaps(const std::string & uid, const std::vector<uint64_t> & positions){
	return seqToJsonFactory::removeGaps(cache_.at(uid).reads_, positions, uid);
}
cppcms::json::value seqCache::rComplement(const std::string & uid, const std::vector<uint64_t> & positions){
	return seqToJsonFactory::rComplement(cache_.at(uid).reads_, positions, uid);
}
cppcms::json::value seqCache::getJson(const std::string & uid, const std::vector<uint64_t> & positions){
	return seqToJsonFactory::seqsToJson(*(cache_.at(uid).reads_), positions, uid);
}
cppcms::json::value seqCache::translate(const std::string & uid,
		const std::vector<uint64_t> & positions, bool complement, bool reverse,
		uint64_t start){
	auto ret = seqToJsonFactory::translate(cache_.at(uid).reads_, positions, uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : cppcmsJsonToJson(ret["seqs"])){
		(*proteins).emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		(*proteins).back().seqBase_.cnt_ = j["cnt"].asDouble();
		(*proteins).back().seqBase_.frac_ = j["frac"].asDouble();
	}
	updateAddCache(uid + "_protein", proteins);
	return ret;
}
cppcms::json::value seqCache::translate(const std::string & uid, bool complement,
		bool reverse, uint64_t start){
	auto ret = seqToJsonFactory::translate(cache_.at(uid).reads_, uid, complement, reverse, start);
	std::shared_ptr<std::vector<readObject>> proteins = std::make_shared<std::vector<readObject>>();
	for(const auto & j : cppcmsJsonToJson(ret["seqs"])){
		(*proteins).emplace_back(seqInfo(j["name"].asString(), j["seq"].asString()));
		(*proteins).back().seqBase_.cnt_ = j["cnt"].asDouble();
		(*proteins).back().seqBase_.frac_ = j["frac"].asDouble();
	}
	updateAddCache(uid + "_protein", proteins);
	return ret;
}

bool seqCache::recordValid(const std::string & uid)const{
	if(containsRecord(uid)){
		return nullptr != cache_.find(uid)->second.reads_;
	}else{
		return false;
	}
}

bool seqCache::containsRecord(const std::string & uid)const{
	return cache_.find(uid) != cache_.end();
}

void seqCache::addToCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads){
	//check to see if cache already exists
	if(containsRecord(uid)){
		std::cerr << "seqCache::addToCache" << std::endl;
		std::cerr << "Cache already contains uid: " << uid << ", should call update instead" << std::endl;
	}else{
		cache_.emplace(uid,cacheRecord(uid, reads));
		if(cachePos_ < currentCache_.size()){
			cache_.at(currentCache_[cachePos_]).reads_ = nullptr;
			currentCache_[cachePos_] = uid;
			++cachePos_;
			if(cachePos_ >= cacheSizeLimit_){
				cachePos_ = 0;
			}
		}else{
			++cachePos_;
			currentCache_.emplace_back(uid);
		}
	}
}

std::shared_ptr<std::vector<readObject>> seqCache::getRecord(const std::string & uid){
	return cache_.find(uid)->second.reads_;
}

void seqCache::updateAddCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads){
	if(containsRecord(uid)){
		updateCache(uid,reads);
	}else{
		addToCache(uid, reads);
	}
}

void seqCache::updateCache(const std::string & uid, const std::shared_ptr<std::vector<readObject>> & reads){
	// check to see if cache exists
	if(containsRecord(uid)){
		cache_.find(uid)->second.reads_ = reads;
		if(!bib::in(uid, currentCache_)){
			if(cachePos_ < currentCache_.size()){
				cache_.at(currentCache_[cachePos_]).reads_ = nullptr;
				currentCache_[cachePos_] = uid;
				++cachePos_;
				if(cachePos_ >= cacheSizeLimit_){
					cachePos_ = 0;
				}
			}
		}
	}else{
		std::cerr << "seqCache::updateCache" << std::endl;
		std::cerr << "Cache doesn't contain uid: " << uid << ", nothing to update" << std::endl;
	}
}



} /* namespace bibseq */
