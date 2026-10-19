#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Core/File.h"

namespace nu
{
	Shader::~Shader()
	{
		// if both the gpu device and the gpu shader are valid (not null),
		// release the shader with SDL_ReleaseGPUShader(device, shader)
		if (m_gpuDevice && m_gpuShader)
		{
			SDL_ReleaseGPUShader(m_gpuDevice, m_gpuShader);
		}
	}

	bool Shader::Load(const std::string& filename, Renderer& renderer, uint32_t uniformCount, uint32_t samplerCount)
	{
		// store the renderer's gpu device in m_gpuDevice so the destructor can release the shader later
		m_gpuDevice = renderer.GetGPUDevice();

		// figure out the shader stage from the filename
		// if the filename contains ".vert" set stage to SDL_GPU_SHADERSTAGE_VERTEX
		// if the filename contains ".frag" set stage to SDL_GPU_SHADERSTAGE_FRAGMENT
		// otherwise print an error with std::cerr and return false
		// hint: filename.find(".vert") != std::string::npos is true when ".vert" is in the filename
		SDL_GPUShaderStage stage{};

		if (filename.find(".vert") != std::string::npos)
		{
			stage = SDL_GPU_SHADERSTAGE_VERTEX;
		}
		else if (filename.find(".frag") != std::string::npos)
		{
			stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
		}
		else
		{
			std::cerr << "Invalid Shader Type: " << filename << std::endl;
			return false;
		}

		// the shader format and filename depend on which graphics backend sdl is using
		// (vulkan uses spirv, direct3d 12 uses dxil)
		SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
		std::string shaderFilename = filename;

		// the name of the function the shader starts in
		const char* entrypoint = "main";

		// get the shader formats supported by the gpu device
		// the result is a set of bit flags, use & to check if a format is supported
		SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(m_gpuDevice);
		if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
		{
			// add ".spv" to the end of shaderFilename and set format to SDL_GPU_SHADERFORMAT_SPIRV
			shaderFilename += ".spv";
			format = SDL_GPU_SHADERFORMAT_SPIRV;
		}
		else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
		{
			// add ".dxil" to the end of shaderFilename and set format to SDL_GPU_SHADERFORMAT_DXIL
			shaderFilename += ".dxil";
			format = SDL_GPU_SHADERFORMAT_DXIL;
		}
		else
		{
			// no supported format, print an error with std::cerr and return false
			std::cerr << "Invalid Shader Format: " << filename << std::endl;
			return false;
		}

		// read the compiled shader file into a vector named 'bytes' using ReadBinaryFile()
		// if the vector is empty the read failed, print an error with std::cerr and return false
		std::vector<uint8_t> bytes = ReadBinaryFile(shaderFilename);
		if (bytes.empty())
		{
			std::cerr << "Could not read shader: " << shaderFilename << std::endl;
			return false;
		}

		// fill out the shader settings
		// each field below needs a value, replace each {} using the comment beside it
		SDL_GPUShaderCreateInfo shaderInfo{
			.code_size = bytes.size(), // todo: the number of bytes in the shader code
			.code = bytes.data(), // todo: pointer to the shader code (bytes.data())
			.entrypoint = entrypoint, // todo: the entry function name
			.format = format, // todo: the shader format found above
			.stage = stage, // todo: the shader stage found above
			.num_samplers = samplerCount,
			.num_uniform_buffers = uniformCount
		};

		// create the shader with SDL_CreateGPUShader(device, &createInfo) and store it in m_gpuShader
		m_gpuShader = SDL_CreateGPUShader(m_gpuDevice, &shaderInfo);

		// if the shader is null, creation failed
		// print an error with std::cerr that includes SDL_GetError() and return false
		if (m_gpuShader == nullptr)
		{
			std::cerr << "Could not create shader: " << shaderFilename << " " << SDL_GetError() << std::endl;
			return false;
		}

		// shader loaded successfully
		return true;
	}
}