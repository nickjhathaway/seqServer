#pragma once
/*
 * mipViewer.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "seqServer/apps/seqApp.hpp"
#include "seqServer/utils.h"
#include <bibcpp.h>

namespace bibseq {
namespace bfs = boost::filesystem;

uint64_t countFastqSeqs(const std::string & filename, bool processed, bool verbose);

void appendSlash(std::string & dirName);

std::string appendSlashRet(std::string dirName);

template<typename T>
T getSumOfVecStr(const VecStr & vec){
	return vectorSum(bib::lexical_cast_con<VecStr,std::vector<T>>(vec));
}

VecStr processMipExtractInfoFile(table info);

table getSampleStats(const std::string & dirName, bool verbose);

class miv: public bibseq::seqApp {
private:



	//by mip target
	std::unordered_map<std::string, std::vector<bibseq::readObject>> popReads_;
	//by mip target by sample
	std::unordered_map<std::string,
			std::unordered_map<std::string, std::vector<bibseq::readObject>>> clusteredReads_;
	//by mip target by sample
	std::unordered_map<std::string,std::unordered_map<std::string,std::vector<bibseq::readObject>>> filteredReads_;

	//by sample name, master directory for the sample
	std::unordered_map<std::string, bfs::path> sampAnalysisFolders_;
	//the location of the latest mipextraction analysis
	std::unordered_map<std::string, std::unordered_map<std::string, bfs::path>> sampleMipAnalysisFolders_;
	//by mip name, created by processClusteredMipArms
	std::unordered_map<std::string, bfs::path> mipAnalysisFolders_;
	//by mip name, created by SeekDeep processClusters
	std::unordered_map<std::string, bfs::path> mipAnalysisWithOutBarcodesFolders_;
	//by mip name, all sample names for a given mip
	std::unordered_map<std::string, VecStr> sampNamesForMip_;
	std::unordered_map<std::string, VecStr> sampNamesForGene_;
	std::unordered_map<std::string, VecStr> mipNamesForSamp_;

	table stats_;

	std::unordered_map<std::string, bibseq::table> allInfoBySample_;

	std::string rootName_;
	std::string clusteringDir_;
	std::string genomeDir_;


	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	miv(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const{
		return VecStr{"clusDir", "resources", "genomeDir"};
	}

	void getAllSampleNames() ;


	void rootName() ;

	void mipNames(std::string geneName);

	void mipNamesForSample(std::string sampleName);

	void geneNames();

	void showGeneInfo(std::string geneName);

	void showOneSampAllMip(std::string sampName) ;

	void sampMipNamesData(std::string sampName) ;

	void oneSampAllMipData(std::string sampName, std::string mipNames) ;

	void showMipInfo(std::string mipName) ;

	void showOneSampleInfo(std::string mipName ,std::string sampName) ;

	void showAllSampInfo(std::string mipName);

	void showMinTree(std::string mipName, std::string sampname);

	void popInfoData(std::string mipName) ;


	void allSampsInfoData(std::string mipName, std::string sampNames) ;
	void mipSampleNames(std::string mipName) ;

	void getGeneForMipName(std::string mipName);

	void popSeqData(std::string mipName) ;

	void minTreeData(std::string mipName, std::string sampname) ;

	void oneSampTabData(std::string mipName, std::string sampname) ;

	void oneSampInitSeqData(std::string mipName, std::string sampName) ;

	void oneSampFinalSeqData(std::string mipName, std::string sampName) ;

	void mainPage();

	void getInitialReadStats(std::string sampleNames);
	void showInitialReadStats();

	void getInitialReadStatsPerSample(std::string sampName, std::string mipNames);
	void showInitialReadStatsPerSample(std::string sampName);

	// barcode information per sample per mip
	void getBarcodeInfoPerSamplePerMip(std::string mipName, std::string sampName);

	//one gene, one sample data
	void showOneGeneOneSamp(std::string geneName, std::string sampName);
	void oneGeneOneSampAlnData(std::string geneName, std::string sampName);
	void sampNamesForGene(std::string geneName);
};

int mipViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


