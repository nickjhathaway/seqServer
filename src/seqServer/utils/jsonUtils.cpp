//
// seqServer - A library for analyzing sequence data
// Copyright (C) 2012-2018 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
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

namespace njhseq {
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
					for (const auto & at : attrSplit) {
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] =
								trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for (const auto &at : attrs) {
						if (at.first == "color") {
							links[linkCount]["color"] = njh::replaceString(
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
					for (const auto & at : attrSplit) {
						auto attr = tokenizeString(at, "=");
						attrs[trimEndWhiteSpaceReturn(attr.front())] =
								trimEndWhiteSpaceReturn(attr.back());
						//std::cout << attr.front() << ":" << attr.back() << "\n";
					}
					for (const auto &at : attrs) {
						if (at.first == "fillcolor") {
							nodes[nodeCount]["color"] = njh::replaceString(
									trimEndWhiteSpaceReturn(at.second), "\"", "");
						} else if (at.first == "width") {
							nodes[nodeCount]["size"] = njh::lexical_cast<double, std::string>(
									trimEndWhiteSpaceReturn(at.second));
						}
					}
					++nodeCount;
				}
			}
		}
	} else {
		std::stringstream ss;
		ss << njh::bashCT::bold
				<< njh::bashCT::red
				<< "Error in opening " << dotFilename <<
				njh::bashCT::reset << std::endl;
		throw std::runtime_error{ss.str()};
	}
	return graph;
}

Json::Value tableToJsonByRow(const njhseq::table & tab,
		const std::string mainColName, const VecStr & initialVisibleColumns,
		const VecStr & excludeFromNum) {

	auto columnNamesMod = tab.columnNames_;
	for (auto & col : columnNamesMod) {
		col = njh::replaceString(col, ".", "_");
	}

	Json::Value ret;
	VecStr numericCols;
	if(tab.empty()){
		ret["tab"] = Json::arrayValue;
		ret["numericColNames"] = Json::arrayValue;
	}else{
		auto & outTab = ret["tab"];
		std::unordered_map<uint32_t, bool> numCheck;
		for (const auto colPos : iter::range<uint32_t>(columnNamesMod.size())) {
			numCheck[colPos] = njhseq::isVecOfDoubleStr(tab.getColumn(colPos));
			if (numCheck[colPos]) {
				if (!njh::in(columnNamesMod[colPos], excludeFromNum)) {
					numericCols.emplace_back(columnNamesMod[colPos]);
				}
			}
		}

		for (const auto rowPos : iter::range<uint32_t>(tab.content_.size())) {
			for (const auto colPos : iter::range<uint32_t>(columnNamesMod.size())) {
				if (numCheck[colPos]) {
					outTab[rowPos][columnNamesMod[colPos]] = njh::lexical_cast<double>(
							tab.content_[rowPos][colPos]);
				} else {
					outTab[rowPos][columnNamesMod[colPos]] = tab.content_[rowPos][colPos];
				}
			}
		}
		ret["numericColNames"] = njh::json::toJson(numericCols);
	}

	ret["columnNames"] = njh::json::toJson(columnNamesMod);


	/**@todo hide checks for presence of actual names for mainColName and hideOnSTartColNames*/
	ret["mainColName"] = mainColName;

	VecStr hideOnStartColNames;
	if(!initialVisibleColumns.empty()){
		for(const auto & col : columnNamesMod){
			if(!njh::in(col, initialVisibleColumns)){
				hideOnStartColNames.emplace_back(col);
			}
		}
	}
	ret["initialVisibleColumns"] = njh::json::toJson(initialVisibleColumns);
	ret["hideOnStartColNames"] = njh::json::toJson(hideOnStartColNames);
	return ret;
}

Json::Value tableToJsonColumnWise(const njhseq::table & tab){
	Json::Value ret;
	std::unordered_map<uint32_t, bool> numCheck;
	for(const auto colPos : iter::range<uint32_t>(tab.columnNames_.size())){
		if(njhseq::isVecOfDoubleStr(tab.getColumn(colPos))){
			ret[tab.columnNames_[colPos]] = njh::json::toJson(njh::lexical_cast_con<std::vector<std::string>,std::vector<double>>(tab.getColumn(colPos)));
		}else{
			ret[tab.columnNames_[colPos]] = njh::json::toJson(tab.getColumn(colPos));
		}
	}
	return ret;
}



std::vector<uint32_t> parseJsonForSelected(const Json::Value & postData) {
	std::vector<uint32_t> selected { };
	if (postData.isMember("selected")) {
		selected = njh::json::jsonArrayToVec(postData["selected"],
				std::function<uint32_t(const Json::Value &)>(
						[](const Json::Value & val)->uint32_t {return val.asUInt();}));
	}
	return selected;
}


std::vector<uint32_t> parseJsonForPosition(const Json::Value & postData){
	std::vector<uint32_t> positions { };
	if (postData.isMember("positions")) {
		positions = njh::json::jsonArrayToVec(postData["positions"],
				std::function<uint32_t(const Json::Value &)>(
						[](const Json::Value & val)->uint32_t {return val.asUInt();}));
	}
	return positions;
}

} /* namespace njhseq */
