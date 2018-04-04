#include <cstdio>
#include <SDL.h>
#include <GL/glew.h>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

int main(int argc, char** argv) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_Window* window = SDL_CreateWindow("zeta", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = true;
    glewInit();

    NVGcontext* vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG);

    int font = nvgCreateFont(vg, "open_sans", "OpenSans-Regular.ttf");
    nvgFontFaceId(vg, font);

    glClearColor(1, 0, 0, 1);

    bool running = true;
    while (running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);



            // Start drawing to the UI
            nvgBeginFrame(vg, 1280, 720, 1);

            nvgFontSize(vg, 30);

            float bounds[4];
            nvgTextBounds(vg, 100, 100, "Hello world!", nullptr, bounds);

            nvgBeginPath(vg);

            nvgRoundedRect(vg, bounds[0] - 10, bounds[1] - 10, bounds[2] - bounds[0] + 20, bounds[3] - bounds[1] + 20, 10);
            nvgFillColor(vg, nvgRGBAf(0, 0, 0, 1));
            nvgStrokeWidth(vg, 4);
            nvgStrokeColor(vg, nvgRGBAf(0, 1, 0, 1));

            nvgFill(vg);
            nvgStroke(vg);

            nvgFillColor(vg, nvgRGBAf(1, 1, 1, 1));
            nvgText(vg, 100, 100, "Hello world!", nullptr);

            nvgEndFrame(vg);
            // Finished with the UI


            SDL_GL_SwapWindow(window);
        }

    }

    SDL_Quit();

    return 0;

}