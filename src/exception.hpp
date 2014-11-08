#pragma once

#include <stdexcept>
#include <sstream>

namespace err {

class F {
// from http://stackoverflow.com/a/12262626
public:
	F() {
	}
	~F() {
	}

	template<typename Type>
	F & operator <<(const Type & value) {
		stream_ << value;
		return *this;
	}

	std::string str() const {
		return stream_.str();
	}
	operator std::string() const {
		return stream_.str();
	}

	enum ConvertToString {
		to_str
	};
	std::string operator >>(ConvertToString) {
		return stream_.str();
	}

private:
	std::stringstream stream_;

	F(const F &);
	F & operator =(F &);
};

struct str: public std::exception {
	// from http://stackoverflow.com/a/134612
	std::string s;
	str(std::string ss) :
			s(ss) {
	}
	~str() throw () {
	}
	const char* what() const throw () {
		return s.c_str();
	}
};
} // namespace err
