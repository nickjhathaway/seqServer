/*
 * LogMessageFactory.cpp
 *
 *  Created on: Sep 3, 2016
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
#include "LogMessageFactory.hpp"

namespace bibseq {


LogMessageFactory::LogMessage::LogMessage(const std::string & funcName,
		std::ostream & out,
		bool debug,
		uint32_t indentLevel,
		std::shared_ptr<LogMessageFactory> lmf) :
		mess_(
				std::make_unique<bib::scopedMessage>(funcName + "_start",
						funcName + "_stop", out, debug, indentLevel)), lmf_(lmf) {

}

LogMessageFactory::LogMessage::~LogMessage(){
	if(lmf_){
		lmf_->decreaseeMessageDepth();
	}
}


LogMessageFactory::LogMessageFactory(std::ostream & out, bool debug) :
		out_(out), debug_(debug) {

}


void LogMessageFactory::decreaseeMessageDepth(){
	--messageDepth_;
}
void LogMessageFactory::increaseMessageDepth(){
	++messageDepth_;
}

std::unique_ptr<LogMessageFactory::LogMessage> LogMessageFactory::genLogMessage(const std::string & funcName) {
	increaseMessageDepth();
	int32_t indentLevel = messageDepth_ - 1;
	return std::make_unique<LogMessage>(messStrFactory(funcName), out_, debug_,
			std::max(0, indentLevel), shared_from_this());
}

std::unique_ptr<LogMessageFactory::LogMessage> LogMessageFactory::genLogMessage(const std::string & funcName,
		const MapStrStr & args) {
	increaseMessageDepth();
	int32_t indentLevel = messageDepth_ - 1;
	return std::make_unique<LogMessage>(messStrFactory(funcName, args), out_,
			debug_, std::max(0, indentLevel), shared_from_this());
}

std::string LogMessageFactory::messStrFactory(const std::string & funcName) {
	return bib::err::F() << "[" << getCurrentDate() << "] " << funcName;
}

std::string LogMessageFactory::messStrFactory(const std::string & funcName,
		const MapStrStr & args) {
	VecStr argsVec;
	for (const auto & kv : args) {
		argsVec.emplace_back(kv.first + " = " + kv.second);
	}
	std::string argStrs = messStrFactory(funcName) + " ["
			+ vectorToString(argsVec, ", ") + "]";
	return argStrs;
}

} /* namespace bibseq */
