#pragma once
/*
 * popClusterViewer.hpp
 *
 *  Created on: Jan 13, 2015
 *      Author: nickhathaway
 */
#include "seqServer/apps/seqApp.hpp"
#include "seqServer/utils.h"
#include <bibcpp.h>



namespace bibseq {
namespace bfs = boost::filesystem;

class pcv: public bibseq::seqApp {
private:



	table sampTable_;
	table popTable_;
	std::vector<readObject> popReads_;
	VecStr sampleNames_;
	std::string projectName_;
	std::string rootName_;
	std::string mainDir_;



	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	pcv(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const{
		return VecStr{"dir", "resources", "projectName"};
	}
	//html
	//main page
	void mainPage();

	//json
	void getProjectName();
	void getSampleNames();
	void getSampInfo(std::string sampNames);
	void getPosSeqData();
	void getPopInfo();

};

int popClusteringViewer(std::map<std::string, std::string> inputCommands);


} /* namespace bibseq */


