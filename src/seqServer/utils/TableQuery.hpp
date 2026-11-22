#pragma once

/*
 * TableQuery.hpp
 *
 *  Created on: Nov 22, 2026
 *      Author: nick
 */

#include <njhcpp/jsonUtils.h>
#include <njhseq/objects/dataContainers/tables/table.hpp>

namespace njhseq {

/**@brief Search, sort and page a table on the server for a table the browser shows a page at a time (njhServerTable in seqServerCore njhServerTable.js)
 *
 * Searches work like njhTable's: a column search keeps rows whose value has every space separated word in it, ignoring case,
 * and on a numeric column a comparison like >0.1, <=5 or =0 compares the value instead. The search over all columns keeps rows
 * where every word is in some column.
 */
struct TableQuery {
	std::map<std::string, std::string> searches_; /**< by column name*/
	std::string search_; /**< over all columns*/
	std::vector<std::pair<std::string, bool>> order_; /**< column name and whether ascending, first is the main sort, empty keeps the table's order*/
	uint64_t start_{0}; /**< first matching row for page()*/
	uint64_t length_{50}; /**< rows for page()*/

	/**@brief from {"searches":{"col":"text"}, "search":"text", "order":[{"col":"name","dir":"asc|desc"}], "start":0, "length":50}, every field optional
	 *
	 */
	[[nodiscard]] static TableQuery fromJson(const Json::Value & query);

	/**@brief the positions of the rows matching the searches, sorted by order_, ties keep the table's order
	 *
	 * @param tab the table
	 * @param numericCols the columns that compare and sort as numbers
	 */
	[[nodiscard]] std::vector<uint64_t> matchingRows(const table & tab, const VecStr & numericCols) const;

	/**@brief the page from start_, as {"recordsTotal", "recordsFiltered", "data":[[row values in column order]]} with numericCols as numbers
	 *
	 */
	[[nodiscard]] Json::Value page(const table & tab, const VecStr & numericCols) const;

	/**@brief every matching row as tab delimited text with the column names first
	 *
	 */
	[[nodiscard]] std::string tsv(const table & tab, const VecStr & numericCols) const;

	/**@brief the distinct values of a column in the matching rows, in the order they come
	 *
	 */
	[[nodiscard]] VecStr uniqueValues(const table & tab, const VecStr & numericCols, const std::string & col) const;
};

}  // namespace njhseq
