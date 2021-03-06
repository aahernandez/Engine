#pragma once
//-----------------------------------------------------------------------------------------------
// Noise.hpp
//
// #include "Engine/Math/MathUtilities.hpp"
#include "Engine/Math/Vector2.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Squirrel's Noise utilities (version 3)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [frankly awful] bit-noise & fractal noise sample code functions commonly found on the web,
//	many of which are hugely biased or terribly patterned, e.g. having bits which are on (or off)
//	75% or even 100% of the time.
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following functions are all based on a simple bit-noise function which returns an unsigned
//	integer containing 32 reasonably-well-scrambled bits, based on a given (signed) integer
//	input parameter (position/index) and [optional] seed.  Kind of like looking up a value in an
//	infinitely large [non-existent] table of previously rolled random numbers.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in smoothed/fractal/simplex/Perlin noise
//	functions and out-of-order (or or-demand) procedural content generation (i.e. that mountain
//	village is the same whether you generated it first or last, ahead of time or just now).
//
// The N-dimensional variations simply hash their multidimensional coordinates down to a single
//	32-bit index and then proceed as usual, so while results are not unique they should
//	(hopefully) not seem locally predictable or repetitive.
//
// Note: these functions assume the presence of a few simple math utility functions and classes;
//	class Vector2, Vector3, Vector4								// public x,y,z,w floats, operators
//	float DotProduct( const Vector2& a, const Vector2& b )		// (ax*bx) + (ay*by)
//	float DotProduct( const Vector3& a, const Vector3& b )		// (ax*bx) + (ay*by) + (az*bz)
//	float DotProduct( const Vector4& a, const Vector4& b )		// (ax*bx) + (ay*by) + (az*bz) + (aw*bw)
//	float FastFloor( float )									// faster replacement for floor()
//	float SmoothStep( float inputZeroToOne )					// 3t^2 - 2t^3		(^ is power-of)
//	float SmoothStep5( float inputZeroToOne )					// 6t^5 - 15t^4 + 10t^3
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Raw pseudorandom noise functions (random-access / deterministic).  Basis of all other noise.
//
unsigned int Get1dNoiseUint( int index, unsigned int seed=0 );
unsigned int Get2dNoiseUint( int indexX, int indexY, unsigned int seed=0 );
unsigned int Get3dNoiseUint( int indexX, int indexY, int indexZ, unsigned int seed=0 );
unsigned int Get4dNoiseUint( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [0,1] for convenience.
//
float Get1dNoiseZeroToOne( int index, unsigned int seed=0 );
float Get2dNoiseZeroToOne( int indexX, int indexY, unsigned int seed=0 );
float Get3dNoiseZeroToOne( int indexX, int indexY, int indexZ, unsigned int seed=0 );
float Get4dNoiseZeroToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [-1,1] for convenience.
//
float Get1dNoiseNegOneToOne( int index, unsigned int seed=0 );
float Get2dNoiseNegOneToOne( int indexX, int indexY, unsigned int seed=0 );
float Get3dNoiseNegOneToOne( int indexX, int indexY, int indexZ, unsigned int seed=0 );
float Get4dNoiseNegOneToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );


