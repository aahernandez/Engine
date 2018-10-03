#include "Engine/Particles/Particle.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

Particle::Particle()
{
}

Particle::~Particle()
{

}

void Particle::Update(float deltaSeconds, float maxAge)
{
	m_lifetime -= deltaSeconds;
	m_color.a = (unsigned char)(255 * (m_lifetime / maxAge));
	m_velocity.y += m_parentEmitter->m_gravity;
	m_position += m_velocity * deltaSeconds;
	
	if (m_parentEmitter->m_hasGroundCollision)
	{
		if (m_position.y < 0.f && m_velocity.y < 0.f)
		{
			m_velocity.y = -m_velocity.y * m_parentEmitter->m_elasticity;
		}
	}
}

void Particle::Render(float size)
{
	Matrix4 modelMatrix;

	if (m_parentEmitter->m_stretch == STRETCH_VELOCITY)
		modelMatrix.Scale(m_velocity.Absolute());
	else if (m_parentEmitter->m_stretch == STRETCH_POSITION)
		modelMatrix.Scale(m_position.Absolute());

	modelMatrix.SetTranslation(m_position);
	g_theSimpleRenderer->SetModelMatrix(modelMatrix);

	Vector3 rightDir = g_theSimpleRenderer->m_matrixData.view.m_iBasis.xyz();
	Vector3 upDir = g_theSimpleRenderer->m_matrixData.view.m_jBasis.xyz();

	g_theSimpleRenderer->DrawQuad3D(Vector3(0.f, 0.f, 0.f), rightDir, -size, size, upDir, -size, size, m_color);
	
	modelMatrix = Matrix4::IDENTITY;
	g_theSimpleRenderer->SetModelMatrix(modelMatrix);
}

bool Particle::IsDead()
{
	if (m_lifetime <= 0.f)
		return true;
	return false;
}