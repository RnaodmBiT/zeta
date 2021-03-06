#pragma once
#include <string>
#include <nanovg.h>
#include <glm/glm.hpp>

#include "json.hpp"

typedef int Font;

typedef NVGpaint Paint;

struct Color {
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) : c(nvgRGBAf(r, g, b, a)) { }
    Color(const glm::vec4& c) : c(nvgRGBAf(c.x, c.y, c.z, c.w)) { }
    Color(const Paint& p) : paint(p), solid(false) { }

    union {
        NVGcolor c;
        struct {
            float r, g, b, a;
        };
        glm::vec4 v;
        Paint paint;
    };

    bool solid = true;
};

static void to_json(json& j, const Color& c) {
    if (c.solid) {
        j = { c.r, c.g, c.b, c.a };
    } else {
        j = "gradient";
    }
}

static void from_json(const json& j, Color& c) {
    if (j.is_string()) {
        c = { 1.0f, 1.0f, 1.0f, 1.0f }; // HACK: this needs to be fixed for gradients
    }
    std::vector<float> channels = j;
    c = { channels[0], channels[1], channels[2], channels[3] };
}

enum class Align {
    left = NVG_ALIGN_LEFT,
    center = NVG_ALIGN_CENTER,
    right = NVG_ALIGN_RIGHT,

    top = NVG_ALIGN_TOP,
    middle = NVG_ALIGN_MIDDLE,
    bottom = NVG_ALIGN_BOTTOM,
    baseline = NVG_ALIGN_BASELINE
};

inline Align operator|(Align a, Align b) {
    return static_cast<Align>(static_cast<int>(a) | static_cast<int>(b));
}
inline bool operator&(Align a, Align b) {
    return (static_cast<int>(a) & static_cast<int>(b)) > 0;
}

static void to_json(json& j, const Align& a) {
    std::string output;
    if (a & Align::top) {
        output += "top ";
    } else if (a & Align::middle) {
        output += "middle ";
    } else if (a & Align::bottom) {
        output += "bottom ";
    } else if (a & Align::baseline) {
        output += "baseline ";
    }

    if (a & Align::left) {
        output += "left";
    } else if (a & Align::center) {
        output += "center";
    } else if (a & Align::right) {
        output += "right";
    }

    j = output;
}

static void from_json(const json& j, Align& a) {
    Align horizontal;
    std::string i = j;
    if (i.find("left") != std::string::npos) {
        horizontal = Align::left;
    } else if (i.find("right") != std::string::npos) {
        horizontal = Align::right;
    } else {
        horizontal = Align::center;
    }

    if (i.find("top") != std::string::npos) {
        a = horizontal | Align::top;
    } else if (i.find("bottom") != std::string::npos) {
        a = horizontal | Align::bottom;
    } else if (i.find("baseline") != std::string::npos) {
        a = horizontal | Align::baseline;
    } else {
        a = horizontal | Align::middle;
    }
}

class Canvas {
public:

    Canvas(NVGcontext* ctx = nullptr) : ctx(ctx) {}


    void begin_frame(const glm::ivec2& resolution, float pixel_ratio = 1.0f) {
        nvgBeginFrame(ctx, resolution.x, resolution.y, pixel_ratio);
    }

    void end_frame() {
        nvgEndFrame(ctx);
    }

    NVGcontext* get_context() {
        return ctx;
    }

    // State operations

    void push_state() {
        nvgSave(ctx);
    }

    void pop_state() {
        nvgRestore(ctx);
    }

    // Font operations


    Font load_font(const std::string& name, const std::string& filename) {
        return nvgCreateFont(ctx, name.c_str(), filename.c_str());
    }

    void set_font(Font font, float size) {
        nvgFontFaceId(ctx, font);
        font_size(size);
    }

    void set_font(const std::string& name, float size) {
        nvgFontFace(ctx, name.c_str());
        font_size(size);
    }

    void font_size(float size) {
        nvgFontSize(ctx, size);
    }


    // Text rendering operations

