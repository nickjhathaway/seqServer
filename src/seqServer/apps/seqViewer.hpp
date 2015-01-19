#pragma once
/*
 * seqViewer.hpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nickhathaway
 */

#include "seqServer/apps/seqApp.hpp"


namespace bibseq {

class ssv: public bibseq::seqApp {
private:

	typedef bibseq::seqApp super;

	std::vector<bibseq::readObject> reads_;
	cppcms::json::value readsJson_;
	std::string rootName_;
	bool needsUpdate_ = false;
	bool debug_ = true;
	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	ssv(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const ;

	void seqData();


	void rootName();

	void sort(std::string sortBy);

	void muscleAln();
	void removeGaps();
	void complementSeqs();


	void showMinTree();

	void minTreeData();

	void mainPage();



};

int seqViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


