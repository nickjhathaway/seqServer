/*
 * LogMessageFactory.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: nick
 */

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
