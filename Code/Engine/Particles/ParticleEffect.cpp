#include "Engine/Particles/ParticleEffect.hpp"

ParticleEffect::ParticleEffect()
	: m_position(Vector3::ZERO)
	, m_scale(Vector3::ZERO)
	, m_isStopping(false)
	, m_isDestroying(false)
	, m_isWaitingForDeletion(false)
	, m_isPersistant(true)
{
	m_name = "";
}

ParticleEffect::ParticleEffect(ParticleEffect* particleEffectCopy, const Vector3 &location)
	: m_position(location)
	, m_scale(particleEffectCopy->m_scale)
	, m_isStopping(particleEffectCopy->m_isStopping)
	, m_isDestroying(particleEffectCopy->m_isDestroying)
	, m_isWaitingForDeletion(particleEffectCopy->m_isWaitingForDeletion)
	, m_name(particleEffectCopy->m_name)
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = particleEffectCopy->m_emitters.begin(); emitterIter != particleEffectCopy->m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = (*emitterIter);
		ParticleEmitter *newEmitter = new ParticleEmitter(currentEmitter);
		newEmitter->m_parentEffect = this;
		m_emitters.push_back(newEmitter);
	}
}

ParticleEffect::~ParticleEffect()
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = *emitterIter;
		delete currentEmitter;
		currentEmitter = nullptr;
		emitterIter = m_emitters.erase(emitterIter);
		if (emitterIter == m_emitters.end())
			break;
	}
}

void ParticleEffect::Update(float deltaSeconds)
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = (*emitterIter);
		if (!m_isStopping && currentEmitter->m_lifetime > 0.f)
			currentEmitter->CreateNewParticles();
		currentEmitter->UpdateParticles(deltaSeconds);

		if (m_isDestroying && currentEmitter->m_particles.empty())
		{
			delete currentEmitter;
			currentEmitter = nullptr;
			emitterIter = m_emitters.erase(emitterIter);
			if (emitterIter == m_emitters.end())
				break;
		}
	}

	if (m_emitters.empty())
	{
		m_isWaitingForDeletion = true;
	}
}

void ParticleEffect::Render()
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		(*emitterIter)->Render();
	}
}

void ParticleEffect::Stop()
{
	m_isStopping = true;
}

void ParticleEffect::Destroy()
{
	m_isStopping = true;
	m_isDestroying = true;
}

void ParticleEffect::DestroyImmediate()
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = (*emitterIter);
		if (m_isDestroying && currentEmitter->m_particles.empty())
		{
			delete currentEmitter;
			currentEmitter = nullptr;
			emitterIter = m_emitters.erase(emitterIter);
			if (emitterIter == m_emitters.end())
				break;
		}
	}

	m_isWaitingForDeletion = true;
}

void ParticleEffect::Play()
{
	m_isStopping = false;
	
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = (*emitterIter);
		currentEmitter->m_hasInitialBursted = false;
	}
}

void ParticleEffect::Reset()
{
	m_isStopping = false;
	DestroyImmediate();
	Prewarm();
}

void ParticleEffect::Prewarm()
{
	for (int updateCount = 0; updateCount < 100; updateCount++)
	{
		Update(0.0166f);
	}
}

bool ParticleEffect::IsFinished()
{
	std::vector<ParticleEmitter*>::iterator emitterIter;
	for (emitterIter = m_emitters.begin(); emitterIter != m_emitters.end(); ++emitterIter)
	{
		ParticleEmitter *currentEmitter = (*emitterIter);
		if (!currentEmitter->m_particles.empty())
		{
			return false;
		}
	}
	return true;
}

ParticleEmitter* ParticleEffect::AddEmitter(std::string emitterName)
{
	ParticleEmitter *newEmitter = new ParticleEmitter();
	newEmitter->m_parentEffect = this;
	newEmitter->m_name = emitterName;
	m_emitters.push_back(newEmitter);
	return newEmitter;
}
