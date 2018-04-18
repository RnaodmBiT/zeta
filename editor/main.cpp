#include <memory>
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
    Align font_align, anchor;
    Color fill, stroke, background;
};


struct Rectangle {
    glm::vec2 min, max;

    glm::vec2 get_center() const {
        return (min + max) / 2.0f;
    }

    float get_width() const {
        return max.x - min.x;
    }

    float get_height() const {
        return max.y - min.y;
    }

    glm::vec2 get_size() const {
        return max - min;
    }

    glm::vec2 get_anchor(Align anchor) const {
        glm::vec2 a;
        if (anchor & Align::left) {
            a.x = min.x;
        } else if (anchor & Align::center) {
            a.x = (min.x + max.x) / 2;
        } else {
            a.x = max.x;
        }

        if (anchor & Align::top) {
            a.y = min.y;
        } else if (anchor & Align::middle) {
            a.y = (min.y + max.y) / 2;
        } else {
            a.y = max.y;
        }
        return a;
    }
};


class Element;


class ILayout {
public:
    virtual std::vector<Rectangle> layout(const Rectangle& bounds) = 0;
    virtual void draw(Canvas& canvas, const Rectangle& bounds) = 0;
private:
};


class ElementRenderer {
public:
    ElementRenderer(const Style& style) : style(style) { }

    void draw_background(Canvas& canvas, const Rectangle& bounds) {
        auto s = bounds.get_size();

        canvas.push_state();
        canvas.translate(bounds.get_center());

        canvas.begin_path();
        canvas.rounded_rect(-s / 2.0f, s, style.radius);

        canvas.fill(style.background);
        canvas.stroke(style.stroke, style.stroke_size);

        canvas.pop_state();
    }

    void draw_text(Canvas& canvas, const std::string& text, const Rectangle& bounds) {
        canvas.font_size(style.font_size);
        canvas.text(bounds.get_anchor(style.anchor), text, style.fill, style.font_align);
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


typedef std::shared_ptr<Element> ElementPtr;
typedef std::vector<ElementPtr> ElementList;



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
    VerticalLayout(const ElementList& children) : children(children) { }

    std::vector<Rectangle> layout(const Rectangle& bounds) override {
        std::vector<Rectangle> result;

        float row_height = bounds.get_height() / (float)children.size();
        for (std::size_t i = 0; i < children.size(); ++i) {
            result.push_back({ { bounds.min.x, bounds.min.y + i * row_height },
                               { bounds.max.x, bounds.min.y + (i + 1) * row_height } });
        }

        return result;
    }

    void draw(Canvas& canvas, const Rectangle& bounds) override {
        auto b = layout(bounds);
        for (std::size_t i = 0; i < children.size(); ++i) {
            children[i]->draw(canvas, b[i]);
        }
    }

private:
    ElementList children;
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
        window.set_background({ 0.8f, 0.8f, 0.8f, 1.0f });

        Style label_style;
        label_style.background = { 0, 0, 0, 0 };
        label_style.fill = { 0, 0, 0, 1 };
        label_style.font = bold;
        label_style.font_size = 18;
        label_style.font_align = Align::top | Align::left;
        label_style.anchor = Align::top | Align::left;

        Style button_style;
        button_style.background = canvas.linear_gradient({ 0, -10 }, { 0, 10}, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.3f, 0.3f, 0.3f, 1.0f });
        button_style.fill = { 0.0f, 0.0f, 0.0f, 1.0f };
        button_style.stroke = { 0.5f, 0.5f, 0.5f, 1.0f };
        button_style.stroke_size = 1;
        button_style.radius = 2;
        button_style.font = bold;
        button_style.font_size = 18;
        button_style.font_align = Align::center | Align::middle;
        button_style.anchor = Align::center | Align::middle;

        Label label("label", button_style, "Hello, world!");

        VerticalLayout layout(ElementList{
            std::make_shared<Label>("label 1", label_style, "Hello!"),
            std::make_shared<Label>("label 2", label_style, "I like pie!"),
            std::make_shared<Label>("button", button_style, "Button!")
        });


        while (running) {
            window.process_events();

            window.begin_frame();

            label.draw(canvas, { { 50, 50 }, { 150, 100 } });


            layout.draw(canvas, { { 50, 150 }, { 150, 250 } });

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
