#pragma once

#include "components/NavigationGridSquare.hpp"
#include "raylib.h"
#include "slib.hpp"
#include "systems/NavigationGridSystem.hpp"
#include <entt/entity/registry.hpp>

namespace sage
{

    class TextureTerrainOverlay
    {
      private:
        entt::registry* registry;
        NavigationGridSystem* navigationGridSystem;
        Texture texture;
        GridSquare lastHit{};
        GridSquare minRange{}, maxRange{};
        bool initialised = false;
        bool m_active = false;
        Vector3 meshOffset{};

        void updateTerrainPolygon();
        ModelSafe generateTerrainPolygon();
        void updateMeshData(Mesh& mesh);
        Mesh createInitialMesh();
        void updateVertexData(Mesh& mesh, int vertexIndex, int gridRow, int gridCol);
        void updateNormalData(Mesh& mesh, int vertexIndex, int gridRow, int gridCol);
        void updateTexCoordData(Mesh& mesh, int vertexIndex, int row, int col, int maxRow, int maxCol);
        void generateIndices(Mesh& mesh, int maxRow, int maxCol);

      public:
        const entt::entity entity;

        void Enable(bool enable);
        void Init(Vector3 mouseRayHit);
        bool active() const;
        void Update(Vector3 mouseRayHit);
        ~TextureTerrainOverlay();
        TextureTerrainOverlay(
            entt::registry* _registry,
            NavigationGridSystem* _navigationGridSystem,
            const char* texturePath,
            Color _hint,
            const char* shaderPath);
    };

} // namespace sage