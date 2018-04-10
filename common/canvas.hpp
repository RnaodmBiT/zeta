#pragma once
#include <string>
#include <nanovg.h>
#include <glm/glm.hpp>

typedef int Font;

struct Color {
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) : c(nvgRGBAf(r, g, b, a)) { }
    Color(const glm::vec4& c) : c(nvgRGBAf(c.x, c.y, c.z, c.w)) { }

    union {
        NVGcolor c;
        struct {
            float r, g, b, a;
        };
        glm::vec4 v;
    };
};

enum class Align : int {
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
        nvgFillColor(ctx, color.c);
        nvgFill(ctx);
    }

    void stroke(const Color& color, float width) {
        nvgStrokeColor(ctx, color.c);
        nvgStrokeWidth(ctx, width);
        nvgStroke(ctx);
    }


private:
    NVGcontext* ctx;
};
