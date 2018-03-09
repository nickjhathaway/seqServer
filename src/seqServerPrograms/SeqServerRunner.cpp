

/*
 * SeqServerRunner.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nick
 */




#include "SeqServerRunner.hpp"

namespace bibseq {




SeqServerRunner::SeqServerRunner()
    : bib::progutils::OneRing(
    		{
					addRing<SeqViewerRunner>()
    		},
				{

				},//
				"SeqServerRunner", "1", "5", "2") {}

}  // namespace bibseq

//bib::progutils::oneRing
