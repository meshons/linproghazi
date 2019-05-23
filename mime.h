#ifndef LINPROGHAZI_MIME_H
#define LINPROGHAZI_MIME_H

#include <map>
#include <fstream>
#include <string>

class mime {
    std::map<std::string, std::string> mimeTypes;
public:
    mime() {}
    explicit mime(const std::string & filename) {
        std::ifstream file(filename);
        std::string line;
        while (!file.eof()){
            std::getline(file, line);
            std::istringstream iss(line);
            std::vector<std::string> words((std::istream_iterator<std::string>(iss)),
                                             std::istream_iterator<std::string>());

            if (!words.empty() && words[0][0] == '#')
                continue;
            for (int i=1;i<words.size();++i)
                mimeTypes.emplace(std::make_pair(words[i], words[0]));
        }
    }

    std::string getMime(const std::string & extension) const {
        if (extension == "php")
            return (mimeTypes.find("html") != mimeTypes.end())
                   ? mimeTypes.at("html") : "application/octet-stream";
        return (mimeTypes.find(extension) != mimeTypes.end())
                ? mimeTypes.at(extension) : "application/octet-stream";
    }
};

#endif //LINPROGHAZI_MIME_H
