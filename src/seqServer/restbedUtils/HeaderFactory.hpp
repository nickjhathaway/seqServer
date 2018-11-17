#pragma once
/*
 * HeaderFactory.hpp
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
#include "seqServer/utils.h"

namespace njhseq {

class HeaderFactory{

public:
	static std::multimap<std::string, std::string> initiateTxtJavascriptHeader(
			const std::string & body);
	static std::multimap<std::string, std::string> initiateTxtCssHeader(
			const std::string & body);
	static std::multimap<std::string, std::string> initiateTxtHtmlHeader(
			const std::string & body);
	static std::multimap<std::string, std::string> initiateAppJsonHeader(
			const std::string & body);
	static std::multimap<std::string, std::string> initiatePlainTxtHeader(
			const std::string & body);

	static std::multimap<std::string, std::string> initiateTxtJavascriptHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateTxtCssHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateTxtHtmlHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateAppJsonHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiatePlainTxtHeader(
			const uint32_t & length);

};



}  // namespace njhseq




