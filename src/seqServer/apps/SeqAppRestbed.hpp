#pragma once
/*
 * SeqAppRestbed.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "seqServer/utils.h"
#include "seqServer/restbedUtils.h"
#include "seqServer/objects.h"


namespace bibseq {


class SeqAppRestbed {

	void getDNAColorsHandler(
			std::shared_ptr<restbed::Session> session) const;
	void getProteinColorsHandler(
			std::shared_ptr<restbed::Session> session) const;
	void getColorsHandler(
			std::shared_ptr<restbed::Session> session) const;



	void cssOwnHandler(std::shared_ptr<restbed::Session> session);
	void cssLibsHandler(std::shared_ptr<restbed::Session> session);
	void jsOwnHandler(std::shared_ptr<restbed::Session> session);
	void jsLibsHandler(std::shared_ptr<restbed::Session> session);


	void sortPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
	void muscleAlnPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
	void removeGapsPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
	void complementSeqsPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
	void translateToProteinPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);
	void minTreeDataDetailedPostHandler(std::shared_ptr<restbed::Session> session,
			const restbed::Bytes & body);

	void sortHandler(std::shared_ptr<restbed::Session> session); //std::string sortBy
	void muscleAlnHandler(std::shared_ptr<restbed::Session> session);
	void removeGapsHandler(std::shared_ptr<restbed::Session> session);
	void complementSeqsHandler(std::shared_ptr<restbed::Session> session);
	void translateToProteinHandler(std::shared_ptr<restbed::Session> session);
	void minTreeDataDetailedHandler(std::shared_ptr<restbed::Session> session);


	void closeSessionPostHandler(std::shared_ptr<restbed::Session> session,
				const restbed::Bytes & body);
	void closeSessionHandler(std::shared_ptr<restbed::Session> session);

	void openSessionHandler(std::shared_ptr<restbed::Session> session);




protected:
	std::map<std::string, bib::files::FileCache> pages_;
	std::map<std::string, bib::files::FilesCache> jsAndCss_;
	std::string rootName_;
	bool debug_ = false;

	SessionUIDFactory sesUIDFac_;

	uint32_t startSeqCacheSession();

public:

	std::shared_ptr<restbed::Resource> cssOwn();
	std::shared_ptr<restbed::Resource> cssLibs();
	std::shared_ptr<restbed::Resource> jsOwn();
	std::shared_ptr<restbed::Resource> jsLibs();

	std::shared_ptr<restbed::Resource> getDNAColors() const;
	std::shared_ptr<restbed::Resource> getProteinColors() const;
	std::shared_ptr<restbed::Resource> getColors() const;


	std::shared_ptr<restbed::Resource> sort(); //std::string sortBy
	std::shared_ptr<restbed::Resource> muscleAln();
	std::shared_ptr<restbed::Resource> removeGaps();
	std::shared_ptr<restbed::Resource> complementSeqs();
	std::shared_ptr<restbed::Resource> translateToProtein();
	std::shared_ptr<restbed::Resource> minTreeDataDetailed();

	std::shared_ptr<restbed::Resource> closeSession();
	std::shared_ptr<restbed::Resource> openSession();

	SeqAppRestbed(const Json::Value & config);



	const Json::Value config_;
	std::shared_ptr<seqCache> seqs_;

	std::unordered_map<uint32_t, std::unique_ptr<seqCache>> seqsBySession_;

	virtual ~SeqAppRestbed();

	virtual VecStr requiredOptions() const;

	void checkConfigThrow() const;

	virtual std::vector<std::shared_ptr<restbed::Resource>> getAllResources();


	void addPages(const bfs::path & dir);

	std::string messStrFactory(const std::string & funcName);
	std::string messStrFactory(const std::string & funcName,
			const MapStrStr & args);

};


}  // namespace bibseq


