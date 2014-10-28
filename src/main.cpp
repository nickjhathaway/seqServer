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

cppcms::json::value tableToJsonRowWise(const bibseq::table & tab){
	cppcms::json::value ret;
	std::unordered_map<uint32_t, bool> numCheck;
	for(const auto & colPos : iter::range(tab.columnNames_.size())){
		numCheck[colPos] = bibseq::vectorOfNumberStringsDouble(tab.getColumn(colPos));
	}
	for(const auto & rowPos : iter::range(tab.content_.size())){
		for(const auto & colPos : iter::range(tab.columnNames_.size())){
			if(numCheck[colPos]){
				ret[rowPos][tab.columnNames_[colPos]] = bibseq::lexical_cast<double>(tab.content_[rowPos][colPos]);
			}else{
				ret[rowPos][tab.columnNames_[colPos]] = tab.content_[rowPos][colPos];
			}
		}
	}
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

bool checkForSubStrs(const std::string & str,
		const std::vector<std::string> & contains){
	for(const auto & s : contains){
		if(str.find(s) != std::string::npos){
			return true;
		}
	}
	return false;
}

void listAllFilesHelper(const boost::filesystem::path & dirName, bool recursive,
		std::map<boost::filesystem::path, bool> & files,
		uint32_t currentLevel,
		uint32_t levels){
	boost::filesystem::path dir(dirName);
	boost::filesystem::directory_iterator end_iter;
	if(boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)){
	  for(boost::filesystem::directory_iterator dir_iter(dir); dir_iter != end_iter ; ++dir_iter){
	  	boost::filesystem::path current = dir_iter->path();
	  	if(boost::filesystem::is_directory(dir_iter->path())){
	  		files[current] = true;
	  		if(recursive && currentLevel <levels){
	  			listAllFilesHelper(current, recursive, files,
	  					currentLevel + 1, levels);
	  		}
	  	}else{
	  		files[current] = false;
	  	}
	  }
	}
}


