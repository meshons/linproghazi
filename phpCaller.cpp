#include "phpCaller.h"


phpCaller::phpCaller(const std::string &get,
        const std::string &post,
        std::shared_ptr<Config> cfg,
        std::string file
        ): cfg{std::move(cfg)}, file{std::move(file)} {
    GET = splitParam(get);
    POST = splitParam(post);
}

std::string phpCaller::response() const {
    static unsigned int tempcounter = 0;

    std::string postget = "\'";
    for (const auto & it : GET)
    {
        postget += "$_GET[\""+it.first+"\"]=\""+it.second+"\";";
    }
    for (const auto & it : POST)
    {
        postget += "$_POST[\""+it.first+"\"]=\""+it.second+"\";";
    }
    postget += "\'";

    std::string cmd = cfg->PHPPath;
    if (postget.length() > 2)
        cmd += " -B "+postget;
    cmd += " -F "+file;
    cmd += " < helper.txt";
    std::string tempFileName = "temp" + std::to_string(tempcounter++) + ".txt";
    cmd += " > "+tempFileName;

    system(cmd.c_str());

    std::ifstream ifs(tempFileName);
    std::string ret{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
    ifs.close();
    if (std::remove(tempFileName.c_str()) != 0) {
        std::cout << "Error deleting temporary file" << std::endl;
    }
    return ret;
}

std::map<std::string, std::string> phpCaller::splitParam(const std::string & params) const {
    std::map<std::string, std::string> map;
    std::stringstream paramSplitter{params};
    std::string param;
    while (!paramSplitter.eof()) {
        std::getline(paramSplitter, param, '&');
        if (!param.empty()) {
            if (param.find('=')!=std::string::npos && param.find('=') && param.length() > 2) {
                map.emplace(std::make_pair(
                        param.substr(0, param.find('=')),
                        param.substr(param.find('=')+1)
                        ));
            } else {
                map.emplace(std::make_pair(param, ""));
            }
        }
    }
    return map;
}
