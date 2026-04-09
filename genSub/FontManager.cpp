//
//  FontManager.cpp
//  genSub
//
//  Created by Tommy To on 2026/01/26.
//

#include "FontManager.hpp"

FontManager::FontManager() {
    FcInit();
    config = FcInitLoadConfigAndFonts();
}

FontManager::~FontManager() {
    if (config != nullptr) {
      FcConfigDestroy(config);
    }
    FcFini();
}

std::shared_ptr<FontManager> FontManager::FontManager::getInstance() {
    static std::shared_ptr<FontManager> inst;
    if (!inst) {
        inst = std::make_shared<FontManager>();
    }
    return inst;
}


std::shared_ptr<Font> FontManager::load(const std::string &family) {
    FcResult   result;
    FcPattern  *font, *pat;
    FcObjectSet *attr = nullptr;

    std::shared_ptr<Font> ret;
        
    if (fonts.contains(family)) {
        return fonts.find(family)->second;
    }

    attr = FcObjectSetBuild (FC_FILE, (char *) 0);
    pat  = FcNameParse((const FcChar8*)family.c_str());
    FcFontSet *fs = FcFontList (0, pat, attr);
    if (fs->nfont == 0) {
        return ret; // Return an empty font
    }

    FcObjectSetDestroy(attr);
      
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    font = FcFontMatch(config, pat, &result);
    
    if (font) {
      FcChar8  *family_str = nullptr, *path_str = nullptr;
      
      if (FcPatternGetString(font, FC_FILE, 0, &path_str)     == FcResultMatch &&
          FcPatternGetString(font, FC_FAMILY, 0, &family_str) == FcResultMatch ) {
          ret.reset( new Font(family_str, path_str) );
          fonts.insert(std::make_pair(family, ret));
      }
      FcPatternDestroy(font);
    }
    FcPatternDestroy(pat);
    return ret;
}


Font::Font(const std::string &family, const std::string &path) {
    font_family = family;
    font_path   = path;
}

Font::Font(const FcChar8* family, const FcChar8* path) {
    font_family = reinterpret_cast<const char*>(family);
    font_path   = reinterpret_cast<const char*>(path);
}

const std::string& Font::getPath() const {
    return font_path;
}

const std::string& Font::getFamily() const {
    return font_family;
}
