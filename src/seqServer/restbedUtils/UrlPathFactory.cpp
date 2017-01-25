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
	VecStr pathParams{"/"};
	for(const auto & seg : segments){
		if("" == seg.pat_){
			pathParams.emplace_back(seg.name_);
		}else{
			pathParams.emplace_back("{" + seg.name_ + ": " + seg.pat_ + "}");
		}
	}
	ret = bib::files::make_path(pathParams).string();

	return ret;
}

const std::string UrlPathFactory::pat_wordNumsDash_ { "[A-Za-z0-9\\-_\\.]+" };
const std::string UrlPathFactory::pat_wordNums_ { "[A-Za-z0-9]+" };
const std::string UrlPathFactory::pat_word_ { "[A-Za-z]+" };
const std::string UrlPathFactory::pat_nums_ { "[0-9]+" };


} /* namespace bibseq */
