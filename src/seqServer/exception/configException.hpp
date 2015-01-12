#pragma once
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


