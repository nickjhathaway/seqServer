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


	struct popInfo {
		popInfo(){}
		popInfo(const table & sampTable, const table & popTable,
				const std::vector<readObject> & popReads,
				const std::vector<readObject> & popReadsTranslated) :
				sampTable_(sampTable), popTable_(popTable), popReads_(popReads), popReadsTranslated_(
						popReadsTranslated) {
			clusteredSampleNames_ = sampTable_.getColumnLevels("s_Name");
			bib::sort(clusteredSampleNames_);
		}
		table sampTable_;
		table popTable_;
		std::vector<readObject> popReads_;
		std::vector<readObject> popReadsTranslated_;
		VecStr clusteredSampleNames_;
	};

	table sampTable_;
	table popTable_;
	VecStr clusteredSampleNames_;
	double fracCutOff_ = 0;

	std::unordered_map<std::string, std::unordered_map<std::string, popInfo>> groupInfos_;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> groupInfosDirNames_;

	VecStr allSampleNames_;
	std::string projectName_;

	std::string rootName_;
	std::string mainDir_;


	ExtractionInfo extractInfo_;

	std::map<std::string, std::string> config_;


	std::unordered_map<std::string, Json::Value> sampleMinTreeDataCache_;
	Json::Value minTreeData_;
	bool calculatedTreeData_ = false;

	std::unordered_map<std::string, uint32_t> sampNameToCodedNum_;
	std::unordered_map<uint32_t, std::string> codedNumToSampName_;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	pcvExp(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const{
		return VecStr{"mainDir", "resources", "projectName"};
	}

	void loadInPopSeqs();

	//html
	//main page
	void mainPage();
	void individualSamplePage(std::string sampName);

	//json
	void getProjectName();
	void getSampleNames();
	void getAllSampleNames();
	void getSampleNamesEncoding();

	void getEncodingForSampleNames();

	std::string decodeSampEncoding(const std::string& sampName);
	void getSampInfo(std::string sampNames);
	void getPosSeqData();
	void getPopInfo();

	void getPopProtenData();


	void showExtractionInfo();
	void getIndexExtractionInfo();
	void getSampleExtractionInfo(std::string sampNames);


	void showMinTree();
	void getMinTreeData();

	void getSeqData(std::string sampName);
	void getProteinData(std::string sampName);

	void showMinTreeForSample(std::string sampName);
	void getMinTreeDataForSample(std::string sampName);

	//group info
	void getGroupPopInfo(std::string group, std::string subGroup);
	void getGroupPopSeqData(std::string group, std::string subGroup);
	void getGroupPopProtenData(std::string group, std::string subGroup);
	void getGroupSampInfo(std::string group, std::string subGroup, std::string sampName);
	void getGroupSampleNames(std::string group, std::string subGroup);

	void showGroupMainPage(std::string group, std::string subGroup);
	void showSubGroupsPage(std::string group);
	void getSubGroupsForGroup(std::string group);

	void getGroupNames();
	bool setUpGroup(std::string group, std::string subGroup);


	void getGroupPopInfos(std::string group);

};

int popClusteringViewerExp(std::map<std::string, std::string> inputCommands);


} /* namespace bibseq */
