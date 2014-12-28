#pragma once
/*
 * minTreeViewer.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "apps/seqApp.hpp"
#include "utils.h"

namespace bibseq {
namespace bfs = boost::filesystem;
class mtv: public cppcms::application {
private:

	bib::FileCache mainPageHtml_;
	bib::FileCache minTreeViewHtml_;

	bib::FilesCache jsLibs_;
	bib::FilesCache jsOwn_;

	bib::FilesCache cssLibs_;
	bib::FilesCache cssOwn_;

	std::string rootName_;
	std::string dir_;
	std::unordered_map<std::string, std::map<std::string, bfs::path>> files_;

	static bfs::path make_path(const bfs::path fn) {
		return fn;
	}

	template<typename T> void dispMapRoot(T func) {
		dispatcher().assign("", func, this);
		mapper().assign("");
	}

	template<typename T> void dispMap(T func, std::string n) {
		dispatcher().assign("/" + n, func, this);
		mapper().assign(n, "/" + n);
	}

	template<typename T> void dispMap_1arg(T func, std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, this, 1);
		mapper().assign(n, "/" + n + "/{1}");
	}

	template<typename T> void dispMap_2arg(T func, std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, this, 1, 2);
		mapper().assign(n, "/" + n + "/{1}/{2}");
	}

	void ret_json() {
		response().content_type("application/json");
	}

	void ret_js() {
		response().content_type("text/javascript");
	}

	void ret_css() {
		response().content_type("text/css");
	}

public:
	mtv(cppcms::service& srv, std::string name, std::string dir) :
			cppcms::application(srv), mainPageHtml_(
					make_path("../resources/mtv/mainPage.html")), minTreeViewHtml_(
					make_path("../resources/mtv/minTreeView.html")), jsLibs_(
					std::vector<bfs::path> { make_path(
							"../resources/js/libs/d3/d3.v3.min.js"), make_path(
							"../resources/js/libs/c3/c3.min.js"), make_path(
							"../resources/js/libs/bootstrap/bootstrap.min.js"), make_path(
							"../resources/js/libs/canvas2svg.js"), make_path(
							"../resources/js/libs/rgbcolor.js"), make_path(
							"../resources/js/libs/jspdf.min.js"), make_path(
							"../resources/js/libs/svgToPdf.js"), make_path(
							"../resources/js/libs/d3.legend.js"), make_path(
							"../resources/js/libs/underscore-min.js") }), jsOwn_(
					std::vector<bfs::path> { make_path("../resources/js/own/utils.js"),
							make_path("../resources/js/own/tableFuncs.js"), make_path(
									"../resources/js/own/SeqView.js"), make_path(
									"../resources/js/own/minSpanTree.js") }), cssLibs_(
					std::vector<bfs::path> { make_path("../resources/css/libs/c3.css"),
							make_path("../resources/css/libs/bootstrap.min.css"), make_path(
									"../resources/css/libs/bootstrap-theme.min.css") }), cssOwn_(
					std::vector<bfs::path> { make_path("../resources/css/own/SeqView.css") }), rootName_(
					name), dir_(dir) {
		mainPageHtml_.replaceStr("/ssv", name);
		minTreeViewHtml_.replaceStr("/ssv", name);
		//main page
		dispMapRoot(&mtv::mainPage);
		dispMap(&mtv::sampleNames, "sampleNames");
		dispMap_1arg(&mtv::treeNames, "treeNames", "(\\w+)");
		//show the minimum spanning tree for one sample info
		dispMap_1arg(&mtv::showMinTree, "showMinTree", "(\\w+)");
		dispMap_1arg(&mtv::minTreeData, "minTreeData", "(\\w+)");
		//js and css loading
		dispMap(&mtv::jsLibs, "jsLibs");
		dispMap(&mtv::jsOwn, "jsOwn");
		dispMap(&mtv::cssLibs, "cssLibs");
		dispMap(&mtv::cssOwn, "cssOwn");
		mapper().root(name);
		auto files = bib::files::listAllFiles(dir_, false,
				bibseq::VecStr { ".dot" });
		for (const auto & f : files) {
			auto timePointName = f.first.filename().replace_extension("").string();
			files_[timePointName.substr(0, timePointName.find("_"))].emplace(
					timePointName, f.first);
		}
		/*
		 for(const auto & f : files_){
		 for(const auto & subF : f.second){
		 std::cout << "f: " << f.first << " " << "subF: " << subF.first << " " << subF.second << std::endl;
		 }
		 }*/
		std::cout << "finish set up " << std::endl;
	}

	void sampleNames() {
		std::cout << "sampleNames" << std::endl;
		ret_json();
		cppcms::json::value r;
		auto trees = bib::getVecOfMapKeys(files_);
		std::sort(trees.begin(), trees.end());
		r = trees;
		response().out() << r;
	}

	void treeNames(std::string sampName) {
		std::cout << "treeNames" << std::endl;
		ret_json();
		cppcms::json::value r;
		auto trees = bib::getVecOfMapKeys(files_);
		std::sort(trees.begin(), trees.end());
		r = trees;
		response().out() << r;
	}

	void getColors(std::string num) {
		ret_json();
		cppcms::json::value ret;
		auto outColors = bib::njhColors(std::stoi(num));
		bibseq::VecStr outColorsStrs;
		outColorsStrs.reserve(outColors.size());
		for (const auto & c : outColors) {
			outColorsStrs.emplace_back("#" + c.hexStr_);
		}
		ret["colors"] = outColorsStrs;
		response().out() << ret;
	}

	void colorsData() {
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

	void showMinTree(std::string mipName) {
		response().out() << minTreeViewHtml_.get("/ssv", rootName_);
	}

	void minTreeData(std::string sampName) {
		cppcms::json::value ret;
		uint32_t count = 0;
		for (const auto & tp : files_[sampName]) {
			ret[count]["treeData"] = bibseq::dotToJson(tp.second.string());
			ret[count]["name"] = tp.first;
			++count;
		}
		ret_json();
		response().out() << ret;
	}

	void mainPage() {
		response().out() << mainPageHtml_.get("/ssv", rootName_);
	}

	void jsLibs() {
		ret_js();
		response().out() << jsLibs_.get();
	}

	void jsOwn() {
		ret_js();
		response().out() << jsOwn_.get();
	}

	void cssLibs() {
		ret_css();
		response().out() << cssLibs_.get();
	}

	void cssOwn() {
		ret_css();
		response().out() << cssOwn_.get();
	}
};

int minTreeViewer(std::map<std::string, std::string> inputCommands);

} /* namespace bibseq */


