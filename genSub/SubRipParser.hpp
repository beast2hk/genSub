//
//  SubRipParser.hpp
//  genSub
//
//  Created by Tommy To on 2025/09/24.
//

#ifndef __SUBRIPPARSER__
#define __SUBRIPPARSER__

#include <filesystem>
#include <vector>
#include <memory>
#include <string>
#include "srtparser.hpp"

namespace Be {
  class SubRipParser {
  private:
      static void split(const std::string &s, char delim, std::vector<std::string> &elems);
      
  public:
    typedef std::vector< std::shared_ptr<SubtitleItem> > subtitles_t;
    static subtitles_t parse(const std::filesystem::path &path);
  };
}

#endif /* __SUBRIPPARSER__ */
