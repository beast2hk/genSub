//
//  Style.hpp
//  genSub
//
//  Created by Tommy To on 2025/09/28.
//

#ifndef __STYLE__
#define __STYLE__

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace Be {

enum Attr {
    ATTR_OUTLINE,
    ATTR_GLOW
};

class StyleItem {
private:
    std::string name, font, color;
    int32_t     size;
    bool  bBasicColor;

public:
    std::vector<std::tuple<Attr, std::string, int32_t>> attrs;

    StyleItem (const std::string &name);
    void setFont(const std::string &font);
    void setColor(const std::string &color);
    void setSize(int32_t size);
    void addAttr(Attr attr, const std::string &color, int32_t size);
    
    const std::string& getName() const;
    const std::string& getFontPath() const;
    const std::string& getFontFamily() const;
    
    int32_t getSize() const;
    const std::string& getColor() const;
    bool  isBasicColor() const;
};

class Style {
private:
    std::map< std::string, std::shared_ptr<StyleItem> > styles;
    
public:
    static std::shared_ptr<Style> getInstance();
    bool contains(const std::string &name) const;
    std::shared_ptr<StyleItem> create(const std::string &name);
    std::shared_ptr<StyleItem> load(const std::string &name);
//    std::shared_ptr<StyleItem> get(const std::string &name);

};

//template<typename _T>
//std::shared_ptr<_T> create() {
//    std::shared_ptr<_T> ret(new _T());
//    return ret;
//}

}


#endif /* Style_hpp */
