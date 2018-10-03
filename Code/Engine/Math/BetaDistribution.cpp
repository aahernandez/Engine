#include "Engine/Math/BetaDistribution.hpp"
#include "Engine/Math/MathUtilities.hpp"

#include <cmath>

BetaDistribution::BetaDistribution(float controlA, float controlB)
	: m_shapeControlA(controlA)
	, m_shapeControlB(controlB)
{
	CalculateBetaFromGamma();
}

BetaDistribution::~BetaDistribution()
{

}

float BetaDistribution::CalculateProbabilityDensity(float realization)
{
	float realizationA = pow(realization, m_shapeControlA - 1.f);
	float realizationB = pow(1.f - realization, m_shapeControlB - 1.f);
	float probabilityDensity = m_inverseBeta * (realizationA * realizationB);
	return probabilityDensity;
}

float BetaDistribution::CalculateLowerCumulativeDistribution(float realization)
{
	float incompleteBeta = CalculateIncompleteBetaFunction(realization);
	return incompleteBeta * m_inverseBeta;
}

float BetaDistribution::CalculateUpperCumulativeDistribution(float realization)
{
	float incompleteBeta = GetIntegral(&BetaDistribution::CalculateBetaFromIntegral, realization, 1.f, 101);
	return incompleteBeta * m_inverseBeta;
}

float BetaDistribution::CalculateUpperCumulativeDistributionFlippedShapeControls(float realization)
{
	FlipShapeControls();
	float incompleteBeta = GetIntegral(&BetaDistribution::CalculateBetaFromIntegral, realization, 1.f, 101);
	FlipShapeControls();
	return incompleteBeta * m_inverseBeta;
}

void BetaDistribution::SetShapeControls(float newControlA, float newControlB)
{
	m_shapeControlA = newControlA;
	m_shapeControlB = newControlB;
	ClampZeroToOne(m_shapeControlA);
	ClampZeroToOne(m_shapeControlB);
}

void BetaDistribution::FlipShapeControls()
{
	//don't need to recalculate beta because B(a,b) == B(b,a)
	float tempControl = m_shapeControlA;
	m_shapeControlA = m_shapeControlB;
	m_shapeControlB = tempControl;
}

void BetaDistribution::CalculateBetaFromGamma()
{
	float gammaA = tgamma(m_shapeControlA);
	float gammaB = tgamma(m_shapeControlB);
	float gammaAB = tgamma(m_shapeControlA + m_shapeControlB);

	m_beta = (gammaA * gammaB) / gammaAB;
	m_inverseBeta = 1 / m_beta;
}

float BetaDistribution::CalculateBetaFromIntegral(float realization)
{
	return pow(realization, m_shapeControlA - 1.f) * pow(1.f - realization, m_shapeControlB - 1.f);
}

float BetaDistribution::CalculateIncompleteBetaFunction(float realization)
{
	return GetIntegral(&BetaDistribution::CalculateBetaFromIntegral, 0.f, realization, 101);
}

float BetaDistribution::GetIntegral(float(BetaDistribution::*betaFunction)(float realization), float lowInterval, float highInterval, int numIterations)
{
	float step = (highInterval - lowInterval) / numIterations;
	float area = 0.0; 
	for (int i = 0; i < numIterations; i++) 
	{
		float newRealization = lowInterval + (i + 0.5f) * step;
		float beta = (*this.*betaFunction)(newRealization);
		area += beta * step;
	}
	return area;
}