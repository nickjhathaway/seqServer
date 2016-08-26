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
			std::shared_ptr<restbed::Session> session) const;
	void getProteinColorsHandler(
			std::shared_ptr<restbed::Session> session) const;
	void getColorsHandler(
			std::shared_ptr<restbed::Session> session) const;

	std::string root_;
	bool debug_ = false;

	void cssOwnHandler(std::shared_ptr<restbed::Session> session);
	void cssLibsHandler(std::shared_ptr<restbed::Session> session);
	void jsOwnHandler(std::shared_ptr<restbed::Session> session);
	void jsLibsHandler(std::shared_ptr<restbed::Session> session);



protected:
	std::map<std::string, bib::files::FileCache> pages_;
	std::map<std::string, bib::files::FilesCache> jsAndCss_;


public:

	std::shared_ptr<restbed::Resource> cssOwn();
	std::shared_ptr<restbed::Resource> cssLibs();
	std::shared_ptr<restbed::Resource> jsOwn();
	std::shared_ptr<restbed::Resource> jsLibs();

	std::shared_ptr<restbed::Resource> getDNAColors() const;
	std::shared_ptr<restbed::Resource> getProteinColors() const;
	std::shared_ptr<restbed::Resource> getColors() const;

	SeqAppRestbed(const Json::Value & config);


	std::shared_ptr<restbed::Service> service_;
	const Json::Value config_;

	virtual ~SeqAppRestbed();

	virtual VecStr requiredOptions() const;

	void checkConfigThrow() const;

	std::vector<std::shared_ptr<restbed::Resource>> getAllResources();


	void addPages(const bfs::path & dir);

	std::string messStrFactory(const std::string & funcName);
	std::string messStrFactory(const std::string & funcName,
			const MapStrStr & args);

};


}  // namespace bibseq