    void text_bounds(const glm::vec2& pos, const std::string& text, glm::vec2& min, glm::vec2 max, Align align) {
        nvgTextAlign(ctx, static_cast<int>(align));
        float bounds[4];
        nvgTextBounds(ctx, pos.x, pos.y, text.c_str(), nullptr, bounds);
        min = { bounds[0], bounds[1] };
        min = { bounds[2], bounds[3] };
    }

    void text(const glm::vec2& pos, const std::string& text, const Color& color, Align align) {
        nvgTextAlign(ctx, static_cast<int>(align));
        nvgFillColor(ctx, color.c);
        nvgText(ctx, pos.x, pos.y, text.c_str(), nullptr);
    }


    // Gradient operations

    Color linear_gradient(const glm::vec2& p_start, const glm::vec2& p_end, const Color& c_start, const Color& c_end) {
        return nvgLinearGradient(ctx, p_start.x, p_start.y, p_end.x, p_end.y, c_start.c, c_end.c);
    }

    Color box_gradient(const glm::vec2& p_start, const glm::vec2& size, const Color& c_start, const Color& c_end, float radius, float feather) {
        return nvgBoxGradient(ctx, p_start.x, p_start.y, size.x, size.y, radius, feather, c_start.c, c_end.c);
    }

    Color radial_gradient(const glm::vec2& p, float start, float end, const Color& c_start, const Color& c_end) {
        return nvgRadialGradient(ctx, p.x, p.y, start, end, c_start.c, c_end.c);
    }


    // Transform operations

    void reset_transform() {
        nvgResetTransform(ctx);
    }

    void translate(const glm::vec2& p) {
        nvgTranslate(ctx, p.x, p.y);
    }

    void rotate(float r) {
        nvgRotate(ctx, r);
    }

    void scale(const glm::vec2& s) {
        nvgScale(ctx, s.x, s.y);
    }


    // Path operations

    void begin_path() {
        nvgBeginPath(ctx);
    }

    void move_to(const glm::vec2& pos) {
        nvgMoveTo(ctx, pos.x, pos.y);
    }

    void line_to(const glm::vec2& pos) {
        nvgLineTo(ctx, pos.x, pos.y);
    }

    void bezier_to(const glm::vec2& c1, const glm::vec2& c2, const glm::vec2& pos) {
        nvgBezierTo(ctx, c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);
    }

    void quad_to(const glm::vec2& c, const glm::vec2& pos) {
        nvgQuadTo(ctx, c.x, c.y, pos.x, pos.y);
    }

    void arc_to(const glm::vec2& p1, const glm::vec2& p2, float r) {
        nvgArcTo(ctx, p1.x, p1.y, p2.x, p2.y, r);
    }

    void close_path() {
        nvgClosePath(ctx);
    }

    void path_winding(int dir) {
        nvgPathWinding(ctx, dir);
    }

    void arc(const glm::vec2& pos, float r, float a0, float a1, int dir) {
        nvgArc(ctx, pos.x, pos.y, r, a0, a1, dir);
    }

    void rect(const glm::vec2& pos, const glm::vec2& size) {
        nvgRect(ctx, pos.x, pos.y, size.x, size.y);
    }

    void rounded_rect(const glm::vec2& pos, const glm::vec2& size, float r) {
        nvgRoundedRect(ctx, pos.x, pos.y, size.x, size.y, r);
    }

    void ellipse(const glm::vec2& pos, const glm::vec2& radius) {
        nvgEllipse(ctx, pos.x, pos.y, radius.x, radius.y);
    }

    void circle(const glm::vec2& pos, float radius) {
        nvgCircle(ctx, pos.x, pos.y, radius);
    }

    void fill(const Color& color) {
        if (color.solid) {
            nvgFillColor(ctx, color.c);
        } else {
            nvgFillPaint(ctx, color.paint);
        }
        nvgFill(ctx);
    }

    void stroke(const Color& color, float width) {
        if (color.solid) {
            nvgStrokeColor(ctx, color.c);
        } else {
            nvgStrokePaint(ctx, color.paint);
        }
        nvgStrokeWidth(ctx, width);
        nvgStroke(ctx);
    }


private:
    NVGcontext* ctx;
};
