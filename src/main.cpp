#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <seqTools.h>
#include <bibcpp.h>
#include <cppitertools/range.hpp>
#include "file_cache.hpp"
#include "ip.hpp"

namespace bfs = boost::filesystem;

template<typename T>
cppcms::json::value seqsToJson(const std::vector<T> & reads){
  cppcms::json::value ret;
  auto& seqs = ret["seqs"];
  //find number of reads
  ret["numReads"] = reads.size();
  // get the maximum length
  uint32_t maxLen = 0;
  bibseq::readVec::getMaxLength(reads, maxLen);
  ret["maxLen"] = maxLen;
  for(const auto & pos : iter::range(reads.size())){
  	//std::cout << "pos" << std::endl;
    seqs[pos]["seq"] = reads[pos].seqBase_.seq_;
    seqs[pos]["name"] = reads[pos].seqBase_.name_;
    seqs[pos]["qual"] = reads[pos].seqBase_.qual_;
  }
  return ret;
}


cppcms::json::value tableToJsonRowWise(const bibseq::table & tab){
	cppcms::json::value ret;
	auto & outTab = ret["tab"];
	std::unordered_map<uint32_t, bool> numCheck;
	bibseq::VecStr numericCols;
	for(const auto & colPos : iter::range(tab.columnNames_.size())){
		numCheck[colPos] = bibseq::vectorOfNumberStringsDouble(tab.getColumn(colPos));
	}
	for(const auto & rowPos : iter::range(tab.content_.size())){
		for(const auto & colPos : iter::range(tab.columnNames_.size())){
			if(numCheck[colPos]){
				numericCols.emplace_back(tab.columnNames_[colPos]);
				outTab[rowPos][tab.columnNames_[colPos]] = bibseq::lexical_cast<double>(tab.content_[rowPos][colPos]);
			}else{
				outTab[rowPos][tab.columnNames_[colPos]] = tab.content_[rowPos][colPos];
			}
		}
	}
	ret["columnNames"] = tab.columnNames_;
	ret["numericColNames"] = numericCols;
	return ret;
}

cppcms::json::value tableToJsonColumnWise(const bibseq::table & tab){
	cppcms::json::value ret;
	std::unordered_map<uint32_t, bool> numCheck;
	for(const auto & colPos : iter::range(tab.columnNames_.size())){
		if(bibseq::vectorOfNumberStringsDouble(tab.getColumn(colPos))){
			ret[tab.columnNames_[colPos]] = bibseq::lexical_cast_con<std::vector<std::string>,std::vector<double>>(tab.getColumn(colPos));
		}else{
			ret[tab.columnNames_[colPos]] = tab.getColumn(colPos);
		}
	}
	return ret;
}






