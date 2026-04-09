//
//  Style.cpp
//  genSub
//
//  Created by Tommy To on 2025/09/28.
//

#include "Style.hpp"
#include "FontManager.hpp"
#include <tuple>

Be::StyleItem::StyleItem(const std::string &name) {
    this->name = name;
    this->bBasicColor = true;
    this->color       = "black";
    this->size        = 30;
}

void Be::StyleItem::setFont(const std::string &font) {
    this->font = font;
}

void Be::StyleItem::setColor(const std::string &color) {
    this->color = color;
    if ((this->color.rfind("gradient:", 0) == 0) || (this->color.rfind("img:", 0) == 0)) {
        bBasicColor = false;
    }
}

void Be::StyleItem::setSize(int32_t size) {
    this->size = size;
}

void Be::StyleItem::addAttr(Attr attr, const std::string &color, int32_t size) {
    attrs.push_back(std::make_tuple(attr, color, size));
}

const std::string& Be::StyleItem::getName() const {
    return name;
}
const std::string& Be::StyleItem::getFontPath() const {
    auto manager = FontManager::getInstance();
    auto font_desc = manager->load(font);
    // assume there must be a font
    return font_desc->getPath();
}

const std::string& Be::StyleItem::getFontFamily() const {
    auto manager = FontManager::getInstance();
    auto font_desc = manager->load(font);
    // assume there must be a font
    return font_desc->getFamily();
}

int32_t Be::StyleItem::getSize() const {
    return size;
}

const std::string& Be::StyleItem::getColor() const {
    return color;
}

bool Be::StyleItem::isBasicColor() const {
    return bBasicColor;
}

std::shared_ptr<Be::Style> Be::Style::getInstance() {
    static std::shared_ptr<Style> inst;
    if (!inst) {
        inst = std::make_shared<Style>();
    }
    return inst;
}

bool Be::Style::contains(const std::string &name) const {
    return styles.contains(name);
}

std::shared_ptr<Be::StyleItem> Be::Style::create(const std::string &name) {
    if (styles.contains(name)) {
        return std::get<1>(*styles.find(name));
    } else {
        auto ret = std::make_shared<StyleItem>(name);
        styles.insert(std::make_pair(name, ret));
        return ret;
    }
}

std::shared_ptr<Be::StyleItem> Be::Style::load(const std::string &name) {
    if (styles.contains(name)) {
        return std::get<1>(*styles.find(name));
    } else {
        return std::shared_ptr<Be::StyleItem>();
    }
}
