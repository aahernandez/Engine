#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Matrix4.hpp"

class ParticleEmitter;

class Particle
{
public:
	Vector3 m_position;
	Vector3 m_velocity;
	RGBA m_color;
	float m_lifetime;
	ParticleEmitter *m_parentEmitter;

	Particle();
	~Particle();

	void Update(float deltaSeconds, float maxAge);
	void Render(float size);

	bool IsDead();
};