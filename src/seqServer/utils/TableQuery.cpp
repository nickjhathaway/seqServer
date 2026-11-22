/*
 * TableQuery.cpp
 *
 *  Created on: Nov 22, 2026
 *      Author: nick
 */

#include "TableQuery.hpp"
#include <regex>
#include <cmath>

namespace njhseq {

namespace {

std::string toLower(const std::string & str) {
	std::string ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
	return ret;
}

std::string trimSpace(const std::string & str) {
	const auto first = str.find_first_not_of(" \t\r\n");
	if (std::string::npos == first) {
		return "";
	}
	return str.substr(first, str.find_last_not_of(" \t\r\n") - first + 1);
}

VecStr lowerWords(const std::string & str) {
	VecStr ret;
	std::istringstream in(toLower(str));
	std::string word;
	while (in >> word) {
		ret.emplace_back(word);
	}
	return ret;
}

//NaN when it isn't a whole number
double toNumber(const std::string & str) {
	if (str.empty()) {
		return std::nan("");
	}
	char * end = nullptr;
	const double ret = std::strtod(str.c_str(), &end);
	return end == str.c_str() + str.size() ? ret : std::nan("");
}

bool hasAllWords(const std::string & lowerStr, const VecStr & words) {
	for (const auto & word : words) {
		if (std::string::npos == lowerStr.find(word)) {
			return false;
		}
	}
	return true;
}

struct ColumnFilter {
	uint32_t pos_{0};
	VecStr words_;
	std::function<bool(double)> compare_; /**< set for a numeric comparison*/
};

}  // namespace

TableQuery TableQuery::fromJson(const Json::Value & query) {
	TableQuery ret;
	if (query.isMember("searches") && query["searches"].isObject()) {
		for (const auto & col : query["searches"].getMemberNames()) {
			ret.searches_[col] = query["searches"][col].asString();
		}
	}
	ret.search_ = query.get("search", "").asString();
	if (query.isMember("order") && query["order"].isArray()) {
		for (const auto & order : query["order"]) {
			ret.order_.emplace_back(order.get("col", "").asString(), "desc" != order.get("dir", "asc").asString());
		}
	}
	ret.start_ = query.get("start", Json::UInt64(0)).asUInt64();
	ret.length_ = query.get("length", Json::UInt64(50)).asUInt64();
	return ret;
}

std::vector<uint64_t> TableQuery::matchingRows(const table & tab, const VecStr & numericCols) const {
	const std::regex comparisonPat{R"(^(>=|<=|>|<|=)\s*(-?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?)$)"};
	std::vector<ColumnFilter> filters;
	for (const auto & search : searches_) {
		if (!njh::in(search.first, tab.columnNames_)) {
			continue;
		}
		ColumnFilter filter;
		filter.pos_ = tab.getColPos(search.first);
		const auto trimmed = trimSpace(search.second);
		std::smatch match;
		if (njh::in(search.first, numericCols) && std::regex_match(trimmed, match, comparisonPat)) {
			const auto op = match[1].str();
			const double cutOff = std::stod(match[2].str());
			if (">" == op) {
				filter.compare_ = [cutOff](double val) { return val > cutOff; };
			} else if (">=" == op) {
				filter.compare_ = [cutOff](double val) { return val >= cutOff; };
			} else if ("<" == op) {
				filter.compare_ = [cutOff](double val) { return val < cutOff; };
			} else if ("<=" == op) {
				filter.compare_ = [cutOff](double val) { return val <= cutOff; };
			} else {
				filter.compare_ = [cutOff](double val) { return val == cutOff; };
			}
		} else {
			filter.words_ = lowerWords(trimmed);
			if (filter.words_.empty()) {
				continue;
			}
		}
		filters.emplace_back(filter);
	}
	const auto searchWords = lowerWords(search_);
	std::vector<uint64_t> ret;
	for (uint64_t rowPos = 0; rowPos < tab.content_.size(); ++rowPos) {
		const auto & row = tab.content_[rowPos];
		bool pass = true;
		for (const auto & filter : filters) {
			if (filter.compare_) {
				const double val = toNumber(row[filter.pos_]);
				pass = !std::isnan(val) && filter.compare_(val);
			} else {
				pass = hasAllWords(toLower(row[filter.pos_]), filter.words_);
			}
			if (!pass) {
				break;
			}
		}
		if (pass && !searchWords.empty()) {
			const auto lowerRow = toLower(njh::conToStr(row, "\t"));
			pass = hasAllWords(lowerRow, searchWords);
		}
		if (pass) {
			ret.emplace_back(rowPos);
		}
	}
	//sort keys, numbers parsed once
	struct SortKey {
		uint32_t pos_{0};
		bool ascending_{true};
		bool numeric_{false};
		std::unordered_map<uint64_t, double> numbers_;
	};
	std::vector<SortKey> keys;
	for (const auto & order : order_) {
		if (!njh::in(order.first, tab.columnNames_)) {
			continue;
		}
		SortKey key;
		key.pos_ = tab.getColPos(order.first);
		key.ascending_ = order.second;
		key.numeric_ = njh::in(order.first, numericCols);
		if (key.numeric_) {
			key.numbers_.reserve(ret.size());
			for (const auto rowPos : ret) {
				key.numbers_[rowPos] = toNumber(tab.content_[rowPos][key.pos_]);
			}
		}
		keys.emplace_back(std::move(key));
	}
	if (!keys.empty()) {
		std::stable_sort(ret.begin(), ret.end(), [&tab, &keys](uint64_t first, uint64_t second) {
			for (const auto & key : keys) {
				int cmp = 0;
				if (key.numeric_) {
					const double firstVal = key.numbers_.at(first);
					const double secondVal = key.numbers_.at(second);
					//non-numbers last either way
					if (std::isnan(firstVal) != std::isnan(secondVal)) {
						return std::isnan(secondVal);
					}
					if (!std::isnan(firstVal)) {
						cmp = firstVal < secondVal ? -1 : (firstVal > secondVal ? 1 : 0);
					}
				} else {
					cmp = tab.content_[first][key.pos_].compare(tab.content_[second][key.pos_]);
				}
				if (0 != cmp) {
					return key.ascending_ ? cmp < 0 : cmp > 0;
				}
			}
			return false;
		});
	}
	return ret;
}

Json::Value TableQuery::page(const table & tab, const VecStr & numericCols) const {
	const auto rows = matchingRows(tab, numericCols);
	std::vector<bool> isNumeric;
	for (const auto & col : tab.columnNames_) {
		isNumeric.emplace_back(njh::in(col, numericCols));
	}
	Json::Value ret;
	ret["recordsTotal"] = Json::UInt64(tab.content_.size());
	ret["recordsFiltered"] = Json::UInt64(rows.size());
	auto & data = ret["data"];
	data = Json::arrayValue;
	const uint64_t stop = std::min<uint64_t>(rows.size(), start_ + length_);
	for (uint64_t pos = start_; pos < stop; ++pos) {
		Json::Value out(Json::arrayValue);
		const auto & row = tab.content_[rows[pos]];
		for (uint32_t colPos = 0; colPos < row.size(); ++colPos) {
			const double val = isNumeric[colPos] ? toNumber(row[colPos]) : std::nan("");
			if (std::isnan(val)) {
				out.append(row[colPos]);
			} else {
				out.append(val);
			}
		}
		data.append(std::move(out));
	}
	return ret;
}

std::string TableQuery::tsv(const table & tab, const VecStr & numericCols) const {
	std::stringstream out;
	out << njh::conToStr(tab.columnNames_, "\t") << "\n";
	for (const auto rowPos : matchingRows(tab, numericCols)) {
		out << njh::conToStr(tab.content_[rowPos], "\t") << "\n";
	}
	return out.str();
}

VecStr TableQuery::uniqueValues(const table & tab, const VecStr & numericCols, const std::string & col) const {
	VecStr ret;
	if (!njh::in(col, tab.columnNames_)) {
		return ret;
	}
	const auto colPos = tab.getColPos(col);
	std::unordered_set<std::string> seen;
	for (const auto rowPos : matchingRows(tab, numericCols)) {
		if (seen.emplace(tab.content_[rowPos][colPos]).second) {
			ret.emplace_back(tab.content_[rowPos][colPos]);
		}
	}
	return ret;
}

}  // namespace njhseq
