#pragma once
/*
 * jsonUtils.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */

#include <bibseq.h>
#include <bibcpp.h>
#include <cppitertools/range.hpp>
#include <cppcms/json.h>

namespace bibseq {


cppcms::json::value dotToJson(const std::string& dotFilename);
cppcms::json::value tableToJsonRowWise(const bibseq::table & tab, const std::string mainColName, const VecStr & hideOnStartColNames, const VecStr & excludeFromNum = VecStr{});
cppcms::json::value tableToJsonColumnWise(const bibseq::table & tab);

cppcms::json::object server_config(std::string name, uint32_t port);
Json::Value cppcmsJsonToJson(const cppcms::json::object& obj);
Json::Value cppcmsJsonToJson(const cppcms::json::value& val);
cppcms::json::value jsonToCppcmsJson(const Json::Value & val);

template<typename T>
cppcms::json::value seqsToJson(const std::vector<T> & reads, const std::string & uid){
  cppcms::json::value ret;
  auto& seqs = ret["seqs"];
  //find number of reads
  ret["numReads"] = reads.size();
  // get the maximum length
  uint64_t maxLen = 0;
  bibseq::readVec::getMaxLength(reads, maxLen);
  ret["maxLen"] = maxLen;
  ret["uid"] = uid;
  for(const auto & pos : iter::range(reads.size())){
  	seqs[pos]= jsonToCppcmsJson(reads[pos].seqBase_.toJson());
  }
  return ret;
}


} /* namespace bibseq */