namespace bibseq{
cppcms::json::value dotToJson(const std::string& dotFilename){
	std::ifstream dot(dotFilename);
	std::unordered_map<std::string, uint32_t> nameIndex;
  cppcms::json::value graph;
  auto& nodes = graph["nodes"];
  auto& links = graph["links"];
  uint32_t nodeCount = 0;
  uint32_t linkCount = 0;
	if(dot){
		for(std::string line; std::getline(dot, line);){
			if(line.find("[") != std::string::npos &&
					line.find("]") != std::string::npos){
				if(line.find("--") != std::string::npos){
					//link
					auto nameSplit = tokenizeString(line, "[");
					nameSplit.back().erase(nameSplit.back().end() - 1);
					auto nameSubSplit = tokenizeString(nameSplit.front(), "--");
					links[linkCount]["source"] = nameIndex[trimEndWhiteSpaceReturn(nameSubSplit.front())];
					links[linkCount]["target"] = nameIndex[trimEndWhiteSpaceReturn(nameSubSplit.back())];
					links[linkCount]["value"] = 1;
					auto attrSplit = tokenizeString(nameSplit.back(), ",");
					std::unordered_map<std::string, std::string> attrs;
					for(const auto at : attrSplit){
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] = trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for(const auto &at : attrs){
						if(at.first == "color"){
							links[linkCount]["color"] = replaceString(trimEndWhiteSpaceReturn(at.second), "\"", "");
						}
					}
					++linkCount;
				}else{
					//node
					//add name to index for links' target
					auto nameSplit = tokenizeString(line, "[");
					nodes[nodeCount]["name"] = trimEndWhiteSpaceReturn(nameSplit.front());
					//std::cout << trimEndWhiteSpaceReturn(nameSplit.front()) << "\n";
					nameIndex[trimEndWhiteSpaceReturn(nameSplit.front())] = nodeCount ;
					nameSplit.back().erase(nameSplit.back().end() - 1);
					auto attrSplit = tokenizeString(nameSplit.back(), ",");
					std::unordered_map<std::string, std::string> attrs;
					for(const auto at : attrSplit){
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] = trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for(const auto &at : attrs){
						if(at.first == "fillcolor"){
							nodes[nodeCount]["color"] = replaceString(trimEndWhiteSpaceReturn(at.second), "\"", "");
						}else if (at.first == "width"){
							nodes[nodeCount]["size"] = bib::lexical_cast<double, std::string>(trimEndWhiteSpaceReturn(at.second)) ;
						}
					}
					++nodeCount;
				}
			}
		}
	}else {
		std::cerr << bib::bashCT::bold
				<< bib::bashCT::red
				<< "Error in opening " << dotFilename <<
				bib::bashCT::reset << std::endl;
	}
	return graph;
}
}  //namespace bib

class ssv: public cppcms::application {
private:

    utils::FileCache mainPageHtml_;
    utils::FileCache oneGeneInfoHtml_;
    utils::FileCache oneMipInfoHtml_;
    utils::FileCache allSampsInfoHtml_;
    utils::FileCache oneSampInfoHtml_;
    utils::FileCache minTreeViewHtml_;
    utils::FileCache popInfoHtml_;


    utils::FilesCache jsLibs_;
    utils::FilesCache jsOwn_;

    utils::FilesCache cssLibs_;
    utils::FilesCache cssOwn_;


    std::string fastqFilename_;
    std::string currentMipName_;
    std::string currentSampName_;
    std::unordered_map<std::string,std::vector<bibseq::readObject>> reads_;
    std::unordered_map<std::string, std::string> readsLocations_;

    std::unordered_map<std::string,std::unordered_map<std::string,std::vector<bibseq::readObject>>> initialReads_;
    std::unordered_map<std::string,std::unordered_map<std::string, std::string>> initialReadsLocations_;

    std::unordered_map<std::string,std::unordered_map<std::string,std::vector<bibseq::readObject>>> finalReads_;
    std::unordered_map<std::string,std::unordered_map<std::string, std::string>> finalReadsLocations_;

    std::unordered_map<std::string,std::unordered_map<std::string, std::string>> dotFilesLocations_;

    std::unordered_map<std::string, std::string> popInfoLocations_;
    std::unordered_map<std::string, std::string> allInfoLocations_;
    std::string rootName_;
    std::string clusteringDir_;
    std::unordered_map<std::string, std::vector<bfs::path>> files_;

    static bfs::path make_path(const bfs::path fn){
        return fn; // TODO: make absolute path, or from config file?
    }

    template <typename T> void dispMapRoot(T func){
        dispatcher().assign("", func, this);
        mapper().assign("");
    }

    template <typename T> void dispMap(T func, std::string n){
        dispatcher().assign("/" + n, func, this);
        mapper().assign(n, "/" + n);
    }

    template <typename T> void dispMap_1arg(T func, std::string n, std::string r){
        dispatcher().assign("/" + n + "/" + r, func, this, 1);
        mapper().assign(n, "/" + n + "/{1}");
    }

    template <typename T> void dispMap_2arg(T func, std::string n, std::string r){
        dispatcher().assign("/" + n + "/" + r, func, this, 1, 2);
        mapper().assign(n, "/" + n + "/{1}/{2}");
    }

    void ret_json(){
        response().content_type("application/json");
    }

