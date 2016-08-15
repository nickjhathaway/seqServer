//
// seqServer - A library for visualizing sequence results data
// Copyright (C) 2012, 2015 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
// Jeffrey Bailey <Jeffrey.Bailey@umassmed.edu>
//
// This file is part of seqServer.
//
// seqServer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// seqServer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with seqServer.  If not, see <http://www.gnu.org/licenses/>.
//
#include "seqServer.h"



class serverRunner : public bib::progutils::programRunner {
 public:
	serverRunner();

};


serverRunner::serverRunner()
    : bib::progutils::programRunner(
    		std::map<std::string, funcInfo>{
					 addFunc("seqViewer", bibseq::seqViewer, false),
					 addFunc("tableViewer", bibseq::tableViewerMain, false),
					 addFunc("bamBaseViewer", bibseq::bamBaseViewerMain, false)
           },
          "serverRunner", "1", "4", "1-dev") {}

int main(int argc, char** argv){
	try{
	  serverRunner serRunner;
	  return serRunner.run(argc, argv);
	}catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}


