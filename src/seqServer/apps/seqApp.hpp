#pragma once
/*
 * seqApp.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: nickhathaway
 */
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <bibseq.h>
#include <bibcpp.h>
#include <cppitertools/range.hpp>
#include "seqServer/utils.h"

namespace bibseq {



class seqApp: public cppcms::application {
protected:
	/**@b dispatch root page
	 *
	 * @param func The func, normally returning a html page
	 */
	template<typename C, typename T> void dispMapRoot(T func, C* classCaller) {
		dispatcher().assign<C>("", func, classCaller);
		mapper().assign("");
	}
	/**@b dispatch a command with no arguments and under name n
	 *
	 * @param func The func to activate for n, func should take no aruments
	 * @param n The name to call from root to activate func
	 */
	template<typename C, typename T> void dispMap(T func,C* classCaller, std::string n) {
		dispatcher().assign("/" + n, func, classCaller);
		mapper().assign(n, "/" + n);
	}
	/**@b dispatch a command with one argument given by separating command with one / , eg. root/com/arg
	 *
	 * @param func A func that takes a one string by copy-by-value
	 * @param n The name to dispatch under
	 * @param r The argument to take
	 */
	template<typename C, typename T> void dispMap_1arg(T func,C* classCaller, std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1);
		mapper().assign(n, "/" + n + "/{1}");
	}
	/**@b dispatch a command with two arguments separated by /, eg. root/com/arg1/arg2
	 *
	 * @param func A function that takes two strings as arguments, both by copy by value
	 * @param n The name of the command under root
	 * @param r the arguments separated by one / , eg. arg1/arg2
	 */
	template<typename C, typename T> void dispMap_2arg(T func,C* classCaller, std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1, 2);
		mapper().assign(n, "/" + n + "/{1}/{2}");
	}

	/**@b dispatch a command with three arguments separated by /, eg. root/com/arg1/arg2/agr3
	 *
	 * @param func A function that takes two strings as arguments, both by copy by value
	 * @param n The name of the command under root
	 * @param r the arguments separated by one / , eg. arg1/arg2
	 */
	template<typename C, typename T> void dispMap_3arg(T func,C* classCaller, std::string n, std::string r) {
		dispatcher().assign("/" + n + "/" + r, func, classCaller, 1, 2, 3);
		mapper().assign(n, "/" + n + "/{1}/{2}/{3}");
	}

	/**@b indicate incoming content is json
	 *
	 */
	void ret_json() {
		response().content_type("application/json");
	}
	/**@b indicate incoming content is javascript
	 *
	 */
	void ret_js() {
		response().content_type("text/javascript");
	}
	/**@b indicate incoming content is css
	 *
	 */
	void ret_css() {
		response().content_type("text/css");
	}

	void colorsData();

	void getColors(std::string num);

	std::map<std::string, bib::FileCache> pages_;
	std::map<std::string, bib::FilesCache> jsAndCss_;
	//bib::FilesCache jsLibs_;
	//bib::FilesCache jsOwn_;

	//bib::FilesCache cssLibs_;
	//bib::FilesCache cssOwn_;


public:
	seqApp(cppcms::service& srv, std::map<std::string, std::string> config);

	virtual ~seqApp();

	virtual VecStr requiredOptions()const;

	/**@b Test config map, should contain at least name, js, and css and additonal checks can be added
	 *
	 * @param config The config map to test
	 * @param additionalChecks Any additional checks that need to be search for
	 * @return Whether config passes
	 */
	static bool configTest(const MapStrStr & config,
			const VecStr & checks, const std::string nameOfClass);

	void jsLibs();
	void jsOwn();
	void cssLibs();
	void cssOwn();
};

} /* namespace bibseq */


