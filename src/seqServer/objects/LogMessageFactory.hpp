#pragma once
/*
 * LogMessageFactory.hpp
 *
 *  Created on: Sep 3, 2016
 *      Author: nick
 */

#include "seqServer/utils.h"


namespace bibseq {



class LogMessageFactory : public std::enable_shared_from_this<LogMessageFactory> {
public:

	struct LogMessage {
		LogMessage(const std::string & funcName, std::ostream & out, bool debug,
				uint32_t indentLevel, std::shared_ptr<LogMessageFactory> lmf);

		std::unique_ptr<bib::scopedMessage> mess_;
		std::shared_ptr<LogMessageFactory> lmf_;

		~LogMessage();
	};

	LogMessageFactory(std::ostream & out, bool debug);

	std::ostream & out_;
	bool debug_;
	std::atomic_int messageDepth_{0};

	void decreaseeMessageDepth();
	void increaseMessageDepth();

	std::unique_ptr<LogMessage> genLogMessage(const std::string & funcName);

	std::unique_ptr<LogMessage> genLogMessage(const std::string & funcName,
			const MapStrStr & args);

	static std::string messStrFactory(const std::string & funcName);

	static std::string messStrFactory(const std::string & funcName,
			const MapStrStr & args);



};

} /* namespace bibseq */