    void ret_js(){
        response().content_type("text/javascript");
    }

    void ret_css(){
        response().content_type("text/css");
    }

public:
    ssv(cppcms::service& srv, std::string name,
    		std::string clusDir)
        : cppcms::application(srv)
        , mainPageHtml_(make_path("../resources/mainPage.html"))
    		, oneGeneInfoHtml_(make_path("../resources/oneGeneView.html"))
				, oneMipInfoHtml_(make_path("../resources/oneMipInfo.html"))
				, allSampsInfoHtml_(make_path("../resources/allSampsInfo.html"))
				, oneSampInfoHtml_(make_path("../resources/oneSampInfo.html"))
    		, minTreeViewHtml_(make_path("../resources/minTreeView.html"))
    		, popInfoHtml_(make_path("../resources/popInfo.html"))
    		, jsLibs_(std::vector<bfs::path>{make_path("../resources/js/libs/d3/d3.v3.min.js"),
    																		 make_path("../resources/js/libs/c3/c3.min.js"),
    																		 make_path("../resources/js/libs/canvas2svg.js"),
    																		 make_path("../resources/js/libs/rgbcolor.js"),
    																		 make_path("../resources/js/libs/jspdf.min.js"),
    																		 make_path("../resources/js/libs/svgToPdf.js"),
    																		 make_path("../resources/js/libs/underscore-min.js")})
        , jsOwn_(std::vector<bfs::path>{make_path("../resources/js/own/utils.js"),
																				make_path("../resources/js/own/tableFuncs.js"),
																				make_path("../resources/js/own/SeqView.js"),
																				make_path("../resources/js/own/minSpanTree.js")})
    		, cssLibs_(std::vector<bfs::path>{make_path("../resources/css/libs/c3.css")})
    		, cssOwn_(std::vector<bfs::path>{make_path("../resources/css/own/SeqView.css")})
    		, rootName_(name)
    		, clusteringDir_(clusDir)
    {
    	mainPageHtml_.replaceStr("/ssv", name);
    	oneGeneInfoHtml_.replaceStr("/ssv", name);
    	oneMipInfoHtml_.replaceStr("/ssv", name);
    	allSampsInfoHtml_.replaceStr("/ssv", name);
    	oneSampInfoHtml_.replaceStr("/ssv", name);
    	minTreeViewHtml_.replaceStr("/ssv", name);
    	popInfoHtml_.replaceStr("/ssv", name);
    	//main page
      dispMapRoot(&ssv::mainPage);
      dispMap(&ssv::geneNames, "geneNames");
      //gene page
      dispMap_1arg(&ssv::showGeneInfo, "geneInfo", "(\\w+)");
      dispMap_1arg(&ssv::mipNames, "mipNames", "(\\w+)");
      //show one mip target info and sample names
      dispMap_1arg(&ssv::showMipInfo, "mipInfo", "(\\w+)");
      dispMap_1arg(&ssv::mipSampleNames, "mipSampleNames", "(\\w+)");
      //show the data table with all sample information
      dispMap_1arg(&ssv::showAllSampInfo, "allSamps", "(\\w+)");
      dispMap_2arg(&ssv::allSampsInfoData, "allSampsInfo", "(\\w+)/(\\w+)");
      //show the mip target info for one sample
      dispMap_2arg(&ssv::showOneSampleInfo, "oneSampInfo", "(\\w+)/(\\w+)");
      dispMap_2arg(&ssv::oneSampInitSeqData, "oneSampInitSeqData", "(\\w+)/(\\w+)");
      dispMap_2arg(&ssv::oneSampFinalSeqData, "oneSampFinalSeqData", "(\\w+)/(\\w+)");
      dispMap_2arg(&ssv::oneSampTabData, "oneSampTabData", "(\\w+)/(\\w+)");
      //show the minimum spanning tree for one sample info
      dispMap_2arg(&ssv::showMinTree, "showMinTree", "(\\w+)/(\\w+)");
      dispMap_2arg(&ssv::minTreeData, "minTreeData", "(\\w+)/(\\w+)");
      //show the Population information for one mip target
      dispMap_1arg(&ssv::showPopData, "pop", "(\\w+)");
      dispMap_1arg(&ssv::popInfoData, "popInfo", "(\\w+)");
      dispMap_1arg(&ssv::popSeqData, "popSeqData", "(\\w+)");

      //general information
      dispMap(&ssv::currentSampName, "currentSampName");
      dispMap(&ssv::rootName, "rootName");
      dispMap(&ssv::currentPopName, "currentPopName");
      dispMap(&ssv::colorsData, "baseColors");
      dispMap_1arg(&ssv::getColors, "getColors", "(\\d+)");
      //js and css loading
      dispMap(&ssv::jsLibs, "jsLibs");
      dispMap(&ssv::jsOwn, "jsOwn");
      dispMap(&ssv::cssLibs, "cssLibs");
      dispMap(&ssv::cssOwn, "cssOwn");



      mapper().root(name);

    	auto files = bib::files::listAllFiles(clusDir, true, bibseq::VecStr {"analysis"});
    	for(const auto & f : files){
    		auto toks = bibseq::tokenizeString(f.first.string(), "/");
    		auto aPos = bibseq::find(toks,"analysis" );
    		--aPos;
    		files_[*aPos].emplace_back(f.first);
    		//std::cout << (*aPos) + "_clustered.fastq" << std::endl;
    		if(f.first.string().find((*aPos) + ".fastq" ) != std::string::npos &&
    				f.first.string().find("population")!= std::string::npos){
    			readsLocations_[*aPos] = f.first.string();
    		}else if (f.first.string().find("selectedClustersInfo.tab.txt")!= std::string::npos){
    			allInfoLocations_[*aPos] = f.first.string();
    		}else if (f.first.string().find("populationCluster.tab.txt")!= std::string::npos){
    			popInfoLocations_[*aPos] = f.first.string();
    		}else if (f.first.string().find("originals") != std::string::npos){
    			std::string samp = bibseq::removeExtention(toks.back());
      		initialReadsLocations_[*aPos][samp] = f.first.string();
      		//std::cout << f.first.string() << std::endl;
    		}else if (f.first.string().find("final") != std::string::npos){
    			std::string samp = bibseq::removeExtention(toks.back());
      		finalReadsLocations_[*aPos][samp] = f.first.string();
      		//std::cout << f.first.string() << std::endl;
    		}else if (f.first.string().find(".dot") != std::string::npos &&
    				f.first.string().find(".pdf") == std::string::npos){
      		std::string samp = bibseq::removeExtention(toks.back());
      		dotFilesLocations_[*aPos][samp] = f.first.string();
    		}
    	}
    }

