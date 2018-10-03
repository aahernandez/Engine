#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Particles/ParticleEffect.hpp"

#include <algorithm>

ParticleEmitter::ParticleEmitter()
	: m_spawnRate(0)
	, m_size(0.f)
	, m_particleLifetime(0.f)
	, m_lifetime(0.f)
	, m_maxAge(0.f)
	, m_hasInitialBursted(false)
	, m_initialBurstNum(0)
	, m_shape(SHAPE_POINT)
	, m_direction(DIR_RANDOM)
	, m_stretch(STRETCH_NONE)
	, m_spawnRadius(0.f)
	, m_speed(0.f)
	, m_maxAngle(0.f)
	, m_spawnHeightMin(0.f)
	, m_spawnHeightMax(0.f)
	, m_hasGroundCollision(false)
	, m_elasticity(0.f)
	, m_parentEffect(nullptr)
	, m_gravity(0.f)
{
	m_name = "";
	m_tint = RGBA::WHITE;
}

ParticleEmitter::ParticleEmitter(ParticleEmitter *copyEmitter)
	: m_name(copyEmitter->m_name)
	, m_spawnRate(copyEmitter->m_spawnRate)
	, m_size(copyEmitter->m_size)
	, m_particleLifetime(copyEmitter->m_particleLifetime)
	, m_lifetime(copyEmitter->m_lifetime)
	, m_maxAge(copyEmitter->m_maxAge)
	, m_hasInitialBursted(copyEmitter->m_hasInitialBursted)
	, m_initialBurstNum(copyEmitter->m_initialBurstNum)
	, m_shape(copyEmitter->m_shape)
	, m_direction(copyEmitter->m_direction)
	, m_stretch(copyEmitter->m_stretch)
	, m_spawnRadius(copyEmitter->m_spawnRadius)
	, m_speed(copyEmitter->m_speed)
	, m_maxAngle(copyEmitter->m_maxAngle)
	, m_spawnHeightMin(copyEmitter->m_spawnHeightMin)
	, m_spawnHeightMax(copyEmitter->m_spawnHeightMax)
	, m_hasGroundCollision(copyEmitter->m_hasGroundCollision)
	, m_elasticity(copyEmitter->m_elasticity)
	, m_parentEffect(copyEmitter->m_parentEffect)
	, m_tint(copyEmitter->m_tint)
	, m_gravity(copyEmitter->m_gravity)
{
}

ParticleEmitter::~ParticleEmitter()
{
	std::vector<Particle*>::iterator particleIter;
	for (particleIter = m_particles.begin(); particleIter != m_particles.end(); ++particleIter)
	{
		Particle *currentParticle = *particleIter;
		delete currentParticle;
		currentParticle = nullptr;
		particleIter = m_particles.erase(particleIter);
		if (particleIter == m_particles.end())
			break;
	}
}

void ParticleEmitter::CreateNewParticles()
{
	AddParticles(m_spawnRate);
}

void ParticleEmitter::UpdateParticles(float deltaSeconds)
{
	if (!m_parentEffect->m_isPersistant)
	{
		m_lifetime -= deltaSeconds;
	}

	if (!m_hasInitialBursted)
	{
		InitialBurst();
		m_hasInitialBursted = true;
	}

	std::vector<Particle*>::iterator particleIter;
	for (particleIter = m_particles.begin(); particleIter != m_particles.end(); ++particleIter)
	{
		Particle *currentParticle = (*particleIter);
		currentParticle->Update(deltaSeconds, m_particleLifetime);
		if (currentParticle->IsDead())
		{
			delete currentParticle;
			currentParticle = nullptr;
			particleIter = m_particles.erase(particleIter);
			if (particleIter == m_particles.end())
				break;
		}
	}

	SortParticles();
}

bool SortByDistanceAlgorithm(const Particle *particle1, const Particle *particle2)
{
	Vector3 cameraTranslation = g_theSimpleRenderer->m_matrixData.view.GetTranslation();
	return GetDistanceVector3(cameraTranslation, particle1->m_position) < GetDistanceVector3(cameraTranslation, particle2->m_position);
}

