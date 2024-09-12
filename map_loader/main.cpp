#include "AssetManager.hpp"
#include "ResourcePacker.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/NavigationGridSystem.hpp"

int main(int argc, char* argv[])
{
    // sage::AssetManager::GetInstance().GenerateBlankJson();
    // return 0;
    sage::AssetManager::GetInstance().LoadPaths();
    sage::ResourcePacker mapLoader;
    entt::registry registry{};
    sage::CollisionSystem collisionSystem(&registry);
    sage::NavigationGridSystem navigationGridSystem(&registry, &collisionSystem);

    InitWindow(300, 100, "Packing Assets...");

    sage::ResourcePacker::PackAssets(&registry, "resources/assets.bin");

    // Should take path and other things as arguments/flags
    sage::ResourcePacker::ConstructMap(
        &registry, &navigationGridSystem, "resources/maps/level", "resources/map1.bin");

    CloseWindow();

    return 0;
}