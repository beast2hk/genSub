//
//  Text.hpp
//  gensubtitle
//
//  Created by Tommy To on 14/5/2022.
//

#ifndef __TEXT__
#define __TEXT__

#include <memory>
#include <string>
#include <Magick++.h>
#include "Canvas.hpp"

class Text : public Canvas {

//private:
//    std::unique_ptr<Magick::Image> text;
    
public:
  Text(const Magick::Image &img);
  ~Text();

//    void set(Magick::Image &img);
    void effect(const std::string color);
    void outline(const std::string color, uint32_t width);
    void glow(const std::string color, uint32_t width);

    //    bool setText(Magick::Image *img, const std::string& color);
    
//  bool set(const std::string str, uint32_t font_size,
//           const std::string color, uint32_t padding,
//           const std::string outline_color, uint32_t stroke);
//
//  bool setText(const std::string str, uint32_t font_size, const std::string color);
//  void hollow(const std::string color, uint32_t width);
};


#endif /* __TEXT__ */
