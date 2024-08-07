//
// Created by Steve Wheeler on 04/05/2024.
//

#include "Cursor.hpp"

#include "GameData.hpp"

#ifndef FLT_MAX
#define FLT_MAX                                                                          \
    340282346638528859811704183484516925440.0f // Maximum value of a float, from bit
                                               // pattern 01111111011111111111111111111111
#endif

namespace sage
{
    void Cursor::onMouseClick()
    {
        if (!enabled) return;

        const auto& layer =
            registry->get<Collideable>(m_mouseHitInfo.collidedEntityId).collisionLayer;
        if (layer == CollisionLayer::NPC)
        {
            onNPCClick.publish(m_mouseHitInfo.collidedEntityId);
        }
        else if (layer == CollisionLayer::FLOOR)
        {
            onFloorClick.publish(m_mouseHitInfo.collidedEntityId);
        }
        else if (layer == CollisionLayer::ENEMY)
        {
            onEnemyClick.publish(m_mouseHitInfo.collidedEntityId);
        }
        onAnyClick.publish(m_mouseHitInfo.collidedEntityId);
    }

    void Cursor::DisableContextSwitching() // Lock mouse context? Like changing depending
                                           // on collision.
    {
        contextLocked = true;
    }

    void Cursor::EnableContextSwitching()
    {
        contextLocked = false;
    }

    void Cursor::Enable()
    {
        enabled = true;
    }

    void Cursor::Disable()
    {
        enabled = false;
    }

    void Cursor::Hide()
    {
        hideCursor = true;
    }

    void Cursor::Show()
    {
        hideCursor = false;
    }

    bool Cursor::isValidMove() const
    {
        GridSquare clickedSquare{};
        if (gameData->navigationGridSystem->WorldToGridSpace(
                m_mouseHitInfo.rlCollision.point, clickedSquare))
        // Out of map bounds (TODO: Potentially pointless, if FLOOR is the same size as
        // bounds.)
        {
            if (registry->any_of<ControllableActor>(controlledActor))
            {
                const auto& actor = registry->get<ControllableActor>(controlledActor);
                GridSquare minRange{};
                GridSquare maxRange{};
                gameData->navigationGridSystem->GetPathfindRange(
                    controlledActor, actor.pathfindingBounds, minRange, maxRange);
                if (!gameData->navigationGridSystem->WorldToGridSpace(
                        m_mouseHitInfo.rlCollision.point,
                        clickedSquare,
                        minRange,
                        maxRange))
                // Out of player's movement range
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
        if (gameData->navigationGridSystem
                ->GetGridSquare(clickedSquare.row, clickedSquare.col)
                ->occupied)
        {
            return false;
        }
        return true;
    }

    void Cursor::changeCursors(CollisionLayer layer)
    {
        if (contextLocked) return;

        if (layer == CollisionLayer::FLOOR || layer == CollisionLayer::NAVIGATION)
        {
            if (isValidMove())
            {
                currentTex = &movetex;
                currentColor = GREEN;
            }
            else
            {
                currentTex = &invalidmovetex;
                currentColor = invalidColor;
            }
            if (registry->all_of<Renderable>(m_mouseHitInfo.collidedEntityId))
            {
                hitObjectName =
                    registry->get<Renderable>(m_mouseHitInfo.collidedEntityId).name;
            }
        }
        else if (layer == CollisionLayer::BUILDING)
        {
            currentTex = &regulartex;
            currentColor = invalidColor;
            if (registry->all_of<Renderable>(m_mouseHitInfo.collidedEntityId))
            {
                hitObjectName =
                    registry->get<Renderable>(m_mouseHitInfo.collidedEntityId).name;
            }
        }
        else if (layer == CollisionLayer::PLAYER)
        {
            currentTex = &regulartex;
            hitObjectName = "Player";
        }
        else if (layer == CollisionLayer::NPC)
        {
            currentTex = &talktex;
            hitObjectName = "NPC";
        }
        else if (layer == CollisionLayer::ENEMY)
        {
            currentTex = &combattex;
            hitObjectName = "NPC";
        }
    }

    void Cursor::getMouseRayCollision()
    {
        // Reset hit information
        resetHitInfo(m_mouseHitInfo);
        resetHitInfo(m_terrainHitInfo);
        hitObjectName = "None";
        currentTex = &regulartex;
        currentColor = defaultColor;

        // Get ray and test against objects
        ray = GetMouseRay(GetMousePosition(), *gameData->camera->getRaylibCam());
        auto collisions = gameData->collisionSystem->GetCollisionsWithRay(ray);

        if (collisions.empty())
        {
            return;
        }

        // Set initial hit info
        m_mouseHitInfo = collisions[0];

        // Find first non-floor collision for mouse hit
        auto nonFloorIt = std::find_if(
            collisions.begin(), collisions.end(), [](const CollisionInfo& coll) {
                return coll.collisionLayer != CollisionLayer::FLOOR;
            });

        if (nonFloorIt != collisions.end())
        {
            m_mouseHitInfo = *nonFloorIt;
            m_terrainHitInfo =
                collisions[0]; // Keep the first hit (likely floor) as terrain hit
            findMeshCollision(m_terrainHitInfo);
        }
        else
        {
            m_terrainHitInfo = m_mouseHitInfo;
        }

        findMeshCollision(m_mouseHitInfo);

        onCollisionHit.publish(m_mouseHitInfo.collidedEntityId);

        auto layer =
            registry->get<Collideable>(m_mouseHitInfo.collidedEntityId).collisionLayer;
        changeCursors(layer);
    }

    void Cursor::resetHitInfo(CollisionInfo& hitInfo)
    {
        hitInfo.rlCollision = {};
        hitInfo.rlCollision.distance = FLT_MAX;
        hitInfo.rlCollision.hit = false;
    }

    // Find the terrain's mesh collision (instead of using its bounding box)
    void Cursor::findMeshCollision(CollisionInfo& hitInfo)
    {
        if (hitInfo.collisionLayer == CollisionLayer::FLOOR &&
            registry->any_of<Renderable>(hitInfo.collidedEntityId))
        {
            auto& renderable = registry->get<Renderable>(hitInfo.collidedEntityId);
            auto meshCollision = GetRayCollisionMesh(
                ray, *renderable.model.meshes, renderable.model.transform);
            if (meshCollision.hit)
            {
                hitInfo.rlCollision = meshCollision;
            }
        }
    }

    void Cursor::OnControlledActorChange(entt::entity entity)
    {
        controlledActor = entity;
    }

    const CollisionInfo& Cursor::getMouseHitInfo() const
    {
        return m_mouseHitInfo;
    }

    const RayCollision& Cursor::terrainCollision() const
    {
        return m_terrainHitInfo.rlCollision;
    }

    const RayCollision& Cursor::collision() const
    {
        return m_mouseHitInfo.rlCollision;
    }

    void Cursor::Update()
    {
        position = {.x = GetMousePosition().x, .y = GetMousePosition().y};
        getMouseRayCollision();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            onMouseClick();
        }
    }

    void Cursor::Draw3D()
    {
        if (!m_mouseHitInfo.rlCollision.hit) return;
        if (contextLocked) return;
        DrawCube(m_mouseHitInfo.rlCollision.point, 0.5f, 0.5f, 0.5f, currentColor);
        Vector3 normalEnd;
        normalEnd.x =
            m_mouseHitInfo.rlCollision.point.x + m_mouseHitInfo.rlCollision.normal.x;
        normalEnd.y =
            m_mouseHitInfo.rlCollision.point.y + m_mouseHitInfo.rlCollision.normal.y;
        normalEnd.z =
            m_mouseHitInfo.rlCollision.point.z + m_mouseHitInfo.rlCollision.normal.z;

        DrawLine3D(m_mouseHitInfo.rlCollision.point, normalEnd, RED);
    }

    void Cursor::Draw2D()
    {
        if (hideCursor) return;
        Vector2 pos = position;
        if (currentTex != &regulartex)
        {
            pos = Vector2Subtract(
                position,
                {static_cast<float>(currentTex->width / 2),
                 static_cast<float>(currentTex->height / 2)});
        }
        DrawTextureEx(*currentTex, pos, 0.0, 1.0f, WHITE);
    }

    Cursor::Cursor(entt::registry* _registry, GameData* _gameData)
        : registry(_registry), gameData(_gameData)
    {
        regulartex = LoadTexture("resources/textures/cursor/32/regular.png");
        talktex = LoadTexture("resources/textures/cursor/32/talk.png");
        movetex = LoadTexture("resources/textures/cursor/32/move.png");
        invalidmovetex = LoadTexture("resources/textures/cursor/32/denied.png");
        combattex = LoadTexture("resources/textures/cursor/32/attack.png");
        currentTex = &regulartex;
        EnableContextSwitching();
    }
} // namespace sage
