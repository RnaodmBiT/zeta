#include <window.hpp>


int main(int argc, char** argv) {
    bool running = true;

    Window window("Editor", 1280, 720);
    window.on_quit = [&]() { running = false; };

    Canvas& canvas = window.get_canvas();

    Font regular = canvas.load_font("regular", "OpenSans-Regular.ttf");
    Font bold = canvas.load_font("bold", "OpenSans-Bold.ttf");

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

    return 0;
}