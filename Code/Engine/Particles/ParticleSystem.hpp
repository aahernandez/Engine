#pragma once
#include "Engine/Particles/ParticleEffect.hpp"

#include <map>

class ParticleSystem
{
public:
	std::map<std::string, ParticleEffect*> m_particleEffectCopies;
	std::vector<ParticleEffect*> m_activeParticleEffects;

	ParticleSystem();
	~ParticleSystem();

	void Update(float deltaSeconds);
	void Render();

	void DestroyLastEffect();
	void LoadParticleEffectsFromXML();
	ParticleEffect* PlayOneOffEffect(std::string effectName, const Vector3 &location);
	ParticleEffect* CreateEffect(std::string effectName, const Vector3 &location);
};