/*
 * ColorFactory.cpp
 *
 *  Created on: Aug 30, 2016
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
#include "ColorFactory.hpp"

namespace bibseq {

//old
//const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#e6e6e6\","
//		"\"A\":\"#ff8787\","
//		"\"C\":\"#afffaf\","
//		"\"G\":\"#ffffaf\","
//		"\"N\":\"#AFAFAF\","
//		"\"T\":\"#87afff\","
//		"\"U\":\"#87afff\","
//		"\"a\":\"#e66e6e\","
//		"\"c\":\"#96dc96\","
//		"\"g\":\"#dcdc91\","
//		"\"n\":\"#7D7D7D\","
//		"\"t\":\"#6e96e6\","
//		"\"u\":\"#6e96e6\"}" };

//color blind safe
//const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#969696\","
//		"\"A\":\"#FCAE91\","
//		"\"C\":\"#EBFAD7\","
//		"\"G\":\"#F5EB91\","
//		"\"N\":\"#636363\","
//		"\"T\":\"#BDD7E7\","
//		"\"U\":\"#BDD7E7\","
//		"\"a\":\"#e66e6e\","
//		"\"c\":\"#96dc96\","
//		"\"g\":\"#dcdc91\","
//		"\"n\":\"#818181\","
//		"\"t\":\"#6e96e6\","
//		"\"u\":\"#6e96e6\"}" };

//const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#969696\","
//		"\"A\":\"#FB6A4A\","
//		"\"C\":\"#55C80A\","
//		"\"G\":\"#F5FA00\","
//		"\"N\":\"#636363\","
//		"\"T\":\"#3182BD\","
//		"\"U\":\"#3182BD\","
//		"\"a\":\"#FCAE91\","
//		"\"c\":\"#EBFAD7\","
//		"\"g\":\"#F5EB91\","
//		"\"n\":\"#818181\","
//		"\"t\":\"#BDD7E7\","
//		"\"u\":\"#BDD7E7\"}" };

//const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#969696\","
//		"\"A\":\"#AA0A3C\","
//		"\"C\":\"#0AB45A\","
//		"\"G\":\"#F0F032\","
//		"\"N\":\"#636363\","
//		"\"T\":\"#005AC8\","
//		"\"U\":\"#3182BD\","
//		"\"a\":\"#FCAE91\","
//		"\"c\":\"#EBFAD7\","
//		"\"g\":\"#F5EB91\","
//		"\"n\":\"#818181\","
//		"\"t\":\"#BDD7E7\","
//		"\"u\":\"#BDD7E7\"}" };

const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#969696\","
		"\"A\":\"#E02527\","
		"\"C\":\"#399F34\","
		"\"G\":\"#F0F032\","
		"\"N\":\"#636363\","
		"\"T\":\"#2679B2\","
		"\"U\":\"#2679B2\","
		"\"a\":\"#FA9A9A\","
		"\"c\":\"#B2DE8D\","
		"\"g\":\"#FFFC9F\","
		"\"n\":\"#818181\","
		"\"t\":\"#A7CEE2\","
		"\"u\":\"#A7CEE2\"}" };

const std::string ColorFactory::AAColorsJson { "{\"*\":\"#e6e6e6\","
		"\"-\":\"#e6e6e6\","
		"\"A\":\"#14b814\","
		"\"C\":\"#13d8d8\","
		"\"D\":\"#12ade0\","
		"\"E\":\"#117de8\","
		"\"F\":\"#13d0a1\","
		"\"G\":\"#134aef\","
		"\"H\":\"#1919f0\","
		"\"I\":\"#541ff2\","
		"\"K\":\"#8c25f4\","
		"\"L\":\"#14c86e\","
		"\"M\":\"#c32bf5\","
		"\"N\":\"#f631f6\","
		"\"P\":\"#f838c8\","
		"\"Q\":\"#f93e9c\","
		"\"R\":\"#fa4572\","
		"\"S\":\"#fb4b4b\","
		"\"T\":\"#fc7c52\","
		"\"V\":\"#fdab58\","
		"\"W\":\"#fed65f\","
		"\"X\":\"#999999\","
		"\"Y\":\"#ffff66\"}" };

Json::Value ColorFactory::getColors(uint32_t num) {
	Json::Value ret;
	auto outColors = bib::njhColors(num);
	VecStr outColorsStrs;
	outColorsStrs.reserve(outColors.size());
	for (const auto & c : outColors) {
		outColorsStrs.emplace_back("#" + c.hexStr_);
	}
	ret["colors"] = bib::json::toJson(outColorsStrs);
	return ret;
}


}  // namespace bibseq
