#pragma once

#include "KuchCraft/World/Chunk.h"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		WorldGenerator(Config config);
		~WorldGenerator();

		void GenerateChunk(Ref<Chunk> chunk);

	private:
		Config m_config;

	};
}