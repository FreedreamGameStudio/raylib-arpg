#include "ResourcePacker.hpp"

#include "AssetManager.hpp"
#include "components/Collideable.hpp"
#include "components/Renderable.hpp"
#include "components/sgTransform.hpp"
#include "components/Spawner.hpp"
#include "GameObjectFactory.hpp"
#include "Light.hpp"
#include "LightManager.hpp"
#include "ResourceManager.hpp"
#include "Serializer.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/NavigationGridSystem.hpp"

#include "raylib.h"
#include "raymath.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace sage
{

    constexpr float WORLD_SCALE = 5.0f;

    Vector3 scaleFromOrigin(const Vector3& point, float scale)
    {
        return Vector3Scale(point, scale);
    }

    CollisionLayer getCollisionLayer(const std::string& objectName)
    {
        if (objectName.find("_BLD_") != std::string::npos)
        {
            return CollisionLayer::BUILDING;
        }
        if (objectName.find("_WALL_") != std::string::npos)
        {
            return CollisionLayer::BUILDING;
        }
        if (objectName.find("_HOLE_") != std::string::npos)
        {
            return CollisionLayer::BUILDING;
        }
        if (objectName.find("_BG_") != std::string::npos)
        {
            return CollisionLayer::BACKGROUND;
        }
        if (objectName.find("_FLOORSIMPLE_") != std::string::npos)
        {
            // Uses bounding box bounds for height (flat surfaces).
            return CollisionLayer::FLOORSIMPLE;
        }
        if (objectName.find("_FLOORCOMPLEX_") != std::string::npos)
        {
            // Samples mesh for height/normal information
            return CollisionLayer::FLOORCOMPLEX;
        }
        if (objectName.find("_PROP_") != std::string::npos)
        {
            return CollisionLayer::BUILDING;
        }
        if (objectName.find("_STAIRS_") != std::string::npos)
        {
            return CollisionLayer::STAIRS;
        }
        if (objectName.find("_MAPBASE_") != std::string::npos)
        {
            return CollisionLayer::BACKGROUND;
        }

        return CollisionLayer::BACKGROUND; // by default, objects are ignored
    }

    std::string readLine(std::ifstream& infile, const std::string& key)
    {
        std::string line;
        std::getline(infile, line);
        if (line.substr(0, key.length()) != key)
        {
            throw std::runtime_error("Expected key '" + key + "' not found");
        }

        // Remove carriage returns from the line
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        return line.substr(key.length() + 2); // +2 to skip ": "
    }

    void HandleLight(entt::registry* registry, std::ifstream& infile)
    {
        std::string light_type;
        std::string objectName;
        float x, y, z;
        int r, g, b, s;
        try
        {
            light_type = readLine(infile, "light_type");

            objectName = readLine(infile, "name");

            std::istringstream locStream(readLine(infile, "location"));
            locStream >> x >> y >> z;

            std::istringstream rgbStream(readLine(infile, "color"));
            rgbStream >> r >> g >> b;

            std::istringstream strStream(readLine(infile, "strength"));
            strStream >> s;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing spawner data: " << e.what() << std::endl;
            assert(0);
        }

        auto entity = registry->create();
        auto& light = registry->emplace<Light>(entity);
        light.target = Vector3Zero();

        if (light_type == "sun")
        {
            light.type = LIGHT_DIRECTIONAL;
            light.position = {0, 1500, 0};
            light.brightness = 0.75; // TODO
        }
        else if (light_type == "point")
        {
            light.type = LIGHT_POINT;
            light.position = scaleFromOrigin({x, y, z}, WORLD_SCALE);
            light.brightness = s / 150; // Seems to work well.
        }

        light.color =
            Color{static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), 1};
    }

    void HandleSpawner(entt::registry* registry, std::ifstream& infile)
    {
        std::string objectName;
        float x, y, z, rotx, roty, rotz;
        try
        {
            objectName = readLine(infile, "name");

            std::istringstream locStream(readLine(infile, "location"));
            locStream >> x >> y >> z;

            std::istringstream rotStream(readLine(infile, "rotation"));
            rotStream >> rotx >> roty >> rotz;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing spawner data: " << e.what() << std::endl;
            assert(0);
        }

        auto entity = registry->create();

        Vector3 scaledPosition = scaleFromOrigin({x, y, z}, WORLD_SCALE);
        auto& spawner = registry->emplace<Spawner>(entity);
        spawner.pos = {scaledPosition.x, scaledPosition.y, scaledPosition.z};
        spawner.rot = {rotx, roty, rotz};
        if (objectName.find("Goblin") != std::string::npos)
        {
            spawner.spawnerType = SpawnerType::GOBLIN;
        }
        else if (objectName.find("Player") != std::string::npos)
        {
            spawner.spawnerType = SpawnerType::PLAYER;
        }
    }

    void HandleMesh(entt::registry* registry, std::ifstream& infile, const fs::path& meshPath, int& slices)
    {
        std::string meshName, objectName;
        float x, y, z, rotx, roty, rotz, scalex, scaley, scalez;
        try
        {
            objectName = readLine(infile, "name");
            meshName = readLine(infile, "mesh");

            std::istringstream locStream(readLine(infile, "location"));
            locStream >> x >> y >> z;

            std::istringstream rotStream(readLine(infile, "rotation"));
            rotStream >> rotx >> roty >> rotz;

            std::istringstream scaleStream(readLine(infile, "scale"));
            scaleStream >> scalex >> scaley >> scalez;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing mesh data: " << e.what() << std::endl;
            assert(0);
        }

        fs::path fullMeshPath = meshPath / meshName;
        std::string meshKey = fullMeshPath.generic_string();

        auto entity = registry->create();

        auto model = ResourceManager::GetInstance().GetModelCopy(meshKey);
        assert(!meshKey.empty());
        model.SetKey(meshKey);

        Vector3 scaledPosition = scaleFromOrigin({x, y, z}, WORLD_SCALE);
        Matrix rotMat =
            MatrixMultiply(MatrixMultiply(MatrixRotateZ(rotz), MatrixRotateY(roty)), MatrixRotateX(rotx));
        Matrix transMat = MatrixTranslate(scaledPosition.x, scaledPosition.y, scaledPosition.z);
        Matrix scaleMat = MatrixScale(scalex * WORLD_SCALE, scaley * WORLD_SCALE, scalez * WORLD_SCALE);

        Matrix mat = MatrixMultiply(MatrixMultiply(scaleMat, rotMat), transMat);

        auto& renderable = registry->emplace<Renderable>(entity, std::move(model), mat);
        renderable.name = objectName;

        auto& trans = registry->emplace<sgTransform>(entity, entity);

        auto& collideable = registry->emplace<Collideable>(
            entity, renderable.GetModel()->CalcLocalBoundingBox(), trans.GetMatrix());

        collideable.collisionLayer = getCollisionLayer(objectName);

        if (objectName.find("_MAPBASE_") != std::string::npos)
        {
            slices = std::ceil(std::max(
                collideable.worldBoundingBox.max.x - collideable.worldBoundingBox.min.x,
                collideable.worldBoundingBox.max.z - collideable.worldBoundingBox.min.z));
            if (slices % 2 == 1)
            {
                slices += 1;
            }
        }
    }

    void processTxtFile(entt::registry* registry, const fs::path& meshPath, const fs::path& txtPath, int& slices)
    {
        std::string typeName;
        std::ifstream infile(txtPath);

        try
        {
            typeName = readLine(infile, "type");
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing file " << txtPath << ": " << e.what() << std::endl;
            assert(0);
        }

        if (typeName.find("spawner") != std::string::npos)
        {
            HandleSpawner(registry, infile);
        }
        else if (typeName.find("light") != std::string::npos)
        {
            HandleLight(registry, infile);
        }
        else
        {
            HandleMesh(registry, infile, meshPath, slices);
        }
    }

    void ResourcePacker::ConstructMap(
        entt::registry* registry,
        NavigationGridSystem* navigationGridSystem,
        const char* input,
        const char* output)
    {
        registry->clear();
        ResourceManager::GetInstance().Reset();

        fs::path inputPath(input);
        fs::path meshPath = inputPath / "mesh";
        if (!fs::exists(inputPath) || !fs::is_directory(meshPath))
        {
            std::cout << "ERROR: MapLoader -> Directory does not exist or path invalid." << std::endl;
            exit(1);
        }

        InitWindow(300, 100, "Loading Map!");

        std::cout << "START: Constructing map into bin file. \n";

        std::cout << "START: Loading mesh data into resource manager. \n";
        for (const auto& entry : fs::directory_iterator(meshPath))
        {
            if (entry.path().extension() == ".obj" || entry.path().extension() == ".glb" ||
                entry.path().extension() == ".gltf")
            {
                ResourceManager::GetInstance().ModelLoadFromFile(entry.path().generic_string());
            }
        }
        std::cout << "FINISH: Loading mesh data into resource manager. \n";

        int slices = 0;

        std::cout << "START: Processing txt data into resource manager. \n";
        for (const auto& entry : fs::directory_iterator(inputPath))
        {
            if (entry.path().extension() == ".txt")
            {
                processTxtFile(registry, meshPath, entry.path(), slices);
            }
        }
        std::cout << "FINISH: Processing txt data into resource manager. \n";

        ImageSafe heightMap(false), normalMap(false);

        navigationGridSystem->Init(slices, 1.0f);
        navigationGridSystem->InitGridHeightAndNormals();
        navigationGridSystem->GenerateHeightMap(heightMap);
        navigationGridSystem->GenerateNormalMap(normalMap);

        ExportImage(heightMap.GetImage(), "resources/HeightMap.png");
        ExportImage(normalMap.GetImage(), "resources/NormalMap.png");

        ResourceManager::GetInstance().ImageLoadFromFile("HEIGHT_MAP", heightMap.GetImage());
        ResourceManager::GetInstance().ImageLoadFromFile("NORMAL_MAP", normalMap.GetImage());

        serializer::SaveMap(*registry, output);
        std::cout << "FINISH: Constructing map into bin file. \n";
    }

    /**
     * output: The path + filename of the resulting binary
     **/
    void ResourcePacker::PackAssets(entt::registry* registry, const std::string& output)
    {
        registry->clear();
        ResourceManager::GetInstance().Reset();

        fs::path outputPath(output);
        if (!fs::is_directory(outputPath.parent_path()))
        {
            std::cout << "ResourcePacker: Directory does not exist, cannot save. Aborting... \n";
            return;
        }
        if (outputPath.extension() != ".bin")
        {
            std::cout << "ResourcePacker: File extension for output file must be 'bin'. Aborting... \n";
            return;
        }

        AssetManager::GetInstance().LoadPaths();

        std::cout << "START: Loading assets into memory \n";

        for (int i = 0; i < magic_enum::enum_underlying(AssetID::COUNT); ++i)
        {
            auto id = magic_enum::enum_cast<AssetID>(i).value();
            auto name = std::string(magic_enum::enum_name(id));
            auto tag = name.substr(0, 3);

            if (tag == "IMG")
            {
                ResourceManager::GetInstance().ImageLoadFromFile(id);
            }
            else if (tag == "MDL")
            {
                ResourceManager::GetInstance().ModelLoadFromFile(id);

                fs::path assetPath = AssetManager::GetInstance().GetAssetPath(id);
                if (assetPath.extension() == ".glb" || assetPath.extension() == ".gltf")
                {
                    ResourceManager::GetInstance().ModelAnimationLoadFromFile(id);
                }
                // TODO: See if txt of same name/path exists and parse that for socket data etc
            }
        }

        // TODO: Make these use AssetIDs and move paths to json
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/equipment.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/hammer.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/inventory.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/spellbook.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/transpixel.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/9patch.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/empty.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ninepatch_button.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icon.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/frame.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/empty-inv_slot.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/window_hud.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/window_dialogue.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/inventory-bg.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/scroll-bg.png");

        std::cout << "FINISH: Loading assets into memory \n";
        serializer::SaveClassBinary(output.c_str(), ResourceManager::GetInstance());
    }
}; // namespace sage