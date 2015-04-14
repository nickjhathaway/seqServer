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
cppcms::json::value tableToJsonRowWise(const bibseq::table & tab);
cppcms::json::value tableToJsonColumnWise(const bibseq::table & tab);

cppcms::json::object server_config(std::string name, uint32_t port);
Json::Value cppcmsJsonToJson(const cppcms::json::object& obj);
Json::Value cppcmsJsonToJson(const cppcms::json::value& val);
cppcms::json::value jsonToCppcmsJson(const Json::Value & val);

template<typename T>
cppcms::json::value seqsToJson(const std::vector<T> & reads){
  cppcms::json::value ret;
  auto& seqs = ret["seqs"];
  //find number of reads
  ret["numReads"] = reads.size();
  // get the maximum length
  uint64_t maxLen = 0;
  bibseq::readVec::getMaxLength(reads, maxLen);
  ret["maxLen"] = maxLen;
  for(const auto & pos : iter::range(reads.size())){
  	seqs[pos]= jsonToCppcmsJson(reads[pos].seqBase_.toJson());
    /*seqs[pos]["seq"] = reads[pos].seqBase_.seq_;
    seqs[pos]["name"] = reads[pos].seqBase_.name_;
    seqs[pos]["qual"] = reads[pos].seqBase_.qual_;*/
  }
  return ret;
}


} /* namespace bibseq */


