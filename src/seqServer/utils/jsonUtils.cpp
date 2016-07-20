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
 * jsonUtils.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include "jsonUtils.hpp"

namespace bibseq {
Json::Value dotToJson(const std::string& dotFilename) {
	std::ifstream dot(dotFilename);
	std::unordered_map<std::string, uint32_t> nameIndex;
	Json::Value graph;
	auto& nodes = graph["nodes"];
	auto& links = graph["links"];
	uint32_t nodeCount = 0;
	uint32_t linkCount = 0;
	if (dot) {
		for (std::string line; std::getline(dot, line);) {
			if (line.find("[") != std::string::npos
					&& line.find("]") != std::string::npos) {
				if (line.find("--") != std::string::npos) {
					//link
					auto nameSplit = tokenizeString(line, "[");
					nameSplit.back().erase(nameSplit.back().end() - 1);
					auto nameSubSplit = tokenizeString(nameSplit.front(), "--");
					links[linkCount]["source"] = nameIndex[trimEndWhiteSpaceReturn(
							nameSubSplit.front())];
					links[linkCount]["target"] = nameIndex[trimEndWhiteSpaceReturn(
							nameSubSplit.back())];
					links[linkCount]["value"] = 1;
					auto attrSplit = tokenizeString(nameSplit.back(), ",");
					std::unordered_map<std::string, std::string> attrs;
					for (const auto at : attrSplit) {
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] =
								trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for (const auto &at : attrs) {
						if (at.first == "color") {
							links[linkCount]["color"] = bib::replaceString(
									trimEndWhiteSpaceReturn(at.second), "\"", "");
						}
					}
					++linkCount;
				} else {
					//node
					//add name to index for links' target
					auto nameSplit = tokenizeString(line, "[");
					nodes[nodeCount]["name"] = trimEndWhiteSpaceReturn(nameSplit.front());
					//std::cout << trimEndWhiteSpaceReturn(nameSplit.front()) << "\n";
					nameIndex[trimEndWhiteSpaceReturn(nameSplit.front())] = nodeCount;
					nameSplit.back().erase(nameSplit.back().end() - 1);
					auto attrSplit = tokenizeString(nameSplit.back(), ",");
					std::unordered_map<std::string, std::string> attrs;
					for (const auto at : attrSplit) {
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] =
								trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for (const auto &at : attrs) {
						if (at.first == "fillcolor") {
							nodes[nodeCount]["color"] = bib::replaceString(
									trimEndWhiteSpaceReturn(at.second), "\"", "");
						} else if (at.first == "width") {
							nodes[nodeCount]["size"] = bib::lexical_cast<double, std::string>(
									trimEndWhiteSpaceReturn(at.second));
						}
					}
					++nodeCount;
				}
			}
		}
	} else {
		std::stringstream ss;
		ss << bib::bashCT::bold
				<< bib::bashCT::red
				<< "Error in opening " << dotFilename <<
				bib::bashCT::reset << std::endl;
		throw std::runtime_error{ss.str()};
	}
	return graph;
}

Json::Value tableToJsonByRow(const bibseq::table & tab,
		const std::string mainColName, const VecStr & initialVisibleColumns,
		const VecStr & excludeFromNum) {
	Json::Value ret;
	auto & outTab = ret["tab"];
	std::unordered_map<uint32_t, bool> numCheck;
	bibseq::VecStr numericCols;
	for (const auto & colPos : iter::range<uint32_t>(tab.columnNames_.size())) {
		numCheck[colPos] = bibseq::isVecOfDoubleStr(tab.getColumn(colPos));
		if (numCheck[colPos]) {
			if (!bib::in(tab.columnNames_[colPos], excludeFromNum)) {
				numericCols.emplace_back(tab.columnNames_[colPos]);
			}
		}
	}

	for (const auto & rowPos : iter::range<uint32_t>(tab.content_.size())) {
		for (const auto & colPos : iter::range<uint32_t>(tab.columnNames_.size())) {
			if (numCheck[colPos]) {
				outTab[rowPos][tab.columnNames_[colPos]] = bib::lexical_cast<double>(
						tab.content_[rowPos][colPos]);
			} else {
				outTab[rowPos][tab.columnNames_[colPos]] = tab.content_[rowPos][colPos];
			}
		}
	}
	ret["columnNames"] = bib::json::toJson(tab.columnNames_);
	ret["numericColNames"] = bib::json::toJson(numericCols);

	/**@todo hide checks for presence of actual names for mainColName and hideOnSTartColNames*/
	ret["mainColName"] = mainColName;

	VecStr hideOnStartColNames;
	if(!initialVisibleColumns.empty()){
		for(const auto & col : tab.columnNames_){
			if(!bib::in(col, initialVisibleColumns)){
				hideOnStartColNames.emplace_back(col);
			}
		}
	}
	ret["initialVisibleColumns"] = bib::json::toJson(initialVisibleColumns);
	ret["hideOnStartColNames"] = bib::json::toJson(hideOnStartColNames);
	return ret;
}

Json::Value tableToJsonColumnWise(const bibseq::table & tab){
	Json::Value ret;
	std::unordered_map<uint32_t, bool> numCheck;
	for(const auto & colPos : iter::range<uint32_t>(tab.columnNames_.size())){
		if(bibseq::isVecOfDoubleStr(tab.getColumn(colPos))){
			ret[tab.columnNames_[colPos]] = bib::json::toJson(bib::lexical_cast_con<std::vector<std::string>,std::vector<double>>(tab.getColumn(colPos)));
		}else{
			ret[tab.columnNames_[colPos]] = bib::json::toJson(tab.getColumn(colPos));
		}
	}
	return ret;
}

cppcms::json::object server_config(std::string name, uint32_t port){
  cppcms::json::object args;
  args["service"]["api"] = "http";
  args["service"]["port"] = port;
  args["service"]["ip"] = "0.0.0.0";

  args["http"]["script"] = name;
  return args;
}

Json::Value cppcmsJsonToJson(const cppcms::json::object& obj ){
	std::stringstream ss;
	ss << obj;
	Json::Value ret;
	ss >> ret;
	return ret;
}

Json::Value cppcmsJsonToJson(const cppcms::json::value& val ){
	std::stringstream ss;
	ss << val;
	Json::Value ret;
	ss >> ret;
	return ret;
}

cppcms::json::value jsonToCppcmsJson(const Json::Value & val ){
	std::stringstream ss;
	ss << val;
	cppcms::json::value ret;
	ss >> ret;
	return ret;
}

} /* namespace bibseq */
