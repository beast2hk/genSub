//
//  Canvas.hpp
//  gensubtitle
//
//  Created by Tommy To on 14/5/2022.
//

#ifndef __CANVAS__
#define __CANVAS__

#include <cstddef>
#include <memory>
#include <string>
#include <Magick++.h>

class Canvas {

public:
    Canvas(std::size_t _width, std::size_t _height);  // Create a canvas with transparent background
    Canvas(const Magick::Image &img);
    Canvas();
    ~Canvas();

    Magick::Image image;

    std::size_t width() const;
    std::size_t height() const;

    void save(std::string filename);
    void size(std::size_t width, std::size_t height);
    void fillColor(std::string color);
    void roundRectangle(uint32_t left, uint32_t top,
                        uint32_t width, uint32_t height, uint32_t radius);
    void composite(const Canvas &src, uint32_t x, uint32_t y);

};

#endif
