#pragma once
/*
 * SeqAppRestbed.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "seqServer/utils.h"
#include "seqServer/restbedUtils.h"
#include "seqServer/objects/seqCache.hpp"


namespace bibseq {


class ColorFactory{
public:

	static const std::string DNAColorsJson;
	static const std::string AAColorsJson;
	static Json::Value getColors(uint32_t num);
};




class SeqAppRestbed {

	void getDNAColorsHandler(
			const std::shared_ptr<restbed::Session> session) const;
	void getProteinColorsHandler(
			const std::shared_ptr<restbed::Session> session) const;

	std::string root_;
	bool debug_ = false;
public:

	SeqAppRestbed(const std::shared_ptr<restbed::Service> service, const Json::Value & config);


	std::shared_ptr<restbed::Service> service_;
	const Json::Value config_;

	virtual ~SeqAppRestbed();

	virtual VecStr requiredOptions() const;

	void checkConfigThrow() const;

	std::map<std::string, bib::files::FileCache> pages_;

	std::map<std::string, bib::files::FilesCache> jsAndCss_;

};


}  // namespace bibseq