void ParticleEmitter::SortParticles()
{
	std::sort(m_particles.begin(), m_particles.end(), SortByDistanceAlgorithm);
}

void ParticleEmitter::Render()
{
	std::vector<Particle*>::iterator particleIter;
	for (particleIter = m_particles.begin(); particleIter != m_particles.end(); ++particleIter)
	{
		Particle *currentParticle = (*particleIter);
		currentParticle->Render(m_size);
	}
}

void ParticleEmitter::InitialBurst()
{
	AddParticles(m_initialBurstNum);
}

void ParticleEmitter::AddParticles(unsigned int numParticles)
{
	for (unsigned int particleCount = 0; particleCount < numParticles; particleCount++)
	{
		Particle *newParticle = new Particle();
		newParticle->m_lifetime = m_particleLifetime;
		newParticle->m_color = m_tint;
		newParticle->m_parentEmitter = this;
		SetSpawnPosition(newParticle);
		SetSpawnVelocity(newParticle);
		m_particles.push_back(newParticle);
	}
}

DistributionDirection_e ParticleEmitter::GetVelocityDirectionEnumFromString(std::string directionString)
{
	if (directionString == "awayFromCenter")
		return DIR_AWAY_CENTER;
	else if (directionString == "towardsCenter")
		return DIR_TOWARDS_CENTER;
	return DIR_RANDOM;
}

DistributionStretch_e ParticleEmitter::GetStretchFromString(std::string stretchString)
{
	if (stretchString == "velocity")
		return STRETCH_VELOCITY;
	else if (stretchString == "position")
		return STRETCH_POSITION;
	return STRETCH_NONE;
}

void ParticleEmitter::SetSpawnPosition(Particle *particle)
{
	if (m_shape == SHAPE_POINT)
	{
		particle->m_position = Vector3(0.f, 0.f, 0.f);
	}
	else if (m_shape == SHAPE_SPHERE)
	{
		particle->m_position.x = GetRandomFloatInRange(-m_spawnRadius, m_spawnRadius);
		particle->m_position.y = GetRandomFloatInRange(-m_spawnRadius, m_spawnRadius);
		particle->m_position.z = GetRandomFloatInRange(-m_spawnRadius, m_spawnRadius);
		if (particle->m_position.CalcDistanceToVector(Vector3::ZERO) > m_spawnRadius)
			SetSpawnPosition(particle);
	}
	else if (m_shape == SHAPE_CONE)
	{
		float theta = GetRandomFloatInRange(0.f, 360.f);
		float height = GetRandomFloatInRange(m_spawnHeightMin, m_spawnHeightMax);
		float radius = m_maxAngle * (height/m_spawnHeightMax);

		particle->m_position.x = cos(fPI / 180 * theta) * radius;
		particle->m_position.y = height;
		particle->m_position.z = sin(fPI / 180 * theta) * radius;
	}
}

void ParticleEmitter::SetSpawnVelocity(Particle *particle)
{
	if (m_direction == DIR_RANDOM)
	{
		particle->m_velocity.x = GetRandomFloatInRange(-m_speed, m_speed);
		particle->m_velocity.y = GetRandomFloatInRange(-m_speed, m_speed);
		particle->m_velocity.z = GetRandomFloatInRange(-m_speed, m_speed);
	}
	else if (m_direction == DIR_AWAY_CENTER)
	{
		Vector3 directionalVector = particle->m_position.GetNormalized();

		particle->m_velocity.x = directionalVector.x * m_speed;
		particle->m_velocity.y = directionalVector.y * m_speed;
		particle->m_velocity.z = directionalVector.z * m_speed;
	}
	else if (m_direction == DIR_TOWARDS_CENTER)
	{
		Vector3 directionalVector = particle->m_position.GetNormalized();

		particle->m_velocity.x = -directionalVector.x * m_speed;
		particle->m_velocity.y = -directionalVector.y * m_speed;
		particle->m_velocity.z = -directionalVector.z * m_speed;
	}
}