    void currentPopName(){
    	ret_json();
    	cppcms::json::value r;
    	r = currentMipName_;
    	response().out() << r;
    }

    void currentSampName(){
    	ret_json();
    	cppcms::json::value r;
    	r = currentSampName_;
    	response().out() << r;
    }

    void rootName(){
    	ret_json();
    	cppcms::json::value r;
    	r = rootName_;
    	response().out() << r;
    }

    void mipNames(std::string geneName){
    	ret_json();
    	cppcms::json::value ret;
    	auto mipNames = bibseq::getVectorOfMapKeys(files_);
    	bibseq::sort(mipNames);
    	uint32_t count = 0;
    	for(const auto & mPos : iter::range(mipNames.size())){
    		if(bibseq::beginsWith(mipNames[mPos], geneName)){
    			ret[count] = mipNames[mPos];
    			++count;
    		}
    	}
    	response().out() << ret;
    }

    void geneNames(){
    	ret_json();
    	cppcms::json::value ret;
    	auto mipNames = bibseq::getVectorOfMapKeys(files_);
    	bibseq::sort(mipNames);
    	std::set<std::string> geneNames;
    	for(const auto & mPos : iter::range(mipNames.size())){
    		geneNames.emplace(mipNames[mPos].substr(0,mipNames[mPos].find_last_of("_")));
    	}
    	for(const auto & gEnum : iter::enumerate(geneNames)){
    		ret[gEnum.index] = gEnum.element;
    	}
    	response().out() << ret;
    }

