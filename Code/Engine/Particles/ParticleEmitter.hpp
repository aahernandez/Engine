#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Particles/Particle.hpp"

#include <vector>

class ParticleEffect;

enum DistributionShape_e
{
	SHAPE_POINT,
	SHAPE_SPHERE,
	SHAPE_CONE,
	NUM_SHAPES,
};

enum DistributionDirection_e
{
	DIR_RANDOM,
	DIR_TOWARDS_CENTER,
	DIR_AWAY_CENTER,
	NUM_VELOCITY_DIRECTIONS,
};

enum DistributionStretch_e
{
	STRETCH_NONE,
	STRETCH_VELOCITY,
	STRETCH_POSITION,
	NUM_STRETCHES,
};

class ParticleEmitter
{
public:
	std::string m_name;
	unsigned int m_spawnRate;
	unsigned int m_initialBurstNum;
	ParticleEffect *m_parentEffect;
	float m_lifetime;
	float m_size;
	float m_particleLifetime;
	float m_maxAge;
	float m_spawnRadius;
	float m_speed;
	bool m_hasInitialBursted;
	float m_maxAngle;
	float m_spawnHeightMin;
	float m_spawnHeightMax;
	float m_gravity;
	bool m_hasGroundCollision;
	float m_elasticity;
	RGBA m_tint;
	DistributionShape_e m_shape;
	DistributionDirection_e m_direction;
	DistributionStretch_e m_stretch;
	std::vector<Particle*> m_particles;

	ParticleEmitter();
	ParticleEmitter(ParticleEmitter *copyEmitter);
	~ParticleEmitter();

	void CreateNewParticles();
	void UpdateParticles(float deltaSeconds);
	void SortParticles();
	void Render();
	void InitialBurst();

	void AddParticles(unsigned int numParticles);

	static DistributionDirection_e GetVelocityDirectionEnumFromString(std::string directionString);
	static DistributionStretch_e GetStretchFromString(std::string stretchString);

private:
	void SetSpawnPosition(Particle *particle);
	void SetSpawnVelocity(Particle *particle);
};