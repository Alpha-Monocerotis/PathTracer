#include "camera.h"
#include "light.h"
#include "object.h"
#include "ray.h"
#include "raytrace.h"
#include "sampler.h"
#include "scene.h"
#include "SDLauxiliary.h"
#include "triangle.h"
#include "utils.h"
#include "vector_type.h"

#include <SDL.h>

#include <iostream>
#include <cstdint>
#include <cstring>
#include <memory>

#define RES 450
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES
#define FOCAL_LENGTH  RES
#define FULLSCREEN_MODE false

#undef main // Bloody hell, hope it doesn't come back and haunt me

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */
bool Update();
void Draw(screen *screen);
void InitialiseBuffer();

scg::Sampler sampler;

scg::Camera camera{
    scg::Vec3f(0, 0, -3),
    scg::Vec3f(0, 0, 0),
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    FOCAL_LENGTH};

scg::Scene scene;

int samples;
scg::Vec3f buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

int main(int argc, char *argv[])
{
    InitialiseBuffer();

    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);

    // Load scene and lights
    scene = scg::LoadTestModel();

    // Point lights
    //scene.lights.emplace_back(std::make_shared<scg::PointLight>(scg::PointLight{{1.0f, 1.0f, 1.0f}, 20, {0, -0.75, 0}}));
    //scene.lights.emplace_back(std::make_shared<scg::PointLight>(scg::PointLight{{1.0f, 1.0f, 1.0f}, 10, {-0.5, -0.75, 0}}));
    //scene.lights.emplace_back(std::make_shared<scg::PointLight>(scg::PointLight{{1.0f, 1.0f, 1.0f}, 10, {0.5, -0.75, 0}}));
    // Directional lights
    //scene.lights.emplace_back(std::make_shared<scg::DirectionalLight>(scg::DirectionalLight{{1.0f, 1.0f, 1.0f}, 5, {0, 0.5, 0.5}}));

//*
    // Ceiling light
    size_t index = scene.materials.size();

    float L = 1;
    scg::Vec3f E(L / 2, 0, -L / 2);
    scg::Vec3f F(-L / 2, 0, -L / 2);
    scg::Vec3f G(L / 2, 0, L / 2);
    scg::Vec3f H(-L / 2, 0, L / 2);
    std::vector<scg::Triangle> triangles{
        scg::Triangle(G, F, E, index),
        scg::Triangle(G, H, F, index)
    };
    std::shared_ptr<scg::Object> objectPtr = std::make_shared<scg::Object>(scg::Object{
        { 0, -0.99, 0},
        std::make_shared<scg::Mesh>(triangles)
    });
//    std::shared_ptr<scg::Object> objectPtr = std::make_shared<scg::Object>(scg::Object{
//        { 0, -0.7, 0},
//        std::make_shared<scg::Sphere>(0.2, index)
//    });
    scene.objects.emplace_back(objectPtr);
    std::shared_ptr<scg::ObjectLight> lightPtr = std::make_shared<scg::ObjectLight>(scg::ObjectLight{
        {1.0f, 1.0f, 1.0f}, 40,
        objectPtr
    });
    scene.lights.emplace_back(lightPtr);
    scene.materials.emplace_back(std::make_shared<scg::Material>(scg::Material{{0.75f, 0.75f, 0.75f}, lightPtr}));
//*/




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
    ++samples;

    /* Clear buffer */
    //memset(screen->buffer, 0, screen->height * screen->width * sizeof(uint32_t));

    // TODO: reseed generator
    #pragma omp parallel for schedule(dynamic) collapse(2)// firstprivate(generator, distribution)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            scg::Ray ray = camera.getRay(x, y);

            int depth = 3;
            scg::Vec3f colour = scg::trace(scene, ray, depth, sampler);
            buffer[y][x] += colour; // TODO: clamp value

            PutPixelSDL(screen, x, y, buffer[y][x] / samples);
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
    std::cout << "Iteration: " << samples << ". Render time: " << dt << " ms." << std::endl;

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
                case SDLK_ESCAPE:
                    /* Move camera quit */
                    return false;
                case SDLK_w:
                    /* Move camera forward */
                    camera.position.z += 0.2f;
                    InitialiseBuffer();
                    break;
                case SDLK_s:
                    /* Move camera backwards */
                    camera.position.z -= 0.2f;
                    InitialiseBuffer();
                    break;
                case SDLK_a:
                    /* Move camera left */
                    camera.position.x -= 0.2f;
                    InitialiseBuffer();
                    break;
                case SDLK_d:
                    /* Move camera right */
                    camera.position.x += 0.2f;
                    InitialiseBuffer();
                    break;
                case SDLK_r:
                    InitialiseBuffer();
                    break;
                case SDLK_UP:
                    /* Move rotate up */
                    camera.rotation.x += 5;
                    InitialiseBuffer();
                    break;
                case SDLK_DOWN:
                    /* Move rotate down */
                    camera.rotation.x -= 5;
                    InitialiseBuffer();
                    break;
                case SDLK_LEFT:
                    /* Move rotate left */
                    camera.rotation.y -= 5;
                    InitialiseBuffer();
                    break;
                case SDLK_RIGHT:
                    /* Move rotate right */
                    camera.rotation.y += 5;
                    InitialiseBuffer();
                    break;
            }
        }
    }
    return true;
}

void InitialiseBuffer()
{
    samples = 0;
    memset(buffer, 0, sizeof(buffer));
}