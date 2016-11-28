

/*
 * SeqServerRunner.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nick
 */




#include "SeqServerRunner.hpp"

namespace bibseq {




SeqServerRunner::SeqServerRunner()
    : bib::progutils::oneRing(
    		{
					addRing<SeqViewerRunner>()
    		},
				{

				},//
				"SeqServerRunner", "1", "4", "1-dev") {}

}  // namespace bibseq


