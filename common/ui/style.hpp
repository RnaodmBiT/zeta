#pragma once

#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#include "../canvas.hpp"
#include "../json.hpp"


struct Style {
    float font_size, stroke_size, radius;
    std::string font;
    Align font_align, anchor;
    Color fill, stroke, background;

    Style() : font_size(10), stroke_size(0), radius(0) { }
};


static void to_json(json& j, const Style& s) {
    j = {
        { "font", s.font },
        { "font_size", s.font_size },
        { "font_align", s.font_align },

        { "fill", s.fill },
        { "stroke", s.stroke },
        { "background", s.background },
        { "stroke_size", s.stroke_size },
        { "radius", s.radius },

        { "anchor", s.anchor }
    };
}

static void from_json(const json& j, Style& s) {
    s.font = j["font"].get<std::string>();
    s.font_size = j["font_size"].get<float>();
    s.font_align = j["font_align"].get<Align>();

    s.fill = j["fill"].get<Color>();
    s.stroke = j["stroke"].get<Color>();
    s.background = j["background"].get<Color>();
    s.stroke_size = j["stroke_size"].get<float>();
    s.radius = j["radius"].get<float>();

    s.anchor = j["anchor"].get<Align>();
}



class StyleSheet {
public:

    void load_style(const std::string& filename) {
        std::ifstream i(filename);
        json style;
        i >> style;

        styles[fs::path(filename).stem().string()] = style.get<Style>();
    }


private:

    std::map<std::string, Style> styles;

};
