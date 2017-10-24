/*
 * HeaderFactory.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


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
