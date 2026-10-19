#pragma once
#include "Resources/Resource.h"
#include <SDL3/SDL.h>

namespace nu
{
	class IndexBuffer : public Resource
	{
	public:
		~IndexBuffer();

		bool Create(uint32_t indexCount, const uint32_t* data, const class Renderer& renderer);
		bool Create(std::vector<uint32_t> indices, const Renderer& renderer);
		
		uint32_t GetIndexCount() const { return m_indexCount; }

		friend class Renderer;

	private:
		uint32_t m_indexCount{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUBuffer* m_gpuBuffer = nullptr;
	};
}
