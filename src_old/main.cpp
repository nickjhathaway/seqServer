#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>

#include "file_cache.hpp"
#include "ip.hpp"

namespace bfs = boost::filesystem;

class evt: public cppcms::application {
private:
    utils::FileCache html_;
    utils::FileCache js_;

    static bfs::path make_path(const bfs::path fn){
        return fn; // TODO: make absolute path, or from config file?
    }

    template <typename T> void dispMapRoot(T func){
        dispatcher().assign("", func, this);
        mapper().assign("");
    }
    template <typename T> void dispMap(T func, std::string n){
        dispatcher().assign("/" + n, func, this);
        mapper().assign(n, "/" + n);
    }
    template <typename T> void dispMap_1arg(T func, std::string n, std::string r){
        dispatcher().assign("/" + n + "/" + r, func, this, 1);
        mapper().assign(n, "/" + n + "/{1}");
    }
    template <typename T> void dispMap_2arg(T func, std::string n, std::string r){
        dispatcher().assign("/" + n + "/" + r, func, this, 1, 2);
        mapper().assign(n, "/" + n + "/{1}/{2}");
    }

    void ret_json(){
        response().content_type("application/json");
    }
    void ret_js(){
        response().content_type("text/javascript");
    }

public:
    evt(cppcms::service& srv, std::string name)
        : cppcms::application(srv)
        , html_(make_path("../resources/index.html"))
        , js_(make_path("../resources/main.js"))
    {
        dispMap(&evt::mainData, "mainData");
        dispMap(&evt::js, "js");
        dispMapRoot(&evt::html);
        mapper().root(name);
    }

    void mainData(){
        ret_json();
        cppcms::json::value r;
        auto& c = r["circles"];

        c[0]["radius"] = 50;
        c[0]["x"] = 50;
        c[0]["y"] = 50;
        c[0]["color"] = "red";
        c[0]["borderColor"] = "green";

        c[1]["radius"] = 50;
        c[1]["x"] = 250;
        c[1]["y"] = 250;
        c[1]["color"] = "purple";
        c[1]["borderColor"] = "orange";

        response().out() << r;
    }

    void html(){
        response().out() << html_.get();
    }

    void js(){
        ret_js();
        response().out() << js_.get();
    }
};

cppcms::json::object server_config(std::string name){
    cppcms::json::object args;
    args["service"]["api"] = "http";
    args["service"]["port"] = 8080;
    args["service"]["ip"] = "0.0.0.0";
    args["http"]["script"] = name;

    return args;
}

int main(int argc, char** argv){
    const std::string name = "/evt";
    auto config = server_config(name);

    try {
        cppcms::service app(config);
        app.applications_pool().mount(cppcms::applications_factory<evt>(name));
        app.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
