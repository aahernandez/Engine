#include "Engine/RHI/Skeleton.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/SkeletonInstance.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"

SkeletonInstance::SkeletonInstance()
	: m_skinTransforms(nullptr)
{

}

SkeletonInstance::SkeletonInstance(Skeleton *skeleton)
{
	m_skeleton = skeleton;

	std::vector<Matrix4> matrices;
	matrices.resize(skeleton->m_joints.size());
	for (int matrixIndex = 0; matrixIndex < (int)matrices.size(); matrixIndex++)
	{
		matrices[matrixIndex] = Matrix4::IDENTITY;
	}

	m_skinTransforms = new StructuredBuffer(g_theSimpleRenderer->m_rhiDevice, matrices.data(), sizeof(Matrix4), matrices.size());
}

SkeletonInstance::~SkeletonInstance()
{

}

Matrix4 SkeletonInstance::GetJointGlobalTransform(int joint_idx, const Pose& pose) const
{
	if (joint_idx == -1)
		return Matrix4::IDENTITY;
	Matrix4 currentMatrix = pose.localTransforms[joint_idx];
	Matrix4 parentMatrix = GetJointGlobalTransform(GetJointParent(joint_idx), pose);
	return currentMatrix * parentMatrix;
}

int SkeletonInstance::GetJointParent(unsigned int joint_idx) const
{
	return m_skeleton->GetJointParentIndex(joint_idx);
}

void SkeletonInstance::ApplyMotion(Motion *motion)
{
	float const time = motion->m_currentTime;
	m_lerpTime = (time * motion->m_framerate) - FastFloor(time * motion->m_framerate);
	motion->Evaluate(m_currentPose, m_prevPose, time);

	std::vector<Matrix4> transformMatrices;
	transformMatrices.resize(m_currentPose.localTransforms.size());
	for (int transformIndex = 0; transformIndex < (int)transformMatrices.size(); transformIndex++)
	{
		Matrix4 initialPoseTransform = m_skeleton->m_joints[transformIndex].transform;
		initialPoseTransform = initialPoseTransform.GetInverse();

		Matrix4 currentPoseTransform = GetJointGlobalTransform(transformIndex, m_currentPose);
		Matrix4 previousPoseTransform = GetJointGlobalTransform(transformIndex, m_prevPose);
		currentPoseTransform = LinearlyInterpolate(previousPoseTransform, currentPoseTransform, m_lerpTime);

		transformMatrices[transformIndex] = initialPoseTransform * currentPoseTransform; 
		transformMatrices[transformIndex].Transpose();
	}

	if(m_skinTransforms != nullptr && !transformMatrices.empty())
	{
		delete m_skinTransforms;
		m_skinTransforms = nullptr;
		m_skinTransforms = new StructuredBuffer(g_theSimpleRenderer->m_rhiDevice, &transformMatrices[0], sizeof(Matrix4), transformMatrices.size());

		m_skinTransforms->Update(g_theSimpleRenderer->m_rhiContext, transformMatrices.data());
		g_theSimpleRenderer->SetStructuredBuffer(5, m_skinTransforms);
	}
	else if (!transformMatrices.empty())
	{
		m_skinTransforms = new StructuredBuffer(g_theSimpleRenderer->m_rhiDevice, &transformMatrices[0], sizeof(Matrix4), transformMatrices.size());

		m_skinTransforms->Update(g_theSimpleRenderer->m_rhiContext, transformMatrices.data());
		g_theSimpleRenderer->SetStructuredBuffer(5, m_skinTransforms);
	}
}

void SkeletonInstance::DrawSkeletonInstance(SimpleRenderer* simpleRenderer)
{
	for (int jointCount = 0; jointCount < (int)m_skeleton->m_joints.size(); jointCount++)
	{

		if (m_skeleton->m_joints[jointCount].parentName != "")
		{
			Matrix4 prevJointMatrix = GetJointGlobalTransform(jointCount, m_prevPose);
			Matrix4 jointMatrix = GetJointGlobalTransform(jointCount, m_currentPose);
			Vector3 prevJointPos = prevJointMatrix.m_translation.xyz();
			Vector3 jointPos = jointMatrix.m_translation.xyz();
			
			Vector3 newJointPos = LinearlyInterpolate(prevJointPos, jointPos, m_lerpTime);

			Matrix4 prevParentMatrix = GetJointGlobalTransform(m_skeleton->GetJointParentIndex(jointCount), m_prevPose);
			Matrix4 parentMatrix = GetJointGlobalTransform( m_skeleton->GetJointParentIndex(jointCount), m_currentPose );
			Vector3 prevParentPos = prevParentMatrix.m_translation.xyz();
			Vector3 parentPos = parentMatrix.m_translation.xyz();

			Vector3 newParentPos = LinearlyInterpolate(prevParentPos, parentPos, m_lerpTime);

			simpleRenderer->SetTexture(simpleRenderer->m_whiteTexture);
			simpleRenderer->DrawLine3D(newJointPos, newParentPos);
		}
	}
}
