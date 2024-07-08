//
// Created by Steve Wheeler on 21/03/2024.
//

#include "Serializer.hpp"

#include <fstream>
#include <type_traits>
#include <vector>
#include "cereal/cereal.hpp"
//#include <cereal/archives/json.hpp>
#include "cereal/archives/xml.hpp"
#include "entt/core/hashed_string.hpp"
#include "entt/core/type_traits.hpp"
#include "entt/entity/snapshot.hpp"

#include "components/Transform.hpp"
#include "components/Renderable.hpp"
#include "components/Collideable.hpp"


namespace sage::serializer
{
	struct entity
	{
		unsigned int id;
	};

	template <typename Archive>
	void serialize(Archive& archive, entity& entity)
	{
		archive(entity.id);
	}

	void Save(const entt::registry& source)
	{
		std::cout << "Save called" << std::endl;
		using namespace entt::literals;
		//std::stringstream storage;

		std::ofstream storage("resources/output.xml");
		if (!storage.is_open())
		{
			// Handle file opening error
			return;
		}

		{
			// output finishes flushing its contents when it goes out of scope
			cereal::XMLOutputArchive output{storage};
			const auto view = source.view<Transform, Renderable, Collideable>();
			for (const auto& ent : view)
			{
				const auto& rend = view.get<Renderable>(ent);
				if (!rend.serializable) continue;
				const auto& trans = view.get<Transform>(ent);

				const auto& col = view.get<Collideable>(ent);
				entity entity{};
				entity.id = entt::entt_traits<entt::entity>::to_entity(ent);
				output.setNextName("entity");
				output(entity);
				output.setNextName("transform");
				output(trans);
				output.setNextName("collideable");
				output(col);
				output.setNextName("renderable");
				output(rend);
			}
		}
		storage.close();
		std::cout << "Save finished" << std::endl;
	}

	void Load(entt::registry* destination)
	{
		std::cout << "Load called" << std::endl;
		using namespace entt::literals;
		std::ifstream storage("resources/output.xml");
		if (!storage.is_open())
		{
			// Handle file opening error
			return;
		}

		{
			//cereal::JSONInputArchive input{storage};
			cereal::XMLInputArchive input{storage};

			entt::entity currentEntity{};
			while (input.getNodeName() != nullptr)
			{
				std::string componentName = input.getNodeName();

				//input.startNode();

				if (componentName == "entity")
				{
					// TODO: this is currently pointless, but I don't know how to
					// advance cereal's parser without calling input with an object.
					entity id;
					input(id);
					currentEntity = destination->create();
				}
				else if (componentName == "transform")
				{
					auto& transform = destination->emplace<Transform>(currentEntity);
					input(transform);
				}
				else if (componentName == "collideable")
				{
					auto& col = destination->emplace<Collideable>(currentEntity);
					input(col);
				}
				else if (componentName == "renderable")
				{
					auto& rend = destination->emplace<Renderable>(currentEntity);
					input(rend);
				}
			}
		}
		storage.close();
		std::cout << "Load finished" << std::endl;
	}

	void SerializeKeyMapping(KeyMapping& keymapping, const char* path)
	{
		std::cout << "Save called" << std::endl;
		using namespace entt::literals;
		//std::stringstream storage;

		std::ofstream storage(path);
		if (!storage.is_open())
		{
			// Handle file opening error
			return;
		}

		{
			// output finishes flushing its contents when it goes out of scope
			cereal::XMLOutputArchive output{storage};
			output(keymapping);
		}
		storage.close();
		std::cout << "Save finished" << std::endl;
	}

	void DeserializeKeyMapping(KeyMapping& keymapping, const char* path)
	{
		std::cout << "Load called" << std::endl;
		using namespace entt::literals;

		std::ifstream storage(path);
		if (storage.is_open())
		{
			cereal::XMLInputArchive input{storage};
			input(keymapping);
			storage.close();
		}
		else
		{
			// File doesn't exist, create a new file with the default key mapping
			std::cout << "Key mapping file not found. Creating a new file with the default key mapping." << std::endl;
			SerializeKeyMapping(keymapping, path);
		}
		std::cout << "Load finished" << std::endl;
	}

	void SerializeSettings(Settings& settings, const char* path)
	{
		std::cout << "Save called" << std::endl;
		using namespace entt::literals;
		//std::stringstream storage;

		std::ofstream storage(path);
		if (!storage.is_open())
		{
			// Handle file opening error
			return;
		}

		{
			// output finishes flushing its contents when it goes out of scope
			cereal::XMLOutputArchive output{storage};
			output(settings);
		}
		storage.close();
		std::cout << "Save finished" << std::endl;
	}

	void DeserializeSettings(Settings& settings, const char* path)
	{
		std::cout << "Load called" << std::endl;
		using namespace entt::literals;

		std::ifstream storage(path);
		if (storage.is_open())
		{
			cereal::XMLInputArchive input{storage};
			input(settings);
			storage.close();
		}
		else
		{
			// File doesn't exist, create a new file with the default key mapping
			std::cout << "Key mapping file not found. Creating a new file with the default key mapping." << std::endl;
			SerializeSettings(settings, path);
		}
		std::cout << "Load finished" << std::endl;
	}
    
    float GetMaxHeight(entt::registry* registry)
    {
        float max = 0;
        
        auto view = registry->view<Collideable, Renderable>();

        for (const auto& entity: view) 
        {
            const auto& r = registry->get<Renderable>(entity);
            if (!r.serializable) continue;
            const auto& c = registry->get<Collideable>(entity);
            if (c.collisionLayer != CollisionLayer::FLOOR) continue;
            if (c.worldBoundingBox.max.y > max)
            {
                max = c.worldBoundingBox.max.y;
            }
        }
        
        return max;
    }

    void GenerateHeightMap(entt::registry* registry, const std::vector<std::vector<NavigationGridSquare*>>& gridSquares)
    {
        float maxHeight = GetMaxHeight(registry);
        int slices = gridSquares.size();
        Image heightMap = GenImageColor(slices, slices, BLACK);
        std::cout << "Generating height map..." << std::endl;
        for (int y = 0; y < slices; ++y)
        {
            for (int x = 0; x < slices; ++x)
            {
                float height = gridSquares[y][x]->terrainHeight;
                unsigned char heightValue = static_cast<unsigned char>((height / maxHeight) * 255.0f);
                Color pixelColor = { heightValue, heightValue, heightValue, 255 };
                ImageDrawPixel(&heightMap, x, y, pixelColor);
            }
        }
    
        ExportImage(heightMap, "output.png");
        UnloadImage(heightMap);
    
        std::cout << "Height map saved as '" << "output.png" << "'" << std::endl;
    }
}
