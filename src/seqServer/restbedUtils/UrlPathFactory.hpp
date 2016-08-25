#pragma once
/*
 * UrlPathFactory.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "seqServer/utils.h"


namespace bibseq {

class UrlPathFactory {
public:
	struct PathSeg{
		const std::string name_;
		const std::string pat_{""};
	};

	/**@brief Create a restbed style url path with optionally putting in path parameter with regex matching
	 *
	 * @param segments Path segments, each to be separated by a /
	 * @return an url that always starts with a /
	 */
	static std::string createUrl(const std::vector<PathSeg> & segments){
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

	const std::string pat_wordNumsDash_ =  R"([A-Za-z0-9\-\_\.]+)";
	const std::string pat_wordNums_ =  R"([A-Za-z0-9]+)";
	const std::string pat_word_ =  R"([A-Za-z]+)";
	const std::string pat_nums_ =  R"([0-9]+)";


};

} /* namespace bibseq */