    void showPopData(std::string mipName){
    	currentMipName_ = mipName;
    	response().out() << popInfoHtml_.get();
    }

    void showGeneInfo(std::string geneName){
    	response().out() << oneGeneInfoHtml_.get();
    }

    void showMipInfo(std::string mipName){
    	currentMipName_ = mipName;
    	response().out() << oneMipInfoHtml_.get();
    }

    void showOneSampleInfo(std::string mipName ,std::string sampName){
    	//std::cout << "here" << std::endl;
    	currentMipName_ = mipName;
    	currentSampName_ = sampName;
    	std::cout << currentMipName_ << " " << currentSampName_ << std::endl;
    	response().out() << oneSampInfoHtml_.get();
    }

    void showAllSampInfo(std::string mipName){
    	currentMipName_ = mipName;
    	response().out() << allSampsInfoHtml_.get();
    }

    void showMinTree(std::string mipName, std::string sampname){
    	currentMipName_ = mipName;
    	currentSampName_ = sampname;
    	response().out() << minTreeViewHtml_.get();
    }

    void popInfoData(std::string mipName){
    	ret_json();
    	auto ret = tableToJsonRowWise(bibseq::table(popInfoLocations_[mipName], "\t", true));
      response().out() << ret;
    }

    void getColors(std::string num){
    	ret_json();
    	cppcms::json::value ret;
    	auto outColors = bibseq::getColsBetweenInc(120,420,.40, .70, .8, 1,std::stoi(num));
    	bibseq::VecStr outColorsStrs;
    	outColorsStrs.reserve(outColors.size());
    	for(const auto & c : outColors){
    		outColorsStrs.emplace_back("#" + c.hexStr_);
    	}
    	ret["colors"] = outColorsStrs;
      response().out() << ret;
    }

    void allSampsInfoData(std::string mipName, std::string sampNames){
    	ret_json();
    	auto sampTab = bibseq::table(allInfoLocations_[mipName], "\t", true);
    	auto sampToks = bibseq::tokenizeString(sampNames, "DELIM");
    	auto containsSampName = [&sampToks](const std::string & str){
    		return bibseq::in(str, sampToks);
    	};
    	//std::cout << bibseq::vectorToString(sampToks,",")<< std::endl;
    	auto trimedTab = sampTab.extractByComp("Sample", containsSampName);
    	auto ret = tableToJsonRowWise(trimedTab);
    	auto popCounts = bibseq::countVec(trimedTab.getColumn("popUID"));
    	auto popColors = bibseq::getColsBetweenInc(120,420,.40, .70, .8, 1, popCounts.size());

    	bibseq::VecStr popColorsStrs(popColors.size());
    	uint32_t count = 0;
    	uint32_t halfCount = 0;
    	//std::cout << popColorsStrs.size() << std::endl;
    	for(const auto & cPos : iter::range(popColors.size())){

    		uint32_t pos = 0;
    		if(cPos %2 == 0){
    			pos = popColors.size()/2 + halfCount;
    			++halfCount;
    		}else{
    			pos = count;
    			++count;
    		}
    		//std::cout << "\tcPos:" << cPos << std::endl;
    		//std::cout << "\tpos:" << pos << std::endl;
    		popColorsStrs[cPos] = "#" + popColors[pos].hexStr_;
    	}
    	//auto gen = bibseq::randomGenerator();

    	//bibseq::shuffle(popColorsStrs, gen.mtGen_);
    	ret["popColors"] = popColorsStrs;

      response().out() << ret;
    }
    void mipSampleNames(std::string mipName){
    	ret_json();
    	cppcms::json::value ret;
    	auto sampNames =  bibseq::getVectorOfMapKeys(dotFilesLocations_[mipName]);
    	bibseq::sort(sampNames);
    	ret = sampNames;
      response().out() << ret;
    }


