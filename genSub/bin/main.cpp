//
//  main.cpp
//  genSub
//
//  Created by Tommy To on 2025/09/24.
//

#include "ResIDGenerator.hpp"
#include "SubRipParser.hpp"
#include "FcpXML.hpp"
#include "Text.hpp"
#include "Style.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <regex>

#include <json_spirit/json_spirit.h>
#include <MagickCore/MagickCore.h>
#include <uvpp/uvpp.hpp>
#include <Magick++.h>

using namespace std;
using namespace Be;

#define CLS_LINE "\033[2K"

typedef std::tuple<std::string,      // filename
                   const StyleItem*, // Text style
                   Text*             // Text image
                   > Payload;

int main(int argc, const char * argv[]) {
  ifstream ifs;
  filesystem::path path, srt_path, output_path;
  json_spirit::Value config;
  Be::SubRipParser::subtitles_t subtitles;
  std::string str;
  bool bWriteImage, bMissingImgOnly;
  float dur;
  int32_t img_total, img_done;
    
//    string my_str   = "[highlight]你好嗎？";
//    regex  attr_express(R"(^\[[a-zA-Z1-9]+\])");
//    smatch sm;
//    
//
//    regex_search(my_str, sm, attr_express);
//    if (sm.size() > 0) {
//        cout << "Matched" << sm.str(0) << endl;
//        cout << sm.str(0).substr(1, sm.str(0).length() - 2);
//    } else {
//        cout << "None matched" << endl;
//    }
//    
//    
//    return 1;
    
    
  bWriteImage     = true;
  bMissingImgOnly = false;
    
  if (argc < 2) {
    cout << argv[0] << " [-x] [-s] <config json>" << endl;
    cout << "-x write xml only" << endl;
    cout << "-s skip existing png" << endl;
    return 1;
  }
    
  for(int32_t a = 1; a < argc; ++a) {
    if (strcmp(argv[a], "-x") == 0) {
      bWriteImage = false;
    } else if (strcmp(argv[a], "-s") == 0) {
      bMissingImgOnly = true;
    } else {
      path = argv[a];
      if (!path.has_extension() || path.extension() != ".json") {
        cerr << "Invalid script name" << endl;
        return 0;
      }
      break;
    }
  }
    
  ifs.open(path);
    
  if (!ifs) {
    cerr << "Json not found" << endl;
    return 0;
  }
    
  if (!json_spirit::read(ifs, config) || config.type() != json_spirit::obj_type) {
    cerr << "Invalid json format" << endl;
    return 0;
  }
    
  auto obj  = config.get_obj();
  auto node = json_spirit::find_value(obj, "filename");
    
  if (node.type() != json_spirit::str_type) {
    cerr << "Invalid filename in json" << endl;
    return 0;
  }
    
  srt_path = node.get_str();
    
  if (!filesystem::exists(srt_path)) {
    cerr << "SRT file not found" << endl;
    return 0;
  }
    
  node = json_spirit::find_value(obj, "fcpxml");
    
  if (node.type() != json_spirit::str_type) {
    cout << "Using default output filename \"Subtitle.fcpxml\"" << endl;
    output_path = "Subtitle.fcpxml";
  } else {
    output_path = node.get_str();
  }
    
  node = json_spirit::find_value(obj, "style");
    
  if (node.type() != json_spirit::array_type) {
    cout << "\"style\" is not an array" << endl;
    return 0;
  }
    
  for(auto i : node.get_array()) {
    if (i.type() != json_spirit::obj_type) {
      cerr << "style type error" << endl;
      return 0;
    }
        
    obj = i.get_obj();
        
    auto node2 = json_spirit::find_value(obj, "name");
    if (node2.type() != json_spirit::str_type) {
      cerr << "style name not found" << endl;;
      return 0;
    }
        
    auto style = Style::getInstance()->create(node2.get_str());
        
    node2 = json_spirit::find_value(obj, "font");
    if (node2.type() != json_spirit::str_type) {
      cerr << "style font not found" << endl;
      return 0;
    }
    style->setFont(node2.get_str());
        
    node2 = json_spirit::find_value(obj, "size");
    if (node2.is_null()) {
      cerr << "style font size not found" << endl;
      return 0;
    }
        
    if (node2.type() != json_spirit::int_type) {
      cerr << "style font size is not an integer" << endl;
      return 0;
    }
    style->setSize(node2.get_int());

    node2 = json_spirit::find_value(obj, "color");
    if (node2.type() != json_spirit::str_type) {
      cerr << "style font color not found" << endl;
      return 0;
    }
    style->setColor(node2.get_str());
        
    node2 = json_spirit::find_value(obj, "attr");
    if (node2.type() != json_spirit::array_type) {
      continue;
    }
        
    auto node3 = node2.get_array();
        
    for(auto attr : node3) {
      if (attr.type() != json_spirit::array_type || attr.get_array().size() < 1) {
        cerr << "Invalid attr format" << endl;
        return 0;
      }
      auto item = attr.get_array();
      if (item.at(0).type() != json_spirit::str_type) {
        cerr << "Invalid attr name" << endl;
        return 0;
      }
      auto attr_name = item.at(0).get_str();
      if (attr_name == "outline") {
        if (item.size() < 3 ||
            item.at(1).type() != json_spirit::str_type ||
            item.at(2).type() != json_spirit::int_type) {
          cerr << "Invalid outline parameters" << endl;
          return 0;
        }
        style->addAttr(ATTR_OUTLINE, item.at(1).get_str(), item.at(2).get_int());
      } else if (attr_name == "glow") {
        if (item.size() < 3 ||
            item.at(1).type() != json_spirit::str_type ||
            item.at(2).type() != json_spirit::int_type) {
          cerr << "Invalid glow parameters" << endl;
          return 0;
        }
        style->addAttr(ATTR_GLOW, item.at(1).get_str(), item.at(2).get_int());
      } else {
        cerr << "Unknown attr" << endl;
      }
    }
  }

  cout << "ok" << endl;
  cout << "SRT:" << srt_path << endl;
    
    // cout << "Abc" << CLS_LINE << "\r" ;
    
  subtitles = Be::SubRipParser::parse(srt_path);

  //    for(auto i : subtitles) {
  //        cout << i->getDialogue(true, true, true) << endl;
  //    }
    
  auto id_gen = ResIDGenerator::getInstance();
  auto xml    = FcpXML::getInstance();
    
  xml->setProjectName("Subtitle.fcpxml");
  xml->setResolution(_FcpXML::UHD);
    
    dur = 0;
    for(auto it : subtitles) {
        float endTime = it->getEndTime();
        if (endTime > dur) {
            dur = endTime;
        }
    }

    xml->setDuration(dur);
    
  if (bWriteImage) {
      char    buff1[200], buff2[200];
      int32_t sub_offset;
      auto   style = Style::getInstance()->load("default");
      regex  attr_express(R"(^\[[a-zA-Z1-9]+\])");
      string final_str, next_style;

      if (!style) {
          cerr << "Default style not found" << endl;
          exit(1);
      }
      
      img_total = img_done = 0;
        
      UVLoop loop = UVLoop::getInstance();
      Magick::InitializeMagick(nullptr);
        
      Magick::TypeMetric typeMetrics;
      Magick::Image      image;
        
      image.font(style->getFontPath());
      image.fontPointsize(style->getSize());
      image.strokeColor("none");
      image.strokeWidth(0);
        
      for(auto s : subtitles) {
          snprintf(buff1, 100, "caption_%04d", s->getSubNo());
          snprintf(buff2, 100, "caption_%04d.png", s->getSubNo());
          xml->addCaption(buff1, buff2, s->getStartTime(), s->getEndTime() - s->getStartTime());
            
          if (bMissingImgOnly &&
              filesystem::exists(buff2)) { continue; }
          smatch sm;
          regex_search(s->getText(), sm, attr_express);
          if (sm.size() > 0 && sm.str(0).length() > 2) {
              sub_offset = (int32_t)(sm.str(0).length() - 2);
              final_str  = s->getText().substr(sub_offset + 2);
              next_style = sm.str(0).substr(1, sub_offset);
              if (!Style::getInstance()->contains(next_style)) {
                  next_style = "default";
              }
          } else {
              next_style = "default";
              final_str = s->getText();
          }
          
          if (style->getName() != next_style) {
              style = Style::getInstance()->load(next_style);
              image.font(style->getFontPath());
              image.fontPointsize(style->getSize());
          }
            
      image.fontTypeMetricsMultiline(final_str, &typeMetrics);
      image.size(Magick::Geometry(typeMetrics.textWidth(), typeMetrics.textHeight()));
      image.read("xc:none");
            
      if (style->isBasicColor()) {
        image.fillColor(style->getColor());
      } else {
        image.fillColor("black");
      }
      image.annotate(final_str, Magick::CenterGravity);
            
      Payload *payload = new Payload();
      std::get<0>(*payload) = buff2;
      std::get<1>(*payload) = style.get();
      std::get<2>(*payload) = new Text(image);
      loop->queueWork([payload] {
          if (!std::get<1>(*payload)->isBasicColor()) {
              std::get<2>(*payload)->effect(std::get<1>(*payload)->getColor());
          }

          for(auto &attr : std::get<1>(*payload)->attrs) {
              switch (std::get<0>(attr)) {
                  case Be::ATTR_OUTLINE:
                      std::get<2>(*payload)->outline(std::get<1>(attr), std::get<2>(attr));
                      break;
                      
                  case Be::ATTR_GLOW:
                      std::get<2>(*payload)->glow(std::get<1>(attr), std::get<2>(attr));
                      break;
              }
          }
          
          Magick::Image frame("1920x1080", "none");
          int32_t offx = int32_t(frame.columns() - std::get<2>(*payload)->width()) / 2;
          int32_t offy = int32_t(frame.rows()    - std::get<2>(*payload)->height() - 50);
          frame.composite(std::get<2>(*payload)->image, offx, offy, Magick::OverCompositeOp);

          frame.magick("png32");
          frame.write(std::get<0>(*payload));
          
          delete payload;
          //std::get<2>(*payload)->save(std::get<0>(*payload));
      }, [&img_total, &img_done] {
          if (img_total > 0) {
              img_done++;
              cout << CLS_LINE << "\r";
              cout << img_done << "/" << img_total;
              cout.flush();
          }
      });
          img_total++;
    }
    loop->run();
      cout << endl;
  }

    xml->save();
  return 0;
}
