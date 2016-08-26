#pragma once
/*
 * SeqViewerRestbed.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: nick
 */


#include "seqServer/apps/SeqAppRestbed.hpp"


namespace bibseq {

class SeqViewerRestbed: public SeqAppRestbed {
public:

	using SeqAppRestbed::SeqAppRestbed;
};


int seqViewerRestbed(const bib::progutils::CmdArgs & inputCommands);

} /* namespace bibseq */

