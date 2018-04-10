#include <window.hpp>

class Table {
public:
    Table(const glm::ivec2& cells, float margin, float spacing) : cells(cells), margin(margin), spacing(spacing) {}

    // Calculate the bounds of a rectangle which covers the cells (pos -> pos + size) in the table.
    void get_bounds(const glm::vec2& dimensions, const glm::ivec2& pos, const glm::ivec2& size, glm::vec2& top_left, glm::vec2& bottom_right) {
        glm::ivec2 spacers = glm::max(cells - 1, 0);

        glm::vec2 cell_pitch = (dimensions - glm::vec2(spacers) * spacing - 2 * margin) / glm::vec2(cells) + spacing;

        top_left = cell_pitch * glm::vec2(pos) + margin;
        bottom_right = top_left + cell_pitch * glm::vec2(size) - spacing;
    }

private:
    float margin; // Margin between the edge of the window and the table.
    float spacing; // Spacing between the table cells.
    glm::ivec2 cells; // Number of cells in the grid.
};


struct Style {
    float font_size, stroke_size, radius;
    Font font;
    Color fill, stroke, background;
};


struct Rectangle {
    glm::vec2 min, max;

    glm::vec2 get_center() const {
        return (min + max) / 2.0f;
    }
};


class Element;


class ILayout {
public:
private:
};


class ElementRenderer {
public:
    ElementRenderer(const Style& style) : style(style) { }

    void draw_background(Canvas& canvas, const Rectangle& bounds) {
        canvas.begin_path();
        canvas.rounded_rect(bounds.min, bounds.max - bounds.min, style.radius);
        canvas.fill(style.background);
        canvas.stroke(style.stroke, style.stroke_size);
    }

    void draw_text(Canvas& canvas, const std::string& text, const Rectangle& bounds) {
        canvas.text(bounds.get_center(), text, style.fill, Align::center | Align::middle);
    }

private:
    const Style& style;
};


class Element {
public:
    Element(const std::string& name, const Style& style) : name(name), style(style) { }

    void draw(Canvas& canvas, const Rectangle& bounds) {
        auto renderer = ElementRenderer(style);
        on_draw(canvas, renderer, bounds);
    }

private:

    virtual void on_draw(Canvas& canvas, ElementRenderer& r, const Rectangle& bounds) { }

    std::string name;
    const Style& style;
};



class Label : public Element {
public:
    Label(const std::string& name, const Style& style, const std::string& text) :
        Element(name, style), text(text) { }

private:

    void on_draw(Canvas& canvas, ElementRenderer& r, const Rectangle& bounds) override {
        r.draw_background(canvas, bounds);
        r.draw_text(canvas, text, bounds);
    }

    std::string text;
};



class VerticalLayout : public ILayout {
public:
    VerticalLayout(const std::vector<Element>& children) : children(children) { }

    std::vector<Rectangle> layout(const Rectangle& bounds) {
        // todo
    }

    void draw(Canvas& canvas, const Rectangle& bounds) {
        auto b = layout(bounds);
        for (std::size_t i = 0; i < children.size(); ++i) {
            children[i].draw(canvas, b[i]);
        }
    }

private:
    std::vector<Element> children;
};



class Editor : public EventContext {
public:
    void init() {
        window.create("Editor", 1280, 720);

        canvas = window.get_canvas();

        regular = canvas.load_font("regular", "OpenSans-Regular.ttf");
        bold = canvas.load_font("bold", "OpenSans-Bold.ttf");

        register_event(window.on_quit, [&]() { running = false; });

    }


    void run() {
        Style button_style;
        button_style.background = { 0.3f, 0.3f, 0.3f, 1.0f };
        button_style.fill = { 1.0f, 1.0f, 1.0f, 1.0f };
        button_style.stroke = { 0.5f, 0.5f, 0.7f, 1.0f };
        button_style.stroke_size = 3;
        button_style.radius = 3;
        button_style.font = bold;
        button_style.font_size = 18;

        Label label("label", button_style, "Hello, world!");


        while (running) {
            window.process_events();

            window.begin_frame();

            label.draw(canvas, { { 50, 50 }, { 150, 100 } });

            window.end_frame();
        }
    }

private:

    Window window;
    Canvas canvas;

    Font regular, bold;

    bool running = true;

};


int main(int argc, char** argv) {
    Editor app;

    app.init();
    app.run();

    return 0;
}
