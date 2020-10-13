#pragma once
/*
 * SeqAppCorePars.hpp
 *
 *  Created on: Sep 14, 2016
 *      Author: nick
 */
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

#include "seqServer/utils.h"

#include <njhseq/programUtils/seqSetUp.hpp>

namespace njhseq {

/**@brief simple struct to store core parameters for seq apps
 *
 */
struct SeqAppCorePars {

	std::string name_; /**< root name of the server*/
	uint32_t port_ = std::numeric_limits<uint32_t>::max(); /**< local port to server on*/
	std::string bindAddress_ = "127.0.0.1"; /**< adress to bind to , defaults to 127.0.0.1 (localhost) */
	bfs::path seqServerCore_ = seqServer::getSeqServerInstallCoreDir(); /**< directory of the seqServer core javascript and css*/
	bfs::path workingDir_ = "/tmp/"; /**< the working to store temporary files*/
	bool verbose_; /**< verbosity of server*/
	bool debug_; /**< whether to run in debug mode*/


	/**@brief set options with core options flags
	 *
	 * the debug_ and verbose_ members will be set to the setUp.pars_ members so these should be set up before this is called
	 *
	 * @param setUp seqSetUp object to add options
	 */
	void setCoreOptions(seqSetUp & setUp);

	/**@brief add core options to a config json object
	 *
	 * @param config the configuration
	 */
	void addCoreOpts(Json::Value & config);

	/**@brief get local address of the root of the server
	 *
	 * @return a string with local address of the root server
	 */
	std::string getAddress() const;

};

}  // namespace njhseq


