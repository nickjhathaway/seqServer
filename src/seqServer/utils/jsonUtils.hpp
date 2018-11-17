#pragma once
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
/*
 * jsonUtils.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include <njhseq.h>
#include <njhcpp.h>
#include <cppitertools/range.hpp>

namespace njhseq {


Json::Value dotToJson(const std::string& dotFilename);

Json::Value tableToJsonByRow(const njhseq::table & tab,
		const std::string mainColName,
		const VecStr & initialVisibleColumns = VecStr { },
		const VecStr & excludeFromNum = VecStr { });

Json::Value tableToJsonColumnWise(const njhseq::table & tab);


std::vector<uint32_t> parseJsonForSelected(const Json::Value & postData);

std::vector<uint32_t> parseJsonForPosition(const Json::Value & postData);


} /* namespace njhseq */


