#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Core/FileUtils.hpp"

ParticleSystem::ParticleSystem()
{
	LoadParticleEffectsFromXML();
}

ParticleSystem::~ParticleSystem()
{
	std::map<std::string, ParticleEffect*>::iterator effectsIter;
	for (effectsIter = m_particleEffectCopies.begin(); effectsIter != m_particleEffectCopies.end(); ++effectsIter)
	{
		ParticleEffect *currentParticleEffect = effectsIter->second;
		delete currentParticleEffect;
		currentParticleEffect = nullptr;
		effectsIter = m_particleEffectCopies.erase(effectsIter);
		if (effectsIter == m_particleEffectCopies.end())
			break;
	}

	std::vector<ParticleEffect*>::iterator activeEffectsIter;
	for (activeEffectsIter = m_activeParticleEffects.begin(); activeEffectsIter != m_activeParticleEffects.end(); ++activeEffectsIter)
	{
		ParticleEffect *currentParticleEffect = *activeEffectsIter;
		delete currentParticleEffect;
		currentParticleEffect = nullptr;
		activeEffectsIter = m_activeParticleEffects.erase(activeEffectsIter);
		if (activeEffectsIter == m_activeParticleEffects.end())
			break;
	}
}

void ParticleSystem::Update(float deltaSeconds)
{
	std::vector<ParticleEffect*>::iterator effectIter;
	for (effectIter = m_activeParticleEffects.begin(); effectIter != m_activeParticleEffects.end(); ++effectIter)
	{
		ParticleEffect *currentEffect = (*effectIter);
		currentEffect->Update(deltaSeconds);
		if (currentEffect->m_isWaitingForDeletion)
		{
			delete currentEffect;
			currentEffect = nullptr;
			effectIter = m_activeParticleEffects.erase(effectIter);
			if (effectIter == m_activeParticleEffects.end())
				break;
		}
	}
}

void ParticleSystem::Render()
{
	std::vector<ParticleEffect*>::iterator systemIter;
	for (systemIter = m_activeParticleEffects.begin(); systemIter != m_activeParticleEffects.end(); ++systemIter)
	{
		(*systemIter)->Render();
	}
}

void ParticleSystem::DestroyLastEffect()
{
	int lastIndex = (int)m_activeParticleEffects.size() - 1;
	m_activeParticleEffects[lastIndex]->Destroy();
}

