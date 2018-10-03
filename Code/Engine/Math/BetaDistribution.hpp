#pragma once

class BetaDistribution
{
public:
	BetaDistribution(float controlA, float controlB);
	~BetaDistribution();

	float CalculateProbabilityDensity(float realization);
	float CalculateLowerCumulativeDistribution(float realization);
	float CalculateUpperCumulativeDistribution(float realization);
	float CalculateUpperCumulativeDistributionFlippedShapeControls(float realization);

	void SetShapeControls(float newControlA, float newControlB);
	void FlipShapeControls();

private:
	float m_shapeControlA;
	float m_shapeControlB;
	float m_beta;
	float m_inverseBeta;

	void CalculateBetaFromGamma();
	float CalculateBetaFromIntegral(float realization);
	float CalculateIncompleteBetaFunction(float realization);
	float GetIntegral(float(BetaDistribution::*betaFunction)(float realization), float lowerInterval, float higherInterval, int numIteration);
};