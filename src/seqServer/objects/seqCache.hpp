#pragma once
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

	static cppcms::json::value sort(const std::shared_ptr<std::vector<readObject>> & reads, const std::string & sortOption);
	static cppcms::json::value muscle(const std::shared_ptr<std::vector<readObject>> & reads);
	static cppcms::json::value removeGaps(const std::shared_ptr<std::vector<readObject>> & reads);
	static cppcms::json::value rComplement(const std::shared_ptr<std::vector<readObject>> & reads);
	static cppcms::json::value minTreeData(const std::shared_ptr<std::vector<readObject>> & reads);

};

} /* namespace bibseq */




