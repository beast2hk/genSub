//
//  Canvas.cpp
//  gensubtitle
//
//  Created by Tommy To on 14/5/2022.
//

#include "Canvas.hpp"
#include <iostream>

using namespace std;
using namespace Magick;

Canvas::Canvas() {}
Canvas::~Canvas() {}

Canvas::Canvas(std::size_t _width, std::size_t _height) {
    size(_width, _height);
}

Canvas::Canvas(const Magick::Image &img):image(img) {}

std::size_t Canvas::width() const {
  return image.columns();
}

std::size_t Canvas::height() const {
  return image.rows();
}

void Canvas::size(std::size_t _width, std::size_t _height) {
    image.size(Geometry(_width, _height));
    image.read("xc:none");
}

void Canvas::fillColor(std::string color) {
  try {
    image.fillColor(color);
  } catch (...) {
      cerr << "Canvas::fillColor caught an exception" << endl;
  }
}

void Canvas::roundRectangle(uint32_t left, uint32_t top,
                            uint32_t width, uint32_t height, uint32_t radius) {
  try {
    image.draw(DrawableRoundRectangle(left, top, left + width, top + height, radius, radius));
  } catch (...) {
      cerr << "Canvas::roundRectangle caught an exception" << endl;
  }
}

void Canvas::composite(const Canvas &src, uint32_t x, uint32_t y) {
  image.composite(src.image, x, y, OverCompositeOp);
}

void Canvas::save(std::string filename) {
  image.write(filename);
}