//-----------------------------------------------------------------------------------------------
// Smooth/fractal pseudorandom noise functions (random-access / deterministic)
//
// These are less "organic" (and more axial) than Perlin's functions, but simpler and faster.
// 
// <numOctaves>			Number of layers of noise added together
// <octavePersistence>	Amplitude multiplier for each subsequent octave (each octave is quieter)
// <octaveScale>		Frequency multiplier for each subsequent octave (each octave is busier)
// <renormalize>		If true, uses nonlinear (SmoothStep) renormalization to within [-1,1]
//
float Compute1dFractalNoise( float position, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute2dFractalNoise( float posX, float posY, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute3dFractalNoise( float posX, float posY, float posZ, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute4dFractalNoise( float posX, float posY, float posZ, float posT, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );


//-----------------------------------------------------------------------------------------------
// Perlin noise functions (random-access / deterministic)
//
// Perlin noise is slightly more expensive, but more organic-looking (less axial) than regular
//	square fractal noise, through the use of blended dot products vs. randomized gradient vectors.
//
// <numOctaves>			Number of layers of noise added together
// <octavePersistence>	Amplitude multiplier for each subsequent octave (each octave is quieter)
// <octaveScale>		Frequency multiplier for each subsequent octave (each octave is busier)
// <renormalize>		If true, uses nonlinear (SmoothStep) renormalization to within [-1,1]
//
float Compute1dPerlinNoise( float position, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute2dPerlinNoise( float posX, float posY, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute3dPerlinNoise( float posX, float posY, float posZ, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute4dPerlinNoise( float posX, float posY, float posZ, float posT, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );


//-----------------------------------------------------------------------------------------------
// Simplex noise functions (random-access / deterministic)
//
// Simplex noise (also by Ken Perlin) is supposedly faster than - and apparently superior to - 
//	Perlin noise, in that it is more organic-looking.  I'm not sure I like the look of it better,
//	though, and examples of cross-sectional 4D simplex noise look worse to me than 4D Perlin.
//
// Simplex noise is based on a regular simplex (2D triangle, 3D tetrahedron, 4-simplex/5-cell)
//	grid, which is a bit more fiddly, so I haven't written my own yet.


//-----------------------------------------------------------------------------------------------
inline unsigned int Get2dNoiseUint( int indexX, int indexY, unsigned int seed )
{
	const int PRIME_NUMBER = 198491317; // Large prime number with non-boring bits
	return Get1dNoiseUint( indexX + (PRIME_NUMBER * indexY), seed );
}


//-----------------------------------------------------------------------------------------------
inline unsigned int Get3dNoiseUint( int indexX, int indexY, int indexZ, unsigned int seed )
{
	const int PRIME1 = 198491317; // Large prime number with non-boring bits
	const int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ), seed );
}


//-----------------------------------------------------------------------------------------------
inline unsigned int Get4dNoiseUint( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
	const int PRIME1 = 198491317; // Large prime number with non-boring bits
	const int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	const int PRIME3 = 357239; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ) + (PRIME3 * indexT), seed );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dNoiseZeroToOne( int index, unsigned int seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get1dNoiseUint( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dNoiseZeroToOne( int indexX, int indexY, unsigned int seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get2dNoiseUint( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dNoiseZeroToOne( int indexX, int indexY, int indexZ, unsigned int seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get3dNoiseUint( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dNoiseZeroToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get4dNoiseUint( indexX, indexY, indexZ, indexT, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dNoiseNegOneToOne( int index, unsigned int seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int) Get1dNoiseUint( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dNoiseNegOneToOne( int indexX, int indexY, unsigned int seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int) Get2dNoiseUint( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dNoiseNegOneToOne( int indexX, int indexY, int indexZ, unsigned int seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int) Get3dNoiseUint( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dNoiseNegOneToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int) Get4dNoiseUint( indexX, indexY, indexZ, indexT, seed ) );
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//
// OLDER STUFF BELOW
//
// Functions below are getting phased out in my engine, and the bit-noise functions they use
//	are crap.  The above functions are faster, more correct, more flexible, take seeds, etc.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
float GetPseudoRandomNoiseValueNegOneToOne1D( int position );
float GetPseudoRandomNoiseValueZeroToOne1D( int position );
float GetPseudoRandomNoiseValueNegOneToOne2D( int positionX, int positionY );
float GetPseudoRandomNoiseValueZeroToOne2D( int positionX, int positionY );
float GetPseudoRandomNoiseValueNegOneToOne3D( int positionX, int positionY, int positionZ );
float GetPseudoRandomNoiseValueZeroToOne3D( int positionX, int positionY, int positionZ );
float GetPseudoNoiseAngleRadians1D( int position );
float GetPseudoNoiseAngleRadians2D( int positionX, int positionY );
Vector2 GetPseudoRandomNoiseUnitVector2D( int positionX, int positionY );
float ComputePerlinNoiseValueAtPosition2D( const Vector2& position, float perlinNoiseGridCellSize, int numOctaves, float baseAmplitude, float persistance );


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueNegOneToOne1D( int position )
{
	const float ONE_OVER_HALF_MAX_POSITIVE_INT = (1.f / 1073741824.f);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatNegOneToOne = 1.f - (ONE_OVER_HALF_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt);
	return pseudoRandomFloatNegOneToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueZeroToOne1D( int position )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueNegOneToOne2D( int positionX, int positionY )
{
	const float ONE_OVER_HALF_MAX_POSITIVE_INT = (1.f / 1073741824.f);
	int position = positionX + (positionY * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatNegOneToOne = 1.f - (ONE_OVER_HALF_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt);
	return pseudoRandomFloatNegOneToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueZeroToOne2D( int positionX, int positionY )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int position = positionX + (positionY * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueNegOneToOne3D( int positionX, int positionY, int positionZ )
{
	const float ONE_OVER_HALF_MAX_POSITIVE_INT = (1.f / 1073741824.f);
	int position = positionX + (positionY * 57) + (positionZ * 3023);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatNegOneToOne = 1.f - (ONE_OVER_HALF_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt);
	return pseudoRandomFloatNegOneToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoRandomNoiseValueZeroToOne3D( int positionX, int positionY, int positionZ )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int seed = positionX + (positionY * 57) + (positionZ * 3023);
	int bits = (seed << 13) ^ seed;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float) pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//---------------------------------------------------------------------------
inline float GetPseudoNoiseAngleRadians1D( int position )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	const float SCALE_FACTOR = ONE_OVER_MAX_POSITIVE_INT * fPI;
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToTwoPi = SCALE_FACTOR * (float) pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToTwoPi;
}


//---------------------------------------------------------------------------
inline float GetPseudoNoiseAngleRadians2D( int positionX, int positionY )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	const float SCALE_FACTOR = ONE_OVER_MAX_POSITIVE_INT * fPI;
	int position = positionX + (positionY * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToTwoPi = SCALE_FACTOR * (float) pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToTwoPi;
}


//---------------------------------------------------------------------------
inline Vector2 GetPseudoRandomNoiseUnitVector2D( int xPosition, int yPosition )
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	const float SCALE_FACTOR = ONE_OVER_MAX_POSITIVE_INT * fPI;
	int position = xPosition + (yPosition * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToTwoPi = SCALE_FACTOR * (float) pseudoRandomPositiveInt;

	// TODO: Rewrite this to use the randomized int to look up Vector2 from a cos/sin table; vectors don't need to be super-precise,
	//	and we certainly don't want to pay full price for cos/sin if this is merely going to be used for, say, Perlin noise gradiants.
	//	Note however that cos/sin are typically pretty fast on GPUs so this can probably stand as-is in shader code.
	return Vector2( (float) cos( pseudoRandomFloatZeroToTwoPi ), (float) sin( pseudoRandomFloatZeroToTwoPi ) );
}