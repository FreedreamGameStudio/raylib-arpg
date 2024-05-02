//
// Created by Steve Wheeler on 18/02/2024.
//

#include "UserInput.hpp"
#include <iostream>

#define FLT_MAX     340282346638528859811704183484516925440.0f     // Maximum value of a float, from bit pattern 01111111011111111111111111111111

#include "GameManager.hpp"

namespace sage
{

void UserInput::GetMouseRayCollision()
{
    auto collisionSystem = ECS->collisionSystem.get();
    auto renderSystem = ECS->renderSystem.get();
    auto sCamera = GM.sCamera.get();
    // Display information about closest hit
    collision = {};
    hitObjectName = "None";
    collision.distance = FLT_MAX;
    collision.hit = false;

    // Get ray and test against objects
    ray = GetMouseRay(GetMousePosition(), *sCamera->getCamera());

    auto collisions = collisionSystem->GetCollisionsWithRay(ray);
    rayCollisionResultInfo = collisions.empty() ? (CollisionInfo){{}, {}} : collisionSystem
        ->GetCollisionsWithRay(ray).at(0);


    if ((rayCollisionResultInfo.rayCollision.hit) && (rayCollisionResultInfo.rayCollision.distance < collision.distance))
    {
        collision = rayCollisionResultInfo.rayCollision;
        if (registry->valid(rayCollisionResultInfo.collidedEntityId))
        {
            if (registry->all_of<Renderable>(rayCollisionResultInfo.collidedEntityId))
            {
                hitObjectName = registry->get<Renderable>(rayCollisionResultInfo.collidedEntityId).name;
            }
        }
        OnCollisionHitEvent->InvokeAllCallbacks();
        if (dOnCollisionHitEvent) dOnCollisionHitEvent();
    }
}

void UserInput::OnClick() const
{
    OnClickEvent->InvokeAllCallbacks();
    if (dOnClickEvent) dOnClickEvent();
    //std::cout << "Hit object position: " << collision.point.x << ", " << collision.point.y << ", " << collision.point.z << "\n";
}

void UserInput::OnDeleteKeyPressed() const
{
    OnDeleteKeyPressedEvent->InvokeAllCallbacks();
    if (dOnDeleteKeyPressedEvent) dOnDeleteKeyPressedEvent();
}

void UserInput::OnCreateKeyPressed() const
{
    OnCreateKeyPressedEvent->InvokeAllCallbacks();
    if(dOnCreateKeyPressedEvent) dOnCreateKeyPressedEvent();
}

void UserInput::OnGenGridKeyPressed() const
{
    OnGenGridKeyPressedEvent->InvokeAllCallbacks();
    if (dOnGenGridKeyPressedEvent) dOnGenGridKeyPressedEvent();
}

void UserInput::OnSerializeKeyPressed() const
{
    OnSerializeKeyPressedEvent->InvokeAllCallbacks();
    if (dOnSerializeKeyPressedEvent) dOnSerializeKeyPressedEvent();
}

void UserInput::Draw()
{
    
    if (collision.hit)
    {
        DrawCube(collision.point, 0.3f, 0.3f, 0.3f, hoverColor);
        DrawCubeWires(collision.point, 0.3f, 0.3f, 0.3f, RED);

        Vector3 normalEnd;
        normalEnd.x = collision.point.x + collision.normal.x;
        normalEnd.y = collision.point.y + collision.normal.y;
        normalEnd.z = collision.point.z + collision.normal.z;

        DrawLine3D(collision.point, normalEnd, RED);
    }
    else
    {
        DrawCube(collision.point, 0.3f, 0.3f, 0.3f, defaultColor);
        DrawCubeWires(collision.point, 0.3f, 0.3f, 0.3f, RED);
    }


    // Draw the mesh bbox if we hit it
    if (rayCollisionResultInfo.rayCollision.hit && registry->valid(rayCollisionResultInfo.collidedEntityId))
    {
        auto collisionSystem = ECS->collisionSystem.get();
        const auto& col = registry->get<Collideable>(rayCollisionResultInfo.collidedEntityId);
        if (col.collisionLayer == FLOOR)
        {
            collisionSystem->BoundingBoxDraw(rayCollisionResultInfo.collidedEntityId, ORANGE);
        }
        else
        {
            collisionSystem->BoundingBoxDraw(rayCollisionResultInfo.collidedEntityId);
        }

    }
}

void UserInput::DrawDebugText() const
{
    // Draw some debug GUI text
    DrawText(TextFormat("Hit Object: %s", hitObjectName.c_str()), 10, 50, 10, BLACK);

    if (collision.hit)
    {
        int ypos = 70;

        DrawText(TextFormat("Distance: %3.2f", collision.distance), 10, ypos, 10, BLACK);

        DrawText(TextFormat("Hit Pos: %3.2f %3.2f %3.2f",
                            collision.point.x,
                            collision.point.y,
                            collision.point.z), 10, ypos + 15, 10, BLACK);

        DrawText(TextFormat("Hit Norm: %3.2f %3.2f %3.2f",
                            collision.normal.x,
                            collision.normal.y,
                            collision.normal.z), 10, ypos + 30, 10, BLACK);

        DrawText(TextFormat("Entity ID: %d", rayCollisionResultInfo.collidedEntityId), 10, 
                 ypos + 45, 10, BLACK);


        
    }
}

void UserInput::ListenForInput()
{
    GetMouseRayCollision();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        OnClick();
    }

    if (IsKeyPressed(KEY_DELETE))
    {
        OnDeleteKeyPressed();
    }
    else if (IsKeyPressed(KEY_P))
    {
        OnCreateKeyPressed();
    } 
    else if (IsKeyPressed(KEY_G))
    {
        OnGenGridKeyPressed();
    }
    else if (IsKeyPressed(KEY_Z))
    {
        OnSerializeKeyPressed();
    }
    else if (IsKeyPressed(KEY_R))
    {
        OnRunModePressedEvent->InvokeAllCallbacks();
        if (dOnRunModePressedEvent) dOnRunModePressedEvent();
    }
}
    
UserInput::UserInput(entt::registry *_registry)
    :
    registry(_registry),
    OnClickEvent(std::make_unique<Event>()), OnCollisionHitEvent(std::make_unique<Event>()),
    OnDeleteKeyPressedEvent(std::make_unique<Event>()), OnCreateKeyPressedEvent(std::make_unique<Event>()),
    OnGenGridKeyPressedEvent(std::make_unique<Event>()), OnSerializeKeyPressedEvent(std::make_unique<Event>()),
    OnRunModePressedEvent(std::make_unique<Event>())
{
}

}
