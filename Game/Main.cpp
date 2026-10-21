#include "Engine.h"
#include "Core/File.h"
#include "Math/Transform.h"
#include "Renderer/Shader.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Model.h"

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

struct Vertex
{
    Vector3 position;
    Color4 color;
};

std::vector<Vertex> vertices =
{
    { Vector3{ -0.5f, -0.5f, 0.0f }, Color4{ 1.0f, 1.0f, 1.0f, 0 } }, // Bottom-Left
    { Vector3{  0.5f, -0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 0.0f, 0 } }, // Bottom-Right
    { Vector3{  0.5f,  0.5f, 0.0f }, Color4{ 1.0f, 1.0f, 1.0f, 0 } }, // Top-Right
    { Vector3{ -0.5f,  0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 0.0f, 0 } }, // Top-Left
};

std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create(vertices, Engine::Instance().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(indices, Engine::Instance().GetRenderer());

    auto model = Resources().Get<Model>("models/suzanne.obj", Engine::Instance().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/unlit.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/unlit.frag", Engine::Instance().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());

    //auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    //auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    //auto pipeline = std::make_shared<Pipeline>();
    //pipeline->AddVertexBuffer(sizeof(Vertex));
    //// Position
    //pipeline->AddVertexAttribute(
    //    0,
    //    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
    //    offsetof(Vertex, position));
    //// Color
    //pipeline->AddVertexAttribute(
    //    1,
    //    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
    //    offsetof(Vertex, color));

    //pipeline->Create(*vshader.get(), *fshader.get(),
    //    Engine::Instance().GetRenderer().GetGPUDevice(),
    //    Engine::Instance().GetRenderer().GetWindow());

    Color4 color = { 1, 1, 1, 1 };
    struct vertexUniform
    {
        glm::mat4 mvp;
    };

    vertexUniform uniform;
    glm::vec3 cameraPosition{ 0, 0, 5 };

    Transform transform;

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

        // TRANSFORM
        transform.rotation.z = time * 90;
        //uniform.model = mxs * mxr * mxt;

        // VIEW MATRIX
        auto view = glm::lookAtRH(glm::vec3{ 0,0,5 }, glm::vec3(0), glm::vec3{0, 1, 0});

        // PROJECTION MATRIX
        float aspectRatio = Engine::Instance().GetRenderer().GetWidth() / (float)Engine::Instance().GetRenderer().GetHeight();
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);

        // MVP
        uniform.mvp = projection * view * transform.GetMatrix();

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        model->Draw(Engine::Instance().GetRenderer());

        Engine::Instance().GetRenderer().EndFrame();
    }// end while

    // SHUTDOWN
    Engine::Instance().Shutdown();    

    return 0;
}