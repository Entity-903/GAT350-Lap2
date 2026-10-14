#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Resources/ResourceManager.h"

using namespace nu;

struct Color4
{
    float r, g, b, a;
};

struct Vertex
{
    Vector3 position;
    Color4 color;
};

std::vector<Vertex> vertices =
{
    Vertex{ Vector3{ -0.5f, -0.5f, 0.0f }, Color4{ 1.0f, 1.0f, 1.0f, 0 } }, // Bottom-Left
    Vertex{ Vector3{  0.5f, -0.5f, 0.0f }, Color4{ 0.5f, 0.5f, 0.5f, 0 } }, // Bottom-Right
    Vertex{ Vector3{  0.0f,  0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 0.0f, 0 } }, // Top-Middle
};

struct vertexUniform
{
    glm::mat4 model;
};

vertexUniform uniform;

int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    // Position
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position));
    // Color
    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());

    float scale = 0.5f;
    Color4 color = { 1, 1, 1, 1 };

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

        uniform.model = glm::mat4(1.0f);
        uniform.model = glm::rotate(uniform.model, glm::radians(Engine::Instance().GetTime().GetTime() * 2700.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 5400.0f, 10800.0f
        uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));
        uniform.model = glm::translate(uniform.model, glm::vec3(0.5f, 0.0f, 0.0f));

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexBuffer(*vb);
        Engine::Instance().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        Engine::Instance().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Instance().GetRenderer().EndFrame();
    }// end while

    // SHUTDOWN
    Engine::Instance().Shutdown();    

    return 0;
}