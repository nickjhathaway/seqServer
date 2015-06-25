
#include "seqServer.h"



class serverRunner : public bib::progutils::programRunner {
 public:
	serverRunner();

};

serverRunner::serverRunner()
    : bib::progutils::programRunner(
          {addFunc("seqViewer", bibseq::seqViewer, false),
					 addFunc("tableViewer", bibseq::tableViewerMain, false)
           },
          "serverRunner") {}

int runServer(int argc, char* argv[]) {
  serverRunner serRunner;
  if (argc > 1) {
    return serRunner.run(argc, argv);
  }
  serRunner.listPrograms(std::cout);
  return 0;
}

int main(int argc, char** argv){
	return runServer(argc, argv);
}


