#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteAnimation::SpriteAnimation()
	: m_durationSeconds(0.f)
	, m_elapsedSeconds(0.f)
	, m_startSpriteIndex(0)
	, m_endSpriteIndex(0)
	, m_isFinished(false)
	, m_isPlaying(true)
{
	m_playBackMode = SPRITE_ANIM_MODE_PLAY_TO_END;
}

SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex)
	: m_durationSeconds(durationSeconds)
	, m_elapsedSeconds(0.f)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_isFinished(false)
	, m_isPlaying(true)
{
	m_spriteSheet = spriteSheet;
	m_playBackMode = playbackMode;
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::Update(float deltaSeconds)
{
	if (!m_isFinished && m_elapsedSeconds <= m_durationSeconds)
		m_elapsedSeconds += deltaSeconds;
	if (m_playBackMode == SPRITE_ANIM_MODE_LOOPING && m_elapsedSeconds > m_durationSeconds)
	{
		Reset();
	}
	if (m_playBackMode == SPRITE_ANIM_MODE_PLAY_TO_END && m_elapsedSeconds > m_durationSeconds)
		m_isFinished = true;
}

AABB2D SpriteAnimation::GetCurrentTexCoords() const
{
	float fractionElapsed = GetFractionElapsed();
	int currentIndex = (int) RangeMap1D( fractionElapsed, Vector2(0.f, 1.f), Vector2( (float) m_startSpriteIndex, (float) m_endSpriteIndex ) );
	if (currentIndex > m_endSpriteIndex)
		currentIndex = m_endSpriteIndex;
	AABB2D m_spriteCoords = m_spriteSheet.GetTexCoordsForSpriteIndex(currentIndex);
	return m_spriteSheet.GetTexCoordsForSpriteIndex(currentIndex);
}

Texture* SpriteAnimation::GetTexture() const
{
	return m_spriteSheet.GetSpriteSheetTexture();
}

void SpriteAnimation::Pause()
{
	m_isPlaying = false;
}

void SpriteAnimation::Resume()
{
	m_isPlaying = true;
}

void SpriteAnimation::Reset()
{
	m_elapsedSeconds = 0.f;
	m_isFinished = false;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	return m_durationSeconds - m_elapsedSeconds;
}

float SpriteAnimation::GetFractionElapsed() const
{
	return m_elapsedSeconds / m_durationSeconds;
}

float SpriteAnimation::GetFractionRemaining() const
{
	return 1.f - GetFractionElapsed();
}

void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
	if (m_elapsedSeconds > m_durationSeconds)
		m_elapsedSeconds = m_durationSeconds;
}

void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
	m_elapsedSeconds = m_durationSeconds * fractionElapsed;
	if (m_elapsedSeconds > m_durationSeconds)
		m_elapsedSeconds = m_durationSeconds;
}
