/*
 * UrlPathFactory.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "UrlPathFactory.hpp"

namespace bibseq {


std::string UrlPathFactory::createUrl(const std::vector<PathSeg> & segments){
	std::string ret = "";
	for(const auto & seg : segments){
		ret += "/";
		if("" == seg.pat_){
			ret += seg.name_;
		}else{
			ret += "{" + seg.name_ + ": " + seg.pat_ + "}";
		}
	}
	return ret;
}

const std::string UrlPathFactory::pat_wordNumsDash_ { R"([A-Za-z0-9\-\_\.]+)" };
const std::string UrlPathFactory::pat_wordNums_ { R"([A-Za-z0-9]+)" };
const std::string UrlPathFactory::pat_word_ { R"([A-Za-z]+)" };
const std::string UrlPathFactory::pat_nums_ { R"([0-9]+)" };


} /* namespace bibseq */