std::map<boost::filesystem::path, bool> listAllFiles(const std::string & dirName,
		bool recursive,const std::vector<std::string>& contains,
		uint32_t levels = std::numeric_limits<uint32_t>::max()){
	std::map<boost::filesystem::path, bool> files;
	listAllFilesHelper(dirName, recursive, files, 1, levels);
	if(!contains.empty()){
		std::map<boost::filesystem::path, bool> specificFiles;
		for(const auto & f : files){
			if(checkForSubStrs(f.first.string(), contains)){
				specificFiles.emplace(f);
			}
		}
		return specificFiles;
	}
	return files;
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
    utils::FileCache html_;
    utils::FileCache seqViewerHtml_;
    utils::FileCache js_;
    utils::FileCache d3_;
    utils::FileCache c3_;
    utils::FileCache SeqViwer_;
    utils::FileCache c3css_;
    std::string fastqFilename_;
    std::string currentPopName_;
    std::unordered_map<std::string,std::vector<bibseq::readObject>>reads_;
    std::unordered_map<std::string, std::string> readsLocations_;
    std::unordered_map<std::string, std::string> popInfoLocations_;
    std::unordered_map<std::string, std::string> allInfoLocations_;
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
        , html_(make_path("../resources/index.html"))
    		, seqViewerHtml_(make_path("../resources/indexSeqViewerTemplate.html"))
        , js_(make_path("../resources/main.js"))
    		, d3_(make_path("../resources/jsLibs/d3/d3.v3.min.js"))
    		, c3_(make_path("../resources/jsLibs/c3/c3.min.js"))
    		, SeqViwer_(make_path("../resources/SeqViewer.js"))
    		, c3css_(make_path("../resources/css/c3.css"))
    		, clusteringDir_(clusDir)
    {
    	dispMap(&ssv::mainData, "mainData");
    	dispMap(&ssv::mainSeqData, "mainSeqData");
      dispMap(&ssv::colorsData, "baseColors");
      dispMap_1arg(&ssv::showPopData, "pop", "(\\w+)");
      dispMap_1arg(&ssv::showPopInfoData, "popInfo", "(\\w+)");
      dispMap(&ssv::js, "js");
      dispMap(&ssv::d3js, "d3");
      dispMap(&ssv::c3js, "c3");
      dispMap(&ssv::c3css, "c3css");
      dispMap(&ssv::SeqViwer, "SeqViewer");
      dispMap(&ssv::mipNames, "mipNames");
      dispMap(&ssv::currentPopName, "currentPopName");
      dispMapRoot(&ssv::html);
      mapper().root(name);
    	auto files = listAllFiles(clusDir, true, bibseq::VecStr {"analysis"});
    	for(const auto & f : files){
    		auto toks = bibseq::tokenizeString(f.first.string(), "/");
    		auto aPos = bibseq::find(toks,"analysis" );
    		--aPos;
    		files_[*aPos].emplace_back(f.first);
    		if(f.first.string().find((*aPos) + ".fastq" ) != std::string::npos &&
    				f.first.string().find("population")!= std::string::npos){
    			readsLocations_[*aPos] = f.first.string();
    		}else if (f.first.string().find("selectedClustersInfo.tab.txt")!= std::string::npos){
    			allInfoLocations_[*aPos] = f.first.string();
    		}else if (f.first.string().find("populationCluster.tab.txt")!= std::string::npos){
    			popInfoLocations_[*aPos] = f.first.string();
    		}
    	}
    	for(const auto & mip : files_){
    		//std::cout << mip.first << std::endl;
    		for(const auto & p : mip.second){
    			//std::cout << "\t" << p << std::endl;
    		}
    	}
    }

    void currentPopName(){
    	ret_json();
    	cppcms::json::value r;
    	r = currentPopName_;
    	response().out() << r;
    }
    void mainData(){
      ret_json();
      //if reads haven't been read yet, read them in
      if(reads_.find(currentPopName_) == reads_.end()){
      	bibseq::readObjectIO reader;
      	reader.read("fastq", readsLocations_[currentPopName_],false);
      	reads_[currentPopName_] = reader.reads;
      }
      cppcms::json::value r;
      r["numReads"] = reads_[currentPopName_].size();
      uint32_t maxLen = 0;
      bibseq::readVec::getMaxLength(reads_[currentPopName_], maxLen);
      r["maxLen"] = maxLen;
      response().out() << r;
    }

    void mipNames(){
    	ret_json();
    	cppcms::json::value ret;
    	auto mipNames = bibseq::getVectorOfMapKeys(files_);
    	bibseq::sort(mipNames);
    	for(const auto & mPos : iter::range(mipNames.size())){
    		ret[mPos] = mipNames[mPos];
    	}
    	response().out() << ret;
    }

    void showPopData(std::string mipName){
    	currentPopName_ = mipName;
    	response().out() << seqViewerHtml_.get();
    }
    void showPopInfoData(std::string mipName){
    	ret_json();
    	auto ret = tableToJsonRowWise(bibseq::table(popInfoLocations_[mipName], "\t", true));
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

    void mainSeqData(){
      ret_json();
      cppcms::json::value r;
      auto& c = r["seqs"];
      //if reads haven't been read yet, read them in
      if(reads_.find(currentPopName_) == reads_.end()){
      	bibseq::readObjectIO reader;
      	reader.read("fastq", readsLocations_[currentPopName_],false);
      	reads_[currentPopName_] = reader.reads;
      }
      //std::cout << currentPopName_ << std::endl;
      //std::cout << readsLocations_[currentPopName_] << std::endl;
      for(const auto & pos : iter::range(reads_[currentPopName_].size())){
      	//std::cout << "pos" << std::endl;
        c[pos]["seq"] = reads_[currentPopName_][pos].seqBase_.seq_;
        c[pos]["name"] = reads_[currentPopName_][pos].seqBase_.name_;
        c[pos]["qual"] = reads_[currentPopName_][pos].seqBase_.qual_;
      }
      response().out() << r;
    }



    void html(){
      response().out() << html_.get();
    }

    void js(){
      ret_js();
      response().out() << js_.get();
    }
    void d3js(){
      ret_js();
      response().out() << d3_.get();
    }

    void c3js(){
      ret_js();
      response().out() << c3_.get();
    }
    void SeqViwer(){
      ret_js();
      response().out() << SeqViwer_.get();
    }

    void c3css(){
    	ret_css();
      response().out() << c3css_.get();
    }

};



cppcms::json::object server_config(std::string name){
  cppcms::json::object args;
  args["service"]["api"] = "http";
  args["service"]["port"] = 8881;
  args["service"]["ip"] = "0.0.0.0";
  args["http"]["script"] = name;
  return args;
}


int main(int argc, char** argv){
	bibseq::seqSetUp setUp(argc, argv);
	std::string clusDir = "";
	setUp.setOption(clusDir, "-clusDir", "Name of the Master Result Directory", true);
	setUp.finishSetUp(std::cout);
  const std::string name = "/ssv";
  auto config = server_config(name);

  try {
      cppcms::service app(config);
      app.applications_pool().mount(cppcms::applications_factory<ssv>(name, clusDir));
      app.run();
  } catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }
}


