#pragma once
/*
 * restbedUtils.hpp
 *
 *  Created on: Sep 10, 2016
 *      Author: nick
 */




#include "seqServer/utils.h"
#include <restbed>

namespace bibseq {

Json::Value bytesToJson(const restbed::Bytes & body);


}  // namespace bibseq


