#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Shader.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Resources/ResourceManager.h"

using namespace nu;

struct Color4
{
    float r, g, b, a;
};

struct Index
{
    Vector3 position;
    Color4 color;
};

std::vector<Index> indices =
{
    Index{ Vector3{ -0.5f, -0.5f, 0.0f }, Color4{ 1.0f, 1.0f, 1.0f, 0 } }, // Bottom-Left
    Index{ Vector3{  0.5f, -0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 0.0f, 0 } }, // Bottom-Right
    Index{ Vector3{  0.5f,  0.5f, 0.0f }, Color4{ 1.0f, 1.0f, 1.0f, 0 } }, // Top-Right
    Index{ Vector3{ -0.5f,  0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 0.0f, 0 } }, // Top-Left
};

std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };




int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(indices.size(), indices.data(), Engine::Instance().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    // Position
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Index, position));
    // Color
    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Index, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());

    Color4 color = { 1, 1, 1, 1 };
    struct indexUniform
    {
        glm::mat4 model;
    };

    indexUniform uniform;
    glm::vec3 cameraPosition{ 0, 0, 5 };

    // MAIN LOOP
    bool quit = false;
    while(!quit) 
    {
        // UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                quit = true;
            }
        }

        // ENGINE

        Engine::Instance().Update();
        float dt = Engine::Instance().GetTime().GetDeltaTime();
        float time = Engine::Instance().GetTime().GetTime();

        glm::vec3 direction{ 0 };
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = +1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = +1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.y = +1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.y = -1; 

        cameraPosition += direction * 4.0f * dt;

        // MODEL MATRIX
        // 1 0 0 0
        // 0 1 0 0
        // 0 0 1 0
        // 0 0 0 1

        //uniform.model = glm::mat4(1.0f);
        //uniform.model = glm::rotate(uniform.model, glm::radians(time * 2700.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 5400.0f, 10800.0f
        //uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));
        //uniform.model = glm::translate(uniform.model, glm::vec3(0.5f, 0.0f, 0.0f));

        // 1 0 0 t
        // 0 1 0 t
        // 0 0 1 t
        // 0 0 0 1
        auto mxt = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.5f, 0.0f, 0.0f));


        //auto mxrx = glm::rotate(glm::mat4{ 1 }, glm::radians(time * 2700.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //auto mxry = glm::rotate(glm::mat4{ 1 }, glm::radians(time * 2700.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //auto mxr = mxrx * mxry;
        auto mxr = glm::eulerAngleYXZ(1.2f, time * 3.0f, 0.0f);
        // x 0 0 0
        // 0 y 0 0
        // 0 0 z 0
        // 0 0 0 1
        auto mxs = glm::scale(uniform.model, glm::vec3(1));
        uniform.model = mxs * mxr * mxt;

        // VIEW MATRIX
        auto view = glm::lookAtRH(glm::vec3{ 0,0,5 }, glm::vec3(0), glm::vec3{0, 1, 0});

        // PROJECTION MATRIX
        // TODO: ensure this is the proper value of aspectRatio
        float aspectRatio = Engine::Instance().GetRenderer().GetWidth() / Engine::Instance().GetRenderer().GetHeight();
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);

        uniform.mvp = projection * view * model;

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexBuffer(*vb);
        Engine::Instance().GetRenderer().SetIndexBuffer(*ib);
        Engine::Instance().GetRenderer().SetIndexUniform(0, &uniform, sizeof(uniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        Engine::Instance().GetRenderer().DrawIndex(*ib);

        Engine::Instance().GetRenderer().EndFrame();
    }// end while

    // SHUTDOWN
    Engine::Instance().Shutdown();    

    return 0;
}