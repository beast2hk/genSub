//
//  Text.cpp
//  gensubtitle
//
//  Created by Tommy To on 14/5/2022.
//

#include "Text.hpp"
#include "FontManager.hpp"
#include <algorithm>
#include <memory>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace Magick;

Text::Text(const Magick::Image &img):Canvas(img) {}

Text::~Text() {};

void Text::effect(const std::string color) {
    // must be gradient: or img:
    Image texture_mask;
    
    if (color.rfind("gradient:", 0) == 0) {
        texture_mask.size(Geometry(image.columns(), image.rows()));
        texture_mask.read(color);
        texture_mask.alpha(false);
        texture_mask.composite(image, Geometry("+0+0"), CopyAlphaCompositeOp);
        image = texture_mask;
    } if (color.rfind("img:", 0) == 0) {
        texture_mask.read(color.substr(4));
        auto geo = Geometry(image.columns(), image.rows());
        geo.aspect(true);
        texture_mask.resize(geo);
        texture_mask.alpha(false);
        texture_mask.composite(image, Geometry("+0+0"), CopyAlphaCompositeOp);
        image = texture_mask;
    }
}

void Text::outline(const std::string color, uint32_t width) {
  char kernel[100];
  Magick::Image texture_mask;
  std::unique_ptr<Magick::Image> outlineImage;

  if (image.rows() < 10) return;

  int32_t  outline_width = width;
  int32_t  padding       = outline_width * 2;

  try {
    image.borderColor("none");
    image.border(Geometry(padding, padding));
    image.repage();
    
    outlineImage.reset(new Image(image));
    
    if (color.rfind("gradient:", 0) == 0) {

      outlineImage->backgroundColor("black");
      outlineImage->fillColor("white");
      outlineImage->colorize(100, "white");
      snprintf(kernel, 50, "Disk:%d", outline_width);
      outlineImage->morphology(MorphologyMethod::DilateMorphology, kernel, 1);
      outlineImage->alpha("remove");

      
      texture_mask.size(Geometry(outlineImage->columns(), outlineImage->rows()));
      texture_mask.read(color);
      texture_mask.composite(*outlineImage, Geometry("+0+0"), CopyAlphaCompositeOp);
      texture_mask.composite(image, Geometry("+0+0"), OverCompositeOp);

      image    = texture_mask;

    } else {
      outlineImage->fillColor("white");
      outlineImage->colorize(100, "white");
      outlineImage->backgroundColor("black");
      
      snprintf(kernel, 50, "Disk:%d", outline_width);
      outlineImage->morphology(MorphologyMethod::DilateMorphology, kernel, 1);
      // outlineImage->gaussianBlur(0, 1);
      
      outlineImage->fillColor(color);
      outlineImage->colorize(100, color);
      
      outlineImage->composite(image, Geometry("+0+0"), OverCompositeOp);
      
      image = *outlineImage;
    }

    
  } catch (Magick::Warning &warning) {
    // cout << "Maigck Warning : " << warning.what() << endl;
  } catch (Magick::Exception &exception) {
    // cout << "Magick Exception : " << exception.what() << endl;
  }
}

void Text::glow(const std::string color, uint32_t width) {
  char kernel[100];

  std::unique_ptr<Magick::Image> outlineImage;

  if (image.rows() < 10) return;

  int32_t  outline_width = width;
  int32_t  padding       = outline_width * 3;
  
  try {
    image.borderColor("none");
    image.border(Geometry(padding, padding));
    image.repage();
      
    outlineImage.reset(new Image(image));
      
    outlineImage->fillColor("white");
    outlineImage->colorize(100, "white");
    outlineImage->backgroundColor("black");
      
    snprintf(kernel, 50, "Disk:%d", outline_width);
    outlineImage->morphology(MorphologyMethod::DilateMorphology, kernel, 1);
    outlineImage->gaussianBlur(0, 1);
      
    outlineImage->fillColor(color);
    outlineImage->colorize(100, color);
      
    outlineImage->gaussianBlur(0, outline_width);
      
    outlineImage->composite(image, Geometry("+0+0"), OverCompositeOp);
      
    image = *outlineImage;
      
  } catch (Magick::Warning &warning) {
    //cout << "Maigck Warning : " << warning.what() << endl;
  } catch (Magick::Exception &exception) {
    //cout << "Magick Exception : " << exception.what() << endl;
  }
}
