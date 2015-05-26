#include <SDL2/SDL.h>

#include <GL/glu.h>

#include "clwrapper.hpp"
#include "CLKernel.hpp"
#include "SpringyObject.hpp"
#include "Camera.hpp"
#include "VolumeMesh.hpp"
#include "Sphere.hpp"

const int width = 1600;
const int height = 900;

CLWrapper cl;

#define TIME( call ) \
    { \
        int ticks1 = SDL_GetTicks();\
        call; \
        int ticks2 = SDL_GetTicks();\
        std::cout << #call << " " << (ticks2 - ticks1) << " ms\n"; \
    }


std::vector<AbstractObject *> objects;
std::vector<Sphere *> spheres;

void clear() {
    for (auto &o : objects) {
        delete o;
    }
    objects.clear();

    for (auto &s : spheres) {
        delete s;
    }
    spheres.clear();
}

void spawnVolume(std::string name) {
    auto t = new VolumeMesh(name);
    objects.push_back(t);
}

void stepAll(float dt, int substeps = 10) {
    for (const auto &o : objects) {
        for (int i = 0; i < substeps; ++i) {
            o->step(dt / substeps);
        }
        clFinish(cl.cqueue());
    }
}

void renderAll() {
    for (const auto &o : objects) {
        o->render();
    }

    for (const auto &s : spheres) {
        s->render();
    }
}


void inflateAll(float dt) {
    for (const auto &o : objects) {
        o->inflate(dt);
    }
}

void deflateAll(float dt) {
    for (const auto &o : objects) {
        o->deflate(dt);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &window, &renderer);

    Camera cam;
    glViewport(0, 0, width, height);

    glClearColor(0.2, 0.2, 0.2, 0.2);



    glEnable(GL_LIGHT0);

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 10.0, 1.0 };
    glShadeModel (GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

    glEnable(GL_COLOR_MATERIAL);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1);


    bool quit = false;
    bool paused = false;


    while (!quit) {
        Uint32 ticks1 = SDL_GetTicks();

        float dt = 0.01;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_C:
                            clear();
                            break;
                        case SDL_SCANCODE_P:
                            paused = !paused;
                            std::cout << (paused ? "PAUSED" : "RESUMED") << std::endl;
                            break;
                        case SDL_SCANCODE_T:
                            spawnVolume("objects/torus.obj");
                            break;
                        case SDL_SCANCODE_U:
                            spawnVolume("objects/gpgpu.obj");
                            break;
                        case SDL_SCANCODE_G:
                            spawnVolume("objects/sphere.obj");
                            break;
                        case SDL_SCANCODE_M:
                            spawnVolume("objects/solid_smooth_monkey.obj");
                            break;
                        case SDL_SCANCODE_I:
                            inflateAll(dt);
                            break;
                        case SDL_SCANCODE_K:
                            deflateAll(dt);
                            break;
                        case SDL_SCANCODE_X:
                            spheres.push_back(new Sphere);
                            break;
                        case SDL_SCANCODE_SPACE:
                            stepAll(dt);
                            break;
                        default:
                            break;
                    }
            }
        }


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

        int x, y;
        int buttonstate = SDL_GetRelativeMouseState(&x, &y);
        if (buttonstate & 1) {
            cam.right(x / 100.0f * dt);
            cam.down(y / 100.0f * dt);
        }

        if (buttonstate & 4) {
            cam.move_left(x / 10.0f * dt);
            cam.move_up(y / 10.0f * dt);
        }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        cam.look();

        if (!paused) {
            TIME( stepAll(dt) );
        }

        TIME( renderAll() );


        SDL_RenderPresent(renderer);

        Uint32 ticks2 = SDL_GetTicks();

        //std::cout << "frametime: " << (ticks2 - ticks1) << "ms" << std::endl;
        //SDL_Delay(10);
    }

    clear();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}