#pragma once

#include <fstream>
#include <string>
#include <cerrno>
#include <boost/filesystem.hpp>
#include "exception.hpp"
#include <seqTools/utils/stringUtils.hpp>

namespace utils {

namespace bfs = boost::filesystem;

// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
static std::string get_file_contents(const bfs::path& fnp, bool verbose) {
	std::ifstream f(fnp.string(), std::ios::in | std::ios::binary);
	if (!f.is_open()) {
		throw err::str(err::F() << "could not open file" << fnp);
	}
	if (verbose) {
		std::cout << "reading file " << fnp << std::endl;
	}
	std::string ret;
	f.seekg(0, std::ios::end);
	ret.resize(f.tellg());
	f.seekg(0, std::ios::beg);
	f.read(&ret[0], ret.size());
	f.close();
	return ret;
}

class FileCache {
private:
	const bfs::path fnp_;
	std::string d_;
	std::time_t time_;

	void load() {
		d_ = utils::get_file_contents(fnp_, false);
		time_ = bfs::last_write_time(fnp_);
	}

public:
	FileCache(const bfs::path& fnp) :
			fnp_(fnp) {
		load();
	}

	const std::string& get() {
		if (needsUpdate()) {
			load();
		}
		return d_;
	}

	const std::string& get(const std::string & str,
			const std::string & replacement) {
		if (needsUpdate()) {
			load();
			d_ = bibseq::replaceString(d_, str, replacement);
		}
		return d_;
	}

	bool needsUpdate() const {
		return time_ != bfs::last_write_time(fnp_);
	}

	void replaceStr(const std::string & str, const std::string & replacement){
		d_ = bibseq::replaceString(d_, str, replacement);
	}
};

class FilesCache {
private:
	std::vector<FileCache> files_;
	std::string s_;

	void load() {
		std::stringstream ss;
		for (auto& f : files_) {
			ss << f.get();
		}
		s_ = ss.str();
	}

public:
	FilesCache(std::vector<bfs::path> jsutils) {
		for (const auto& p : jsutils) {
			files_.emplace_back(FileCache(p));
		}
		load();
	}

	const std::string& get() {
		for (const auto& f : files_) {
			if (f.needsUpdate()) {
				load();
				break;
			}
		}
		return s_;
	}
};

} // ::utils