void ParticleSystem::LoadParticleEffectsFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/ParticleEffects.xml", "ParticleEffects");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode effectNode = xMainNode.getChildNode(nodeCount);
		std::string name = ParseXmlAttribute(effectNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
		m_particleEffectCopies[name] = new ParticleEffect();
		ParticleEffect *newParticleEffect = m_particleEffectCopies[name];
		newParticleEffect->m_name = name;
		newParticleEffect->m_shouldPrewarm = ParseXmlAttribute(effectNode, "prewarm", false);

		int numEmitters = effectNode.nChildNode();
		for (int emitterNodeCount = 0; emitterNodeCount < numEmitters; emitterNodeCount++)
		{
			ITCXMLNode emitterNode = effectNode.getChildNode(emitterNodeCount);
			std::string emitterName = ParseXmlAttribute(emitterNode, "name", (std::string)"ERROR_NAME_NOT_FOUND");
			ParticleEmitter *newParticleEmitter = newParticleEffect->AddEmitter(emitterName);
			newParticleEmitter->m_spawnRate =		ParseXmlAttribute(emitterNode.getChildNode("SpawnRate"), "rate", (int)0);
			newParticleEmitter->m_initialBurstNum = ParseXmlAttribute(emitterNode.getChildNode("InitialBurst"), "num", (int)0);
			newParticleEmitter->m_size =			ParseXmlAttribute(emitterNode.getChildNode("Size"), "size", (float)0.f);
			newParticleEmitter->m_particleLifetime =ParseXmlAttribute(emitterNode.getChildNode("Particle"), "lifetime", (float)0.f);
			newParticleEmitter->m_maxAge =			ParseXmlAttribute(emitterNode.getChildNode("MaxAge"), "age", (float)0.f);
			newParticleEmitter->m_lifetime = newParticleEmitter->m_maxAge;
			std::string colorName =					ParseXmlAttribute(emitterNode.getChildNode("Tint"), "color", (std::string)"white");
			newParticleEmitter->m_tint =			RGBA::GetColorFromString(colorName);
			newParticleEmitter->m_gravity =			ParseXmlAttribute(emitterNode.getChildNode("Velocity"), "gravity", (float)0.f);

			ITCXMLNode collisionNode = emitterNode.getChildNode("Collision");
			newParticleEmitter->m_hasGroundCollision = ParseXmlAttribute(collisionNode, "ground", false);
			newParticleEmitter->m_elasticity = ParseXmlAttribute(collisionNode, "elasticity", (float)0.f);

			ITCXMLNode distributionNode = emitterNode.getChildNode("Distribution");
			if (distributionNode.nChildNode("Sphere") > 0)
			{
				ITCXMLNode sphereNode = distributionNode.getChildNode("Sphere");
				newParticleEmitter->m_shape = SHAPE_SPHERE;
				newParticleEmitter->m_spawnRadius = ParseXmlAttribute(sphereNode, "radius", (float)0.f);

				ITCXMLNode directionNode = distributionNode.getChildNode("Direction");
				std::string velocityDirectionString = ParseXmlAttribute(directionNode, "direction", (std::string)"ERROR_NO_DIRECTION_FOUND");
				newParticleEmitter->m_direction = ParticleEmitter::GetVelocityDirectionEnumFromString(velocityDirectionString);
				newParticleEmitter->m_speed = ParseXmlAttribute(directionNode, "speed", (float)0.f);

				ITCXMLNode stretchNode = distributionNode.getChildNode("Stretch");
				std::string stretchString = ParseXmlAttribute(stretchNode, "type", (std::string)"ERROR_STRETCH_NOT_FOUND");
				newParticleEmitter->m_stretch = ParticleEmitter::GetStretchFromString(stretchString);
			}
			else if (distributionNode.nChildNode("Cone") > 0)
			{
				ITCXMLNode coneNode = distributionNode.getChildNode("Cone");
				newParticleEmitter->m_shape = SHAPE_CONE;
				newParticleEmitter->m_maxAngle =		ParseXmlAttribute(coneNode, "maxAngle", (float)0.f);
				newParticleEmitter->m_maxAngle /= 90.f;
				newParticleEmitter->m_spawnHeightMin =	ParseXmlAttribute(coneNode, "heightMin", (float)0.f);
				newParticleEmitter->m_spawnHeightMax =	ParseXmlAttribute(coneNode, "heightMax", (float)0.f);

				ITCXMLNode directionNode = distributionNode.getChildNode("Direction");
				std::string velocityDirectionString = ParseXmlAttribute(directionNode, "direction", (std::string)"ERROR_NO_DIRECTION_FOUND");
				newParticleEmitter->m_direction = ParticleEmitter::GetVelocityDirectionEnumFromString(velocityDirectionString);
				newParticleEmitter->m_speed = ParseXmlAttribute(directionNode, "speed", (float)0.f);
			}
		}

		if (newParticleEffect->m_shouldPrewarm)
		{
			newParticleEffect->Prewarm();
		}
	}
}

ParticleEffect* ParticleSystem::PlayOneOffEffect(std::string effectName, const Vector3 &location)
{
	ParticleEffect *newParticleEffect = CreateEffect(effectName, location);
	
	if (newParticleEffect == nullptr)
		return nullptr;

	newParticleEffect->m_isPersistant = false;
	return newParticleEffect;
}

ParticleEffect* ParticleSystem::CreateEffect(std::string effectName, const Vector3 &location)
{
	if (m_particleEffectCopies.find(effectName) == m_particleEffectCopies.end())
		return nullptr;

	ParticleEffect *newParticleEffect = new ParticleEffect(m_particleEffectCopies[effectName], location);
	m_activeParticleEffects.push_back(newParticleEffect);
	return newParticleEffect;
}
