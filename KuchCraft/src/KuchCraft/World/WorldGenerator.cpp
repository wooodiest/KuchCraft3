#include "kcpch.h"
#include "KuchCraft/World/WorldGenerator.h"

namespace KuchCraft {

	WorldGenerator::WorldGenerator(Config config)
		: m_config(config)
	{
	}

	WorldGenerator::~WorldGenerator()
	{
	}

	void WorldGenerator::GenerateChunk(Ref<Chunk> chunk)
	{
		for (uint32_t y = 0; y < chunk_size_y; y++)
		{
			for (uint32_t z = 0; z < chunk_size_z; z++)
			{
				for (uint32_t x = 0; x < chunk_size_x; x++)
				{
					if (y < 50 + glm::sin((x + z) * 0.3) * 5)
					{
						Block block;

						if (x % 3 == 0)
							block.SetId(1);
						else if (x % 3 == 1)
							block.SetId(2);
						else if (x % 3 == 2)
							block.SetId(3);

						chunk->SetBlock({ x, y, z }, block);
					}
				}
			}
		}
	}
}