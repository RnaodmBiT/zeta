#include <window.hpp>


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
        while (running) {
            window.process_events();

            window.begin_frame();

            canvas.begin_path();
            canvas.rounded_rect({ 100, 100 }, { 200, 100 }, 2);
            canvas.fill({ 0.1f, 0.1f, 0.2f, 1.0f });

            canvas.begin_path();
            canvas.move_to({ 103, 125 });
            canvas.line_to({ 297, 125 });
            canvas.stroke({ 0.13f, 0.13f, 0.26f, 1.0f }, 2);

            canvas.set_font(bold, 18);
            canvas.text({ 105, 105 }, "Title", { 1, 1, 1, 1 }, Align::top | Align::left);

            canvas.set_font(regular, 16);
            canvas.text({ 105, 130 }, "Body-text", { 1, 1, 1, 1 }, Align::top | Align::left);


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