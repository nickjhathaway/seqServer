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
	struct PathSeg {
		const std::string name_;
		const std::string pat_ { "" };
	};

	/**@brief Create a restbed style url path with optionally putting in path parameter with regex matching
	 *
	 * @param segments Path segments, each to be separated by a /
	 * @return an url that always starts with a /
	 */
	static std::string createUrl(const std::vector<PathSeg> & segments);

	static const std::string pat_wordNumsDash_ ;
	static const std::string pat_wordNums_;
	static const std::string pat_word_;
	static const std::string pat_nums_;


};

} /* namespace bibseq */