    void colorsData(){
      ret_json();
      cppcms::json::value r;
      r["A"] = "#ff8787";
      r["a"] = "#ff8787";

      r["C"] = "#afffaf";
      r["c"] = "#afffaf";

      r["G"] = "#ffffaf";
      r["g"] = "#ffffaf";

      r["T"] = "#87afff";
      r["t"] = "#87afff";

      r["-"] = "e6e6e6";


      response().out() << r;
    }


    void popSeqData(std::string mipName){
    	currentMipName_ = mipName;
      ret_json();
      //if reads haven't been read yet, read them in
      if(reads_.find(mipName) == reads_.end()){
      	bibseq::readObjectIO reader;
      	reader.read("fastq", readsLocations_[mipName],false);
      	reads_[mipName] = reader.reads;
      }
      response().out() << seqsToJson(reads_[mipName]);
    }

    void minTreeData(std::string mipName, std::string sampname){
    	currentSampName_ = sampname;
    	currentMipName_ = mipName;
    	auto ret = bibseq::dotToJson(dotFilesLocations_[mipName][sampname]);
    	ret_json();
    	response().out() << ret;
    }

    void oneSampTabData(std::string mipName, std::string sampname){
    	currentMipName_ = mipName;
    	currentSampName_ = sampname;
    	ret_json();
    	auto tab = bibseq::table(allInfoLocations_[mipName], "\t", true);
    	auto outTab = tab.getRows("sName", sampname);
    	auto ret = tableToJsonRowWise(outTab);
      response().out() << ret;
    }

    void oneSampInitSeqData(std::string mipname, std::string sampname){
    	currentSampName_ = sampname;
      ret_json();
      //if reads haven't been read yet, read them in
      if(initialReads_[mipname][sampname].empty()){
      	bibseq::readObjectIO reader;
      	reader.read("fastq", initialReadsLocations_[mipname][sampname],false);
      	initialReads_[mipname][sampname] = reader.reads;
      }

      response().out() << seqsToJson(initialReads_[mipname][sampname]);
    }

    void oneSampFinalSeqData(std::string mipname, std::string sampname){
    	currentSampName_ = sampname;
      ret_json();
      //if reads haven't been read yet, read them in
      if(finalReads_[mipname][sampname].empty()){
      	bibseq::readObjectIO reader;
      	reader.read("fastq", finalReadsLocations_[mipname][sampname],false);
      	finalReads_[mipname][sampname] = reader.reads;
      }
      response().out() << seqsToJson(finalReads_[mipname][sampname]);
    }

    void mainPage(){
      response().out() << mainPageHtml_.get();
    }

    void jsLibs(){
      ret_js();
      response().out() << jsLibs_.get();
    }

    void jsOwn(){
      ret_js();
      response().out() << jsOwn_.get();
    }

    void cssLibs(){
    	ret_css();
      response().out() << cssLibs_.get();
    }

    void cssOwn(){
      ret_css();
      response().out() << cssOwn_.get();
    }

};



cppcms::json::object server_config(std::string name, uint32_t port){
  cppcms::json::object args;
  args["service"]["api"] = "http";
  args["service"]["port"] = port;
  args["service"]["ip"] = "0.0.0.0";
  args["http"]["script"] = name;
  return args;
}


int main(int argc, char** argv){
	bibseq::seqSetUp setUp(argc, argv);
	std::string clusDir = "";
	uint32_t port = 8881;
	std::string name = "ssv";
	setUp.setOption(clusDir, "-clusDir", "Name of the Master Result Directory", true);
	setUp.setOption(port, "-port", "Port Number to Serve On");
	setUp.setOption(name, "-name", "Nmae of root of the server");
	setUp.finishSetUp(std::cout);
	name = "/" + name;
  auto config = server_config(name, port);

  try {
      cppcms::service app(config);
      app.applications_pool().mount(cppcms::applications_factory<ssv>(name, clusDir));
      app.run();
  } catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }
}


