#pragma once
/*
 * SeqApp.hpp
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

#include "seqServer/utils.h"
#include "seqServer/restbedUtils.h"
#include "seqServer/objects.h"


namespace njhseq {




class SeqApp {

	void getDNAColorsHandler(
			std::shared_ptr<restbed::Session> session) const;
	void getProteinColorsHandler(
			std::shared_ptr<restbed::Session> session) const;
	void getColorsHandler(
			std::shared_ptr<restbed::Session> session) const;



	void cssHandler(std::shared_ptr<restbed::Session> session);
	void jsHandler(std::shared_ptr<restbed::Session> session);


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
	void countBasesPostHandler(std::shared_ptr<restbed::Session> session,
				const restbed::Bytes & body);
	void deleteSeqsPostHandler(std::shared_ptr<restbed::Session> session,
					const restbed::Bytes & body);

	void sortHandler(std::shared_ptr<restbed::Session> session); //std::string sortBy
	void muscleAlnHandler(std::shared_ptr<restbed::Session> session);
	void removeGapsHandler(std::shared_ptr<restbed::Session> session);
	void complementSeqsHandler(std::shared_ptr<restbed::Session> session);
	void translateToProteinHandler(std::shared_ptr<restbed::Session> session);
	void minTreeDataDetailedHandler(std::shared_ptr<restbed::Session> session);
	void countBasesHandler(std::shared_ptr<restbed::Session> session);
	void deleteSeqsHandler(std::shared_ptr<restbed::Session> session);



	void closeSessionPostHandler(std::shared_ptr<restbed::Session> session,
				const restbed::Bytes & body);
	void closeSessionHandler(std::shared_ptr<restbed::Session> session);

	void openSessionHandler(std::shared_ptr<restbed::Session> session);




protected:
	std::map<std::string, njh::files::FileCache> pages_;
	std::map<std::string, njh::files::FileCache> fonts_;
	std::unique_ptr<njh::files::FilesCache> jsFiles_;
	std::unique_ptr<njh::files::FilesCache> cssFiles_;
	std::string rootName_;
	bool debug_ = false;

	SessionUIDFactory sesUIDFac_;

	std::shared_ptr<LogMessageFactory> messFac_;

	uint32_t startSeqCacheSession();

public:

	std::shared_ptr<restbed::Resource> css();
	std::shared_ptr<restbed::Resource> js();

	std::shared_ptr<restbed::Resource> getDNAColors() const;
	std::shared_ptr<restbed::Resource> getProteinColors() const;
	std::shared_ptr<restbed::Resource> getColors() const;


	std::shared_ptr<restbed::Resource> sort();
	std::shared_ptr<restbed::Resource> muscleAln();
	std::shared_ptr<restbed::Resource> removeGaps();
	std::shared_ptr<restbed::Resource> complementSeqs();
	std::shared_ptr<restbed::Resource> translateToProtein();
	std::shared_ptr<restbed::Resource> minTreeDataDetailed();
	std::shared_ptr<restbed::Resource> countBases();
	std::shared_ptr<restbed::Resource> deleteSeqs();

	std::shared_ptr<restbed::Resource> closeSession();
	std::shared_ptr<restbed::Resource> openSession();

	SeqApp(const Json::Value & config);



	const Json::Value config_;
	std::shared_ptr<SeqCache> seqs_;

	std::unordered_map<uint32_t, std::unique_ptr<SeqCache>> seqsBySession_;

	virtual ~SeqApp();

	virtual VecStr requiredOptions() const;

	void checkConfigThrow() const;

	virtual std::vector<std::shared_ptr<restbed::Resource>> getAllResources();


	void addPages(const bfs::path & dir);
	void addScripts(const bfs::path & dir);

	static std::string genHtmlDoc(std::string rName,
			njh::files::FileCache & cache);
};




}  // namespace njhseq


