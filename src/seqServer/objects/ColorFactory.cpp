/*
 * ColorFactory.cpp
 *
 *  Created on: Aug 30, 2016
 *      Author: nick
 */


#include "ColorFactory.hpp"

namespace bibseq {


const std::string ColorFactory::DNAColorsJson { "{\"-\":\"#e6e6e6\","
		"\"A\":\"#ff8787\","
		"\"C\":\"#afffaf\","
		"\"G\":\"#ffffaf\","
		"\"N\":\"#AFAFAF\","
		"\"T\":\"#87afff\","
		"\"U\":\"#87afff\","
		"\"a\":\"#e66e6e\","
		"\"c\":\"#96dc96\","
		"\"g\":\"#dcdc91\","
		"\"n\":\"#7D7D7D\","
		"\"t\":\"#6e96e6\","
		"\"u\":\"#6e96e6\"}" };

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
