#include "pch.h"
#include "Pipeline.h"
#include "Shader.h"

namespace nu
{
	Pipeline::~Pipeline()
	{
		// if both the gpu device and the gpu pipeline are valid (not null),
		// release the pipeline with SDL_ReleaseGPUGraphicsPipeline(device, pipeline)
	}

	bool Pipeline::Create(const Shader& vertexShader, const Shader& fragmentShader, SDL_GPUDevice* gpuDevice, SDL_Window* window)
	{
		// store the gpu device in m_gpuDevice so the destructor can release the pipeline later
		m_gpuDevice = gpuDevice;

		// describe the color target the pipeline renders to
		// the format must match the window's swapchain texture format
		std::array colorTargetDescriptions{
			SDL_GPUColorTargetDescription{
				.format = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window) // todo: get the format with SDL_GetGPUSwapchainTextureFormat(device, window)
			}
		};

		// fill out the pipeline settings
		// each field below needs a value, replace each {} using the comment beside it
		SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{
			// the compiled shaders this pipeline runs (use the m_gpuShader of each shader)
			.vertex_shader = vertexShader.m_gpuShader, // todo: vertex shader
			.fragment_shader = fragmentShader.m_gpuShader, // todo: fragment shader

			// describes how vertex data is laid out in memory
			// these lists are built by AddVertexBuffer() and AddVertexAttribute() before Create() is called
			// sdl needs a pointer to the first element (.data()) and a count (.size() cast to uint32_t)
			.vertex_input_state = SDL_GPUVertexInputState{
				.vertex_buffer_descriptions = m_vertexBufferDescriptions.data(), // todo: pointer to m_vertexBufferDescriptions
				.num_vertex_buffers = (unsigned int)m_vertexBufferDescriptions.size(), // todo: number of vertex buffer descriptions
				.vertex_attributes = m_vertexAttributes.data(), // todo: pointer to m_vertexAttributes
				.num_vertex_attributes = (unsigned int)m_vertexAttributes.size() // todo: number of vertex attributes
			},

			// how vertices are assembled into shapes, every 3 vertices form one triangle
			.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP, // todo: SDL_GPU_PRIMITIVETYPE_...

			// controls how triangles are turned into pixels
			.rasterizer_state = SDL_GPURasterizerState{
				.fill_mode = SDL_GPU_FILLMODE_FILL, // todo: fill the triangles solid (SDL_GPU_FILLMODE_...)
				.cull_mode = SDL_GPU_CULLMODE_NONE, // todo: skip drawing back-facing triangles (SDL_GPU_CULLMODE_...)
				.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE // todo: front faces have counter clockwise winding (SDL_GPU_FRONTFACE_...)
			},

			// the render targets this pipeline draws into, uses the color target array from above
			.target_info = SDL_GPUGraphicsPipelineTargetInfo{
				.color_target_descriptions = colorTargetDescriptions.data(), // todo: pointer to colorTargetDescriptions
				.num_color_targets = colorTargetDescriptions.size() // todo: number of color targets
			}
		};

		// create the pipeline with SDL_CreateGPUGraphicsPipeline(device, &createInfo) and store it in m_gpuPipeline
		m_gpuPipeline = SDL_CreateGPUGraphicsPipeline(m_gpuDevice, &pipelineCreateInfo);

		// if the pipeline is null, creation failed
		// print an error with std::cerr that includes SDL_GetError() and return false
		if (m_gpuPipeline == NULL)
		{
			std::cerr << SDL_GetError();
			return false;
		}

		// pipeline created successfully
		return true;
	}

	void Pipeline::AddVertexBuffer(uint32_t pitch)
	{
		// describe one vertex buffer the pipeline will read from
		SDL_GPUVertexBufferDescription description{
			.slot = (Uint32)m_vertexBufferDescriptions.size(), // todo: the index of this buffer, use the current size of m_vertexBufferDescriptions
			.pitch = pitch, // todo: the size in bytes of one vertex (the pitch parameter)
			.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX, // advance to the next vertex for each vertex drawn
			.instance_step_rate = 0 // only used for instancing
		};

		// add the description to m_vertexBufferDescriptions
		m_vertexBufferDescriptions.push_back(description);
	}

	//void Pipeline::AddIndexBuffer(uint32_t pitch)
	//{
	//	SDL_GPUIndexElementSize();
	//}

	void Pipeline::AddVertexAttribute(uint32_t location, SDL_GPUVertexElementFormat format, uint32_t offset)
	{
		// describe one attribute of a vertex (ex. position, color, uv)
		SDL_GPUVertexAttribute attribute{
			.location = location, // todo: matches layout(location = x) in the vertex shader
			.buffer_slot = 0, // all attributes read from the first vertex buffer
			.format = format, // todo: the data type of the attribute (ex. SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3)
			.offset = offset // todo: the byte offset of this attribute inside one vertex
		};

		// add the attribute to m_vertexAttributes
		m_vertexAttributes.push_back(attribute);
	}
}