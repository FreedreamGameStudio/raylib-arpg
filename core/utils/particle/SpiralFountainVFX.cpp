//
// Created by Steve Wheeler on 26/07/2024.
//

#include "SpiralFountainVFX.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>

namespace sage
{
	void SpiralFountainVFX::Draw3D() const
	{
		fountain->Draw(shader);
	}

	void SpiralFountainVFX::Update(float dt)
	{
		spiralAngle += spiralSpeed * dt;
		spiralRadius += spiralGrowth * dt;
		if (spiralRadius > maxSpiralRadius)
		{
			spiralRadius = minSpiralRadius;
			spiralAngle = 0.0f;
		}
		Vector3 spiralPos = {
				cosf(spiralAngle) * spiralRadius,
				spiralRadius / 2,  // This makes the spiral move upwards as it expands
				sinf(spiralAngle) * spiralRadius
		};
		fountain->SetOrigin(Vector3Add(spiralCentre, spiralPos));
		fountain->Update(dt);
	}

	void SpiralFountainVFX::SetOrigin(const Vector3& origin)
	{
		spiralCentre = origin;
	}

	void SpiralFountainVFX::InitSystem(const Vector3& _target)
	{
		active = true;
		spiralCentre = _target;
	}

	SpiralFountainVFX::~SpiralFountainVFX()
	{
		UnloadShader(shader);
	}

	SpiralFountainVFX::SpiralFountainVFX(Camera3D* _camera) : camera(_camera)
	{
		shader = LoadShader(nullptr, "resources/shader/glsl330/billboard.fs");
		fountain = std::make_unique<FountainEffect>(camera);
		InitSystem({ 30.0f, 4, 20.0f });
	}
}