//
//  FcpXML.cpp
//  srt2xml
//
//  Created by Tommy To on 14/6/2024.
//

#include "FcpXML.hpp"
#include "ResIDGenerator.hpp"

using namespace tinyxml2;

_FcpXML::_FcpXML() {
  auto rID          = ResIDGenerator::getInstance();
  
  video_format_id   = rID->next();
  asset_format_id   = rID->next();
  
  rate     = 30;  // 30 fps
  res      = FHD; // 1920 x 1080
  duration = 60;  // default 60s
  project_name = "subtitle.fcpxml";
}

_FcpXML::~_FcpXML() {
}

void _FcpXML::setProjectName(const std::string &_project_name) {
  project_name = _project_name;
}
void _FcpXML::setFrameRate(uint32_t _rate) {
  rate = _rate;
}

void _FcpXML::setResolution(_FcpXML::Resolution _res) {
  res = _res;
}

void _FcpXML::addCaption(std::string name, std::string src, uint64_t start_time, uint64_t duration) {
  Caption cap;
  
  cap.id         = ResIDGenerator::getInstance()->next();
  cap.name       = name;
  cap.src        = src;
  cap.format_id  = asset_format_id;
  cap.start_time = start_time;
  cap.duration   = duration;

  assets.push_back(cap);
  
}

void _FcpXML::setDuration(uint64_t _duration) {
  duration = _duration;
}

void _FcpXML::save() {
  char buf[1000];
  
  auto *out_decl    = doc_out.NewDeclaration();
  doc_out.InsertFirstChild(out_decl);

  auto *out_doctype = doc_out.NewUnknown("DOCTYPE fcpxml");
  doc_out.InsertAfterChild(out_decl, out_doctype);
  
  auto *doc_fcpxml  = doc_out.NewElement("fcpxml");
  doc_fcpxml->SetAttribute("version", "1.11");
  doc_out.InsertAfterChild(out_doctype, doc_fcpxml);

  auto *doc_res     = doc_out.NewElement("resources");
  doc_fcpxml->InsertFirstChild(doc_res);
  
  auto *doc_format1 = doc_out.NewElement("format");
  auto *doc_format2 = doc_out.NewElement("format");

  doc_format1->SetAttribute("id", video_format_id.c_str());
  doc_format2->SetAttribute("id", asset_format_id.c_str());
  doc_format2->SetAttribute("name", "FFVideoFormatRateUndefined");
  doc_format2->SetAttribute("width", 1920);
  doc_format2->SetAttribute("height", 1080);
  doc_format2->SetAttribute("colorSpace", "1-13-1");
  
  switch (res) {
    case FHD:
      doc_format1->SetAttribute("name", "FFVideoFormat1920x1080");
      doc_format1->SetAttribute("width", 1920);
      doc_format1->SetAttribute("height", 1080);
      break;
      
    case UHD:
      doc_format1->SetAttribute("name", "FFVideoFormat3840x2160");
      doc_format1->SetAttribute("width", 3840);
      doc_format1->SetAttribute("height", 2160);
      break;
  }

  snprintf(buf, 50, "100/%ds", (int32_t)(100 * rate));
  doc_format1->SetAttribute("frameDuration", buf);
  doc_format1->SetAttribute("colorSpace", "1-1-1 (Rec. 709)");

  doc_res->InsertFirstChild(doc_format1);
  doc_res->InsertAfterChild(doc_format1, doc_format2);
  
  auto *last = doc_format2;

  // asset
  
  for(auto &a : assets) {
    auto *next = doc_out.NewElement("asset");
    next->SetAttribute("id", a.id.c_str());
    next->SetAttribute("name", a.name.c_str());
    next->SetAttribute("start"   , "0s");
    next->SetAttribute("duration", "0s");
    next->SetAttribute("hasVideo", "1");
    next->SetAttribute("format", asset_format_id.c_str());
    next->SetAttribute("videoSources", "1");
    
    auto *media = doc_out.NewElement("media-rep");
    media->SetAttribute("kind", "original-media");
    media->SetAttribute("src",  a.src.c_str());
    next->InsertFirstChild(media);
    doc_res->InsertEndChild(next);
  }
  
  auto *doc_event = doc_out.NewElement("event");
  doc_event->SetAttribute("name", "Subtitle");
  doc_fcpxml->InsertEndChild(doc_event);
  
  auto *doc_project = doc_out.NewElement("project");
  doc_project->SetAttribute("name", "Subtitle");
  doc_event->InsertFirstChild(doc_project);

  auto *doc_sequence = doc_out.NewElement("sequence");
  doc_sequence->SetAttribute("format", video_format_id.c_str());
  snprintf(buf, 100, "%d/%ds", (int32_t)(duration*rate/1000 + 5) * 100, rate * 100);

  doc_sequence->SetAttribute("duration", buf);
  doc_sequence->SetAttribute("tcStart", "0s");
  doc_sequence->SetAttribute("tcFormat", "NDF");
  doc_sequence->SetAttribute("audioLayout", "stereo");
  doc_sequence->SetAttribute("audioRate", "48k");
  
  doc_project->InsertFirstChild(doc_sequence);
  
  auto *doc_spine = doc_out.NewElement("spine");
  doc_sequence->InsertFirstChild(doc_spine);
  
  auto *doc_gap   = doc_out.NewElement("gap");
  doc_gap->SetAttribute("name", "Gap");
  doc_gap->SetAttribute("offset", "0s");
  doc_gap->SetAttribute("start", "0s");
  doc_gap->SetAttribute("duration", doc_sequence->Attribute("duration"));
  
  doc_spine->InsertFirstChild(doc_gap);
  
  for(auto &a : assets) {
    auto *next = doc_out.NewElement("video");
    next->SetAttribute("ref", a.id.c_str());
    next->SetAttribute("lane", "1");
    snprintf(buf, 100, "%d/%ds", (int32_t)(a.start_time*rate/1000) * 100, (int32_t)(rate * 100));
    next->SetAttribute("offset", buf);
    next->SetAttribute("name", a.name.c_str());
    next->SetAttribute("start", buf);
    snprintf(buf, 100, "%d/%ds", (int32_t)(a.duration*rate/1000) * 100,(int32_t)(rate * 100));
    next->SetAttribute("duration", buf);
    doc_gap->InsertEndChild(next);
  }
  
  doc_out.SaveFile(project_name.c_str());
}
