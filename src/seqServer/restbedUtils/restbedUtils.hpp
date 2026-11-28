#pragma once
/*
 * restbedUtils.hpp
 *
 *  Created on: Sep 10, 2016
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
#include <restbed>

namespace njhseq {

/**@brief Copy a restbed request body into a std::string
 *
 * Works with restbed 4.x (Bytes is std::vector<uint8_t>) and restbed 5.x (Bytes is std::vector<std::byte>,
 * which can't be converted to char implicitly, so std::string(body.begin(), body.end()) no longer compiles)
 *
 * @param body the request body
 * @return the body as a string
 */
std::string bytesToString(const restbed::Bytes & body);

Json::Value bytesToJson(const restbed::Bytes & body);


}  // namespace njhseq


