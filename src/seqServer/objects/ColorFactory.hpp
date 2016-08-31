#pragma once
/*
 * ColorFactory.hpp
 *
 *  Created on: Aug 30, 2016
 *      Author: nick
 */

#include "seqServer/utils.h"

namespace bibseq {


class ColorFactory{
public:

	static const std::string DNAColorsJson;
	static const std::string AAColorsJson;
	static Json::Value getColors(uint32_t num);
};

}  // namespace bibseq



