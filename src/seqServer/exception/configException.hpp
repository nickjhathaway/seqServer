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
 * configException.hpp
 *
 *  Created on: Jan 11, 2015
 *      Author: nickhathaway
 */



#include <exception>
#include <string>
#include <vector>
#include <bibseq/utils.h>

namespace bibseq {

class configException: public std::exception {
protected:
	const std::string what_;	// error message
public:
	configException() :
			what_("Error in Configuration!") {
	}
	configException(const std::string & nameOfClass,
			const std::vector<std::string> missing) :
			what_("Missing: " + vectorToString(missing, ",") + " for " + nameOfClass) {
	}

	virtual ~configException() {

	}

	virtual const char* what() const throw () {
		return what_.c_str();
	}
};

} /* namespace bibseq */


