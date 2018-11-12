/*
 * UrlPathFactory.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

//
// seqServer - A library for analyzing sequence data
// Copyright (C) 2012-2018 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
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

#include "UrlPathFactory.hpp"

namespace njhseq {


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
	ret = njh::files::make_path(pathParams).string();

	return ret;
}

const std::string UrlPathFactory::pat_wordNumsDash_ { "[A-Za-z0-9\\-_\\.]+" };
const std::string UrlPathFactory::pat_wordNums_ { "[A-Za-z0-9]+" };
const std::string UrlPathFactory::pat_word_ { "[A-Za-z]+" };
const std::string UrlPathFactory::pat_nums_ { "[0-9]+" };


} /* namespace njhseq */
