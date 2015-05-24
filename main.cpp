#include <SDL2/SDL.h>

#include <GL/glu.h>

#include "clwrapper.hpp"
#include "CLKernel.hpp"
#include "SpringyObject.hpp"
#include "Camera.hpp"

const int width = 1600;
const int height = 900;

CLWrapper cl;


void simulationStep(float dt) {

}

int main() {

    SpringyObject s("objects/gridcube_16.obj");


    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &window, &renderer);

    Camera cam;
    glViewport(0, 0, width, height);

    glClearColor(0.2, 0.2, 0.2, 0.2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1);

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        float dt = 0.001;

        const Uint8 *state = SDL_GetKeyboardState(NULL);

        if (state[SDL_SCANCODE_W]) { cam.move_forward(dt); }
        if (state[SDL_SCANCODE_A]) { cam.move_left(dt); }
        if (state[SDL_SCANCODE_S]) { cam.move_backward(dt); }
        if (state[SDL_SCANCODE_D]) { cam.move_right(dt); }
        if (state[SDL_SCANCODE_LCTRL]) { cam.move_down(dt); }
        if (state[SDL_SCANCODE_LSHIFT]) { cam.move_up(dt); }

        if (state[SDL_SCANCODE_UP]) { cam.up(dt); }
        if (state[SDL_SCANCODE_LEFT]) { cam.left(dt); }
        if (state[SDL_SCANCODE_DOWN]) { cam.down(dt); }
        if (state[SDL_SCANCODE_RIGHT]) { cam.right(dt); }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        cam.look();
        s.step(dt);
        s.render();


        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}