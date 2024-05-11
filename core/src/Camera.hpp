//
// Created by Steve Wheeler on 12/02/2024.
//

#pragma once
#include "raylib.h"
#include "components/Transform.hpp"

namespace sage
{
struct UserInput;
class Camera
{
    Camera3D rlCamera;
    Vector3 position {};
    Vector3 target {};
    Vector3 rotation {};
    int zoom = 10;
    void handleInput();

    bool forwardKeyDown;
    bool backKeyDown;
    bool leftKeyDown;
    bool rightKeyDown;
    bool rotateLeftKeyDown;
    bool rotateRightKeyDown;
    bool scrollEnabled = true;
    bool lockInput = false;
public:
    Camera3D* getRaylibCam();
    explicit Camera(UserInput* userInput);
    
    void Update();
    
    void ScrollEnable();
    void ScrollDisable();
    void LockInput();
    void UnlockInput();

    void OnForwardKeyPressed();
    void OnLeftKeyPressed();
    void OnRightKeyPressed();
    void OnBackKeyPressed();
    void OnRotateLeftKeyPressed();
    void OnRotateRightKeyPressed();
    
    void OnForwardKeyUp();
    void OnLeftKeyUp();
    void OnRightKeyUp();
    void OnBackKeyUp();
    void OnRotateLeftKeyUp();
    void OnRotateRightKeyUp();
    void CutscenePose(const sage::Transform& location);
};

} // sage
