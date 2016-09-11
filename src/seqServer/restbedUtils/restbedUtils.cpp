/*
 * restbedUtils.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: nick
 */


#include "restbedUtils.hpp"


namespace bibseq {


Json::Value bytesToJson(const restbed::Bytes & body){
	return bib::json::parse(std::string(body.begin(), body.end()));
}



} // namespace bibseq

