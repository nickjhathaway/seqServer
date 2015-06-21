#pragma once
/*
 * viewTable.hpp
 *
 *  Created on: Jun 18, 2015
 *      Author: nickhathaway
 */

#include "seqServer/apps/seqApp.hpp"


namespace bibseq {

class tableViewer: public bibseq::seqApp {
private:

	typedef bibseq::seqApp super;

	std::string rootName_;
	table originalTable_;
	table updatedTable_;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

public:
	tableViewer(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const ;

	void tableData();

	void updatedTableData();

	void updateTable();

	void rootName();


	void mainPage();



};

int tableViewerMain(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


