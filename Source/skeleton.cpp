#include "camera.h"
#include "ray.h"
#include "raytrace.h"
#include "SDLauxiliary.h"
#include "triangle.h"
#include "utils.h"
#include "vector_type.h"

#include <SDL.h>

#include <iostream>
#include <cstdint>

#define RES 750
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES
#define FOCAL_LENGTH  RES
#define FULLSCREEN_MODE false

#undef main // Bloody hell, hope it doesn't come back and haunt me

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */
bool Update();
void Draw(screen *screen);

scg::Camera camera{
    {0, 0, -3.2},
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    FOCAL_LENGTH};

std::vector<scg::Triangle> triangles;

int main(int argc, char *argv[])
{
    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);

    scg::LoadTestModel(triangles);

    while (Update())
    {
        Draw(screen);
        SDL_Renderframe(screen);
    }

    SDL_SaveImage(screen, "screenshot.bmp");

    KillSDL(screen);
    return 0;
}

/*Place your drawing here*/
void Draw(screen *screen)
{
    /* Clear buffer */
    memset(screen->buffer, 0, screen->height * screen->width * sizeof(uint32_t));

    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            scg::Ray ray = camera.getRay(x, y);

            scg::Intersection closestIntersection;
            if (getClosestIntersection(ray, triangles, closestIntersection))
            {
                scg::Vec3f colour = triangles[closestIntersection.triangleIndex].color;
                PutPixelSDL(screen, x, y, scg::Vec3f(colour.r, colour.g, colour.b));
            }
        }
    }
}

/*Place updates of parameters here*/
bool Update()
{
    static int t = SDL_GetTicks();
    /* Compute frame time */
    int t2 = SDL_GetTicks();
    float dt = float(t2 - t);
    t = t2;
    /*Good idea to remove this*/
    std::cout << "Render time: " << dt << " ms." << std::endl;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            return false;
        } else if (e.type == SDL_KEYDOWN)
        {
            int key_code = e.key.keysym.sym;
            switch (key_code)
            {
                case SDLK_UP:
                    /* Move camera forward */
                    camera.position.z += 0.2f;
                    break;
                case SDLK_DOWN:
                    /* Move camera backwards */
                    camera.position.z -= 0.2f;
                    break;
                case SDLK_LEFT:
                    /* Move camera left */
                    camera.position.x -= 0.2f;
                    break;
                case SDLK_RIGHT:
                    /* Move camera right */
                    camera.position.x += 0.2f;
                    break;
                case SDLK_ESCAPE:
                    /* Move camera quit */
                    return false;
            }
        }
    }
    return true;
}