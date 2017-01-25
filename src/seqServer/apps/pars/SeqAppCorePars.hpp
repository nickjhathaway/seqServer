#pragma once
/*
 * SeqAppCorePars.hpp
 *
 *  Created on: Sep 14, 2016
 *      Author: nick
 */


#include "seqServer/utils.h"

namespace bibseq {

/**@brief simple struct to store core parameters for seq apps
 *
 */
struct SeqAppCorePars {

	std::string name_; /**< root name of the server*/
	uint32_t port_ = std::numeric_limits<uint32_t>::max(); /**< local port to server on*/
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
	std::string getAddress()const;

};

}  // namespace bibseq


