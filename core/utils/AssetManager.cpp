#include "AssetManager.hpp"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "entt/core/hashed_string.hpp"
#include "entt/core/type_traits.hpp"
#include "raylib-cereal.hpp"
#include "raylib.h"
#include <cassert>
#include <cereal/archives/json.hpp>
#include <entt/entt.hpp>
#include <fstream>
#include <magic_enum.hpp>
#include <type_traits>
#include <vector>

namespace sage
{

    void AssetManager::addAsset(AssetID asset, const std::string& path)
    {
        assert(!assetMap.contains(asset));
        // Could check if path has been referenced by other object. Easy to do with a two-way map
        assert(FileExists(path.c_str()));
        assetMap.emplace(asset, path);
    }

    const std::string& AssetManager::GetAssetPath(AssetID asset)
    {
        return assetMap.at(asset);
    }

    void AssetManager::GenerateBlankJson()
    {
        for (int i = 0; i < magic_enum::enum_underlying(AssetID::COUNT); ++i)
        {
            auto e = magic_enum::enum_cast<AssetID>(i).value();
            assetMap.emplace(e, "");
        }
        SavePaths();
    }

    void AssetManager::SavePaths()
    {
        const std::string& jsonPath = "resources/asset-paths.json";
        std::cout << "START: Saving asset paths to JSON file \n";
        using namespace entt::literals;
        if (FileExists(jsonPath.c_str()))
        {
            auto file = LoadFileText(jsonPath.c_str());
            SaveFileText(std::string(std::string(jsonPath) + ".bak").c_str(), file);
            UnloadFileText(file);
        }

        std::ofstream storage(jsonPath);
        if (!storage.is_open())
        {
            return;
        }

        {
            cereal::JSONOutputArchive output{storage};
            output.setNextName("AssetManager");
            output(GetInstance());
        }
        storage.close();
        std::cout << "FINISH: Saving asset paths to JSON file \n";
    }

    void AssetManager::LoadPaths()
    {
        const std::string& jsonPath = "resources/asset-paths.json";
        std::cout << "START: Loading asset paths from JSON file \n";

        if (!FileExists(jsonPath.c_str()))
        {
            std::cout << "WARNING: No asset path file detected. \n";
            assert(0);
        }

        using namespace entt::literals;
        std::ifstream storage(jsonPath);
        if (storage.is_open())
        {
            cereal::JSONInputArchive input{storage};
            input(GetInstance());
            storage.close();
        }
        std::cout << "FINISH: Loading asset paths from JSON file \n";
    }

}; // namespace sage