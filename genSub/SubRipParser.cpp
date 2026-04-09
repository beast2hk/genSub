//
//  SubRipParser.cpp
//  genSub
//
//  Created by Tommy To on 2025/09/24.
//

#include "SubRipParser.hpp"
#include <fstream>
#include <sstream>

using namespace std;

void Be::SubRipParser::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

Be::SubRipParser::subtitles_t Be::SubRipParser::parse(const std::filesystem::path &path) {
    subtitles_t ret;

    std::ifstream infile(path);
    
    std::string line, start, end, completeLine = "", timeLine = "";
    int subNo = 0, turn = 0;

    /*
     * turn = 0 -> Add subtitle number
     * turn = 1 -> Add string to timeLine
     * turn > 1 -> Add string to completeLine
     */

    while (std::getline(infile, line))
    {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        if (line.compare(""))
        {
            if(!turn)
            {
                subNo=atoi(line.c_str());
                turn++;
                continue;
            }

            if (line.find("-->") != std::string::npos)
            {
                timeLine += line;

                std::vector<std::string> srtTime;
                split(timeLine, ' ', srtTime);
                start = srtTime[0];
                end = srtTime[2];

            }
            else
            {
                if (completeLine != "")
                    completeLine += "\n";

                completeLine += line;
            }

            turn++;
        }

        else
        {
            turn = 0;
            ret.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
            completeLine = timeLine = "";
        }

//        if(infile.eof())    //insert last remaining subtitle
//        {
//            _subtitles.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
//        }
    }
  
  if (completeLine.length() > 0) {
    ret.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
  }

  return ret;

}
