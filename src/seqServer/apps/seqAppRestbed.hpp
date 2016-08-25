#pragma once
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
/*
 * seqApp.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */
#include <restbed>
#include <bibseq.h>
#include <bibcpp.h>
#include <cppitertools/range.hpp>
#include "seqServer/utils.h"
#include "seqServer/objects/seqCache.hpp"

namespace bibseq {

class seqAppRestbed {
	/*
protected:

	template<typename C, typename T> void dispMapRoot(T func, C* classCaller) {
		dispatcher().assign<C>("", func, classCaller);
		mapper().assign("");
	}

	template<typename C, typename T> void dispMap(T func, C* classCaller,
			std::string n) {
		dispatcher().assign("/" + n, func, classCaller);
		mapper().assign(n, "/" + n);
	}

	template<typename C, typename T> void dispMap_1arg(T func, C* classCaller,
			std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1);
		mapper().assign(n, "/" + n + "/{1}");
	}

	template<typename C, typename T> void dispMap_2arg(T func, C* classCaller,
			std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1, 2);
		mapper().assign(n, "/" + n + "/{1}/{2}");
	}


	template<typename C, typename T> void dispMap_3arg(T func, C* classCaller,
			std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1, 2, 3);
		mapper().assign(n, "/" + n + "/{1}/{2}/{3}");
	}


	template<typename C, typename T> void dispMap_4arg(T func, C* classCaller,
			std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1, 2, 3, 4);
		mapper().assign(n, "/" + n + "/{1}/{2}/{3}/{4}");
	}


	template<typename C, typename T> void dispMap_1word(T func, C* classCaller,
			std::string n) {
		dispatcher().assign("/" + n + "/" + wordWithDash_, func, classCaller, 1);
		mapper().assign(n, "/" + n + "/{1}");
	}

	template<typename C, typename T> void dispMap_2word(T func, C* classCaller,
			std::string n) {
		dispatcher().assign("/" + n + "/" + twoWordArgs_, func, classCaller, 1, 2);
		mapper().assign(n, "/" + n + "/{1}/{2}");
	}


	template<typename C, typename T> void dispMap_3word(T func, C* classCaller,
			std::string n) {
		dispatcher().assign("/" + n + "/" + threeWordArgs_, func, classCaller, 1, 2,
				3);
		mapper().assign(n, "/" + n + "/{1}/{2}/{3}");
	}


	template<typename C, typename T> void dispMap_4word(T func, C* classCaller,
			std::string n) {
		dispatcher().assign("/" + n + "/" + fourWordArgs_, func, classCaller, 1, 2,
				3, 4);
		mapper().assign(n, "/" + n + "/{1}/{2}/{3}/{4}");
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

	void colorsData();
	void getProteinColors();

	void getColors(std::string num);

	std::map<std::string, bib::files::FileCache> pages_;

	std::map<std::string, bib::files::FilesCache> jsAndCss_;

	const std::string wordWithDash_ =  "([A-Za-z0-9\\-\\_\\.]+)";
	const std::string twoWordArgs_ =   "([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)";
	const std::string threeWordArgs_ = "([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)";
	const std::string fourWordArgs_ =  "([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)/([A-Za-z0-9\\-\\_\\.]+)";

public:
	seqApp(cppcms::service& srv, std::map<std::string, std::string> config);

	std::shared_ptr<seqCache> seqs_;

	bool debug_ = false;
	virtual ~seqApp();

	virtual VecStr requiredOptions() const;


	static bool configTest(const MapStrStr & config, const VecStr & checks,
			const std::string nameOfClass);

	void jsLibs();
	void jsOwn();
	void cssLibs();
	void cssOwn();

	void sort(std::string sortBy);
	void muscleAln();
	void removeGaps();
	void complementSeqs();
	void translateToProtein();
	void minTreeDataDetailed();

	void addPages(const bfs::path & dir);

	std::string messStrFactory(const std::string & funcName);
	std::string messStrFactory(const std::string & funcName,
			const MapStrStr & args);
			*/
};

} /* namespace bibseq */

