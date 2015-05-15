/*
 * utils.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "seqCache.hpp"

namespace bibseq {





cppcms::json::value seqToJsonFactory::sort(const std::shared_ptr<std::vector<readObject>> & reads, const std::string & sortOption){
	readVecSorter::sortReadVector(*reads, sortOption);
	return seqsToJson(*reads, "");
}
cppcms::json::value seqToJsonFactory::muscle(const std::shared_ptr<std::vector<readObject>> & reads){
	bib::for_each(*reads, [](readObject & read){ read.seqBase_.removeGaps();});
	sys::muscleSeqs(*reads);
	return seqsToJson(*reads, "");
}

cppcms::json::value seqToJsonFactory::removeGaps(const std::shared_ptr<std::vector<readObject>> & reads){
	bib::for_each(*reads, [](readObject & read){ read.seqBase_.removeGaps();});
	return seqsToJson(*reads, "");
}

cppcms::json::value seqToJsonFactory::rComplement(const std::shared_ptr<std::vector<readObject>> & reads){
	readVec::allReverseComplement(*reads, true);
	return seqsToJson(*reads, "");
}
cppcms::json::value seqToJsonFactory::minTreeData(const std::shared_ptr<std::vector<readObject>> & reads){
	/**@todo need to complete function */
	return seqsToJson(*reads, "");
}


cppcms::json::value seqCache::getJson(const std::string & uid){
	return seqsToJson(*(cache_.at(uid).reads_), uid);
}

cppcms::json::value seqCache::sort(const std::string & uid, const std::string & sortOption){
	return seqToJsonFactory::sort(cache_.at(uid).reads_, sortOption);
}
cppcms::json::value seqCache::muscle(const std::string & uid){
	return seqToJsonFactory::muscle(cache_.at(uid).reads_);
}
cppcms::json::value seqCache::removeGaps(const std::string & uid){
	return seqToJsonFactory::removeGaps(cache_.at(uid).reads_);
}
cppcms::json::value seqCache::rComplement(const std::string & uid){
	return seqToJsonFactory::rComplement(cache_.at(uid).reads_);
}
cppcms::json::value seqCache::minTreeData(const std::string & uid){
	return seqToJsonFactory::minTreeData(cache_.at(uid).reads_);
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
		std::cerr << __func__ << std::endl;
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
		std::cerr << __func__ << std::endl;
		std::cerr << "Cache doesn't contain uid: " << uid << ", nothing to update" << std::endl;
	}
}

} /* namespace bibseq */
