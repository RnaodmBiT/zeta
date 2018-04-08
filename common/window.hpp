#pragma once
#include <string>
#include <functional>

#include <SDL.h>
#include <GL/glew.h>
#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <glm/glm.hpp>

#include "canvas.hpp"
#include "event.hpp"

class Window {
public:

    struct Settings {
        glm::ivec2 size;
    };

    Window() { }

    void create(const std::string& title, int width, int height) {
        settings = {
            { width, height }
        };

        // Setup the window attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        // Create the window
        window = SDL_CreateWindow(title.c_str(),
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

        // Create the rendering contexts
        gl_context = SDL_GL_CreateContext(window);

        glewExperimental = true;
        glewInit();

        canvas = Canvas(nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG));
    }


    void close() {
        nvgDeleteGL3(canvas.get_context());
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);

        window = nullptr;
        gl_context = nullptr;
    }


    void begin_frame() {
        glClear(GL_COLOR_BUFFER_BIT);
        canvas.begin_frame(settings.size);
    }


    void end_frame() {
        canvas.end_frame();
        SDL_GL_SwapWindow(window);
    }


    void process_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_WINDOWEVENT:
                if (event.window.type == SDL_WINDOWEVENT_RESIZED) {
                    update_resolution(event.window.data1, event.window.data2);
                    on_resize({ event.window.data1, event.window.data2 });
                }
                break;

            case SDL_QUIT: on_quit(); break;

            case SDL_KEYDOWN: on_keydown(event.key.keysym.sym); break;
            case SDL_KEYUP: on_keyup(event.key.keysym.sym); break;

            case SDL_MOUSEBUTTONDOWN: on_buttondown(event.button.button); break;
            case SDL_MOUSEBUTTONUP: on_buttonup(event.button.button); break;
            case SDL_MOUSEMOTION: on_motion({ event.motion.x, event.motion.y }); break;
            }
        }
    }

    Canvas& get_canvas() {
        return canvas;
    }

    void set_background(const Color& color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    // Event callbacks
    Event<> on_quit;
    Event<glm::ivec2> on_resize;

    Event<SDL_Keycode> on_keydown, on_keyup;
    Event<int> on_buttondown, on_buttonup;

    Event<glm::ivec2> on_motion;

private:

    void update_resolution(int width, int height) {
        settings.size = { width, height };
        glViewport(0, 0, width, height);
    }

    Settings settings;

    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    Canvas canvas = nullptr;
};