#pragma once
#include "Engine/Particles/ParticleEmitter.hpp"

#include <string>
#include <vector>

class ParticleEffect
{
public:
	Vector3 m_position;
	Vector3 m_scale;
	std::string m_name;
	bool m_isStopping;
	bool m_isDestroying;
	bool m_isWaitingForDeletion;
	bool m_shouldPrewarm;
	bool m_isPersistant;
	std::vector<ParticleEmitter*> m_emitters;

	ParticleEffect();
	ParticleEffect(ParticleEffect* particleEffectCopy, const Vector3 &location);
	~ParticleEffect();

	void Update(float deltaSeconds);
	void Render();

	void Stop();
	void Destroy();
	void DestroyImmediate();
	void Play();
	void Reset();
	void Prewarm();
	bool IsFinished();

	ParticleEmitter* AddEmitter(std::string emitterName);
};