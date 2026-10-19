#pragma once
#include "Resources/Resource.h"
#include <SDL3/SDL.h>

namespace nu
{
	class IndexBuffer : public Resource
	{
	public:
		~IndexBuffer();

		bool Create(uint32_t indexCount, uint32_t indexSize, const uint8_t* data, SDL_GPUDevice* gpuDevice);
		
		template<typename T>
		bool Create(const std::vector<T>& indices, const class Renderer& renderer);
		
		uint32_t GetIndexCount() const { return m_indexCount; }

		friend class Renderer;

	private:
		uint32_t m_indexCount{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUBuffer* m_gpuBuffer = nullptr;
	};

	template<typename T>
	inline bool IndexBuffer::Create(const std::vector<T>& indices, const class Renderer& renderer)
	{
		return Create(
			static_cast<uint32_t>(indices.size()),
			sizeof(T),
			reinterpret_cast<const uint8_t*>(indices.data()),
			renderer);
	}
}
