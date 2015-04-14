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

class pcvExp: public bibseq::seqApp {
private:



	table sampTable_;
	table popTable_;
	std::vector<readObject> popReads_;
	std::vector<readObject> popReadsTranslated_;
	VecStr sampleNames_;
	std::string projectName_;
	std::string rootName_;
	std::string mainDir_;

	Json::Value minTreeData_;
	bool calculatedTreeData_ = false;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	pcvExp(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const{
		return VecStr{"mainDir", "resources", "projectName"};
	}
	//html
	//main page
	void mainPage();
	void individualSamplePage(std::string sampName);

	//json
	void getProjectName();
	void getSampleNames();
	void getSampInfo(std::string sampNames);
	void getPosSeqData();
	void getPopInfo();

	void getPopProtenData();
	void getProteinColors();

	void showMinTree();
	void getMinTreeData();

	void getSeqData(std::string sampName);
	void getProteinData(std::string sampName);

	void showMinTreeForSample(std::string sampName);
	void getMinTreeDataForSample(std::string sampName);

};

int popClusteringViewerExp(std::map<std::string, std::string> inputCommands);


} /* namespace bibseq */
