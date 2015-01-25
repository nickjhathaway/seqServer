
/*
 * mipPreViewer.hpp
 *
 *  Created on: Jan 22, 2015
 *      Author: nickhathaway
 */



#include "seqServer/apps/seqApp.hpp"


namespace bibseq {

class mpv : public bibseq::seqApp {

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
	mpv(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual VecStr requiredOptions() const;

	void rootName();

	void mainPage();

};

int mipPreViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */




