//
//  FontManager.hpp
//  genSub
//
//  Created by Tommy To on 2026/01/26.
//

#ifndef __FONTMANAGER__
#define __FONTMANAGER__

#include <memory>
#include <string>
#include <map>

#include <fontconfig/fontconfig.h>

class Font;

class FontManager {
private:
    FcConfig*   config;
    std::map<std::string, std::shared_ptr<Font>> fonts;

public:
    FontManager();
    ~FontManager();
    static std::shared_ptr<FontManager> getInstance();
    std::shared_ptr<Font> load(const std::string &family);
};

class Font {
private:
    std::string font_path, font_family;
    
public:
    Font(const std::string &family, const std::string &path);
    Font(const FcChar8* family, const FcChar8* path);
    const std::string& getPath() const;
    const std::string& getFamily() const;
};


#endif /* __FONTMANAGER__ */
