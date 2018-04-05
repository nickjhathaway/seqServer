/*
 * HeaderFactory.cpp
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

#include "HeaderFactory.hpp"

namespace bibseq {

std::multimap<std::string, std::string> HeaderFactory::initiateTxtJavascriptHeader(
		const std::string & body) {
	return HeaderFactory::initiateTxtJavascriptHeader(body.size());
}
std::multimap<std::string, std::string> HeaderFactory::initiateTxtCssHeader(
		const std::string & body) {
	return HeaderFactory::initiateTxtCssHeader(body.size());
}
std::multimap<std::string, std::string> HeaderFactory::initiateTxtHtmlHeader(
		const std::string & body) {
	return HeaderFactory::initiateTxtHtmlHeader(body.size());
}
std::multimap<std::string, std::string> HeaderFactory::initiateAppJsonHeader(
		const std::string & body) {
	return HeaderFactory::initiateAppJsonHeader(body.size());
}
std::multimap<std::string, std::string> HeaderFactory::initiatePlainTxtHeader(
		const std::string & body){
	return HeaderFactory::initiatePlainTxtHeader(body.size());
}


std::multimap<std::string, std::string> HeaderFactory::initiateTxtJavascriptHeader(
		const uint32_t & length) {
	return { {
			"Content-Type", "text/javascript"}, {"Content-Length",
			estd::to_string(length)}};
}
std::multimap<std::string, std::string> HeaderFactory::initiateTxtCssHeader(
		const uint32_t & length) {
	return { {
			"Content-Type", "text/css"}, {"Content-Length",
			estd::to_string(length)}};
}
std::multimap<std::string, std::string> HeaderFactory::initiateTxtHtmlHeader(
		const uint32_t & length) {
	return { {
			"Content-Type", "text/html"}, {"Content-Length",
			estd::to_string(length)}};
}
std::multimap<std::string, std::string> HeaderFactory::initiateAppJsonHeader(
		const uint32_t & length) {
	return { {
			"Content-Type", "application/json"}, {"Content-Length",
			estd::to_string(length)}};
}

std::multimap<std::string, std::string> HeaderFactory::initiatePlainTxtHeader(
		const uint32_t & length){
	return { {
			"Content-Type", "text/plain"}, {"Content-Length",
			estd::to_string(length)}};
}

}  // namespace bibseq
