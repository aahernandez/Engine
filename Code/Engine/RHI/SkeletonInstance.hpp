#pragma once
#include "Engine/RHI/Pose.hpp"
#include "Engine/RHI/Motion.hpp"

class Skeleton;
class SimpleRenderer;
class StructuredBuffer;

class SkeletonInstance
{
public:
	StructuredBuffer *m_skinTransforms;
	Skeleton *m_skeleton;
	Pose m_currentPose;
	Pose m_prevPose;
	float m_lerpTime;

	SkeletonInstance();
	SkeletonInstance(Skeleton *m_skeleton);
	~SkeletonInstance();

	Matrix4 GetJointGlobalTransform(int joint_idx, const Pose& pose) const;

	int GetJointParent(unsigned int joint_idx) const;

	void ApplyMotion(Motion *motion);

	void DrawSkeletonInstance(SimpleRenderer* simpleRenderer);
};
