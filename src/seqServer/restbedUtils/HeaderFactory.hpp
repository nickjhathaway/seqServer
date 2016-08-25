#pragma once
/*
 * HeaderFactory.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */

#include "seqServer/utils.h"

namespace bibseq {

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

	static std::multimap<std::string, std::string> initiateTxtJavascriptHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateTxtCssHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateTxtHtmlHeader(
			const uint32_t & length);
	static std::multimap<std::string, std::string> initiateAppJsonHeader(
			const uint32_t & length);
};



}  // namespace bibseq




