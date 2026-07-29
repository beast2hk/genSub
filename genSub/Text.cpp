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

std::tuple<int32_t, int32_t, std::shared_ptr<Canvas>> Text::createRectBackground(const std::string color, uint32_t padding) const {
    uint32_t xoffset, yoffset;
    Geometry boxsize = image.boundingBox();
    std::shared_ptr<Canvas> canvas(new Canvas(boxsize.width() + 2 * padding, boxsize.height() + 2 * padding));
    
    canvas->stroke(0, "none");
    canvas->fillColor(color);
    canvas->image.draw(DrawableRectangle(0, 0, canvas->width(), canvas->height()));
    
    xoffset = boxsize.xOff() - padding;
    yoffset = boxsize.yOff() - padding;
    
    return make_tuple(xoffset, yoffset, canvas);
}

std::tuple<int32_t, int32_t, std::shared_ptr<Canvas>> Text::createRoundRectBackground(const std::string color, uint32_t padding) const {
    uint32_t xoffset, yoffset, corner;
    Geometry boxsize = image.boundingBox();
    std::shared_ptr<Canvas> canvas(new Canvas(boxsize.width() + 2 * padding, boxsize.height() + 2 * padding));
    
    corner = std::min(canvas->width(), canvas->height()) / 5;
    
    canvas->stroke(0, "none");
    canvas->fillColor(color);
    canvas->image.draw(DrawableRoundRectangle(0, 0, canvas->width(), canvas->height(), corner, corner));
    
    xoffset = boxsize.xOff() - padding;
    yoffset = boxsize.yOff() - padding;
    
    return make_tuple(xoffset, yoffset, canvas);
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


Text* Text::createFromPango(const std::string str, const std::string color) {
    Image texture_mask, imgText;
    
    imgText.read(str);
    imgText.negate();

    if (color.rfind("gradient:", 0) == 0) {
        texture_mask.size(Geometry(imgText.columns(), imgText.rows()));
        texture_mask.read(color);
        texture_mask.alpha(false);
        //texture_mask.composite(imgText, Geometry("+0+0"), CopyAlphaCompositeOp);
        texture_mask.composite(imgText, 0, 0, CopyAlphaCompositeOp);
        return new Text(texture_mask);
    } if (color.rfind("img:", 0) == 0) {
        texture_mask.read(color.substr(4));
        auto geo = Geometry(imgText.columns(), imgText.rows());
        geo.aspect(true);
        texture_mask.resize(geo);
        texture_mask.alpha(false);
        //texture_mask.composite(imgText, Geometry("+0+0"), CopyAlphaCompositeOp);
        texture_mask.composite(imgText, 0, 0, CopyAlphaCompositeOp);
        return new Text(texture_mask);
    } else {
        // Basic plain color
        texture_mask = Image(Geometry(imgText.columns(), imgText.rows()), color);
        texture_mask.alpha(false);
        texture_mask.composite(imgText, 0, 0, CopyAlphaCompositeOp);
        return new Text(texture_mask);
    }
}
