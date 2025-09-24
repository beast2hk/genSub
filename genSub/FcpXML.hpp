//
//  FcpXML.hpp
//  srt2xml
//
//  Created by Tommy To on 14/6/2024.
//

#ifndef __FCPXML__
#define __FCPXML__

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <pointer.hpp>

BE_SINGLETON(FcpXML, false)

class Caption {
public:
  std::string id, name, src, format_id;
  uint64_t start_time, duration;  // in ms
};

class _FcpXML {
public:
  enum Resolution {
    FHD,   // 1920 x 1080
    UHD    // 3840 x 2160  4K
  };
  
private:
  tinyxml2::XMLDocument doc_out;
  std::string project_name;
  std::string video_format_id, asset_format_id;
  uint32_t   rate;  // change to float
  uint64_t   duration;
  Resolution res;
  std::vector<Caption> assets;
  
public:
  _FcpXML();
  ~_FcpXML();
  
  void setProjectName(const std::string &_project_name);
  void setFrameRate(uint32_t _rate);
  void setResolution(_FcpXML::Resolution _res);
  void setDuration(uint64_t _duration);
  void save();
  void addCaption(std::string name, std::string src, uint64_t start_time, uint64_t duration);
};

#endif /* __FCPXML__ */
