#include "Engine/RHI/Motion.hpp"
#include "Engine/Streams/FileBinaryStream.hpp"

float Motion::SetDuration(float time)
{
	m_poses.resize((unsigned int)Ceiling(time * m_framerate) + 1);
	return (float)m_poses.size();
}

void Motion::SetFramerate(float framerate)
{
	m_framerate = framerate;
}

void Motion::Evaluate(Pose &out, Pose& prevPose, float time)
{
	unsigned int poseIndex = (unsigned int)Ceiling(time * m_framerate);
	Pose *newPose = GetPose(poseIndex);
	out = *newPose;

	poseIndex = poseIndex - 1;
	Pose *newPrevPose = GetPose(poseIndex);
	prevPose = *newPrevPose;
}

void Motion::SetName(std::string newName)
{
	m_name = newName;
}

void Motion::AddToCurrentTime(float timeToAdd)
{
	m_currentTime += timeToAdd;
	float maxTime = Ceiling(m_currentTime * m_framerate) + 1;
	if (maxTime > m_poses.size())
	{
		m_currentTime = 0;
	}
}

Pose* Motion::GetPose(unsigned int poseIndex)
{
	if (poseIndex > m_poses.size())
		poseIndex = m_poses.size() - 1;
	return &m_poses[poseIndex];
}

void Motion::SaveMotion(FileBinaryStream& stream, std::string motionName)
{
	std::string filePath = "Data\\FbxSaves\\";
	filePath += motionName;
	stream.OpenForWrite(filePath.c_str());

	stream.WriteBytes(&m_framerate, sizeof(float));

	int nameSize = (int)m_name.size();
	stream.WriteBytes(&nameSize, sizeof(int));
	if (nameSize > 0)
	{
		stream.WriteBytes(m_name.c_str(), m_name.size());
	}

	int numPoses = (int)m_poses.size();
	stream.WriteBytes(&numPoses, sizeof(int));

	for (int poseIndex = 0; poseIndex < numPoses; poseIndex++)
	{
		Pose &pose = m_poses[poseIndex];

		int numTransforms = (int)pose.localTransforms.size();
		stream.WriteBytes(&numTransforms, sizeof(int));

		for (int transformIndex = 0; transformIndex < numTransforms; transformIndex++)
		{
			Matrix4 &transform = pose.localTransforms[transformIndex];

			stream.WriteBytes(&transform.m_iBasis.x, sizeof(float));
			stream.WriteBytes(&transform.m_iBasis.y, sizeof(float));
			stream.WriteBytes(&transform.m_iBasis.z, sizeof(float));
			stream.WriteBytes(&transform.m_iBasis.w, sizeof(float));

			stream.WriteBytes(&transform.m_jBasis.x, sizeof(float));
			stream.WriteBytes(&transform.m_jBasis.y, sizeof(float));
			stream.WriteBytes(&transform.m_jBasis.z, sizeof(float));
			stream.WriteBytes(&transform.m_jBasis.w, sizeof(float));

			stream.WriteBytes(&transform.m_kBasis.x, sizeof(float));
			stream.WriteBytes(&transform.m_kBasis.y, sizeof(float));
			stream.WriteBytes(&transform.m_kBasis.z, sizeof(float));
			stream.WriteBytes(&transform.m_kBasis.w, sizeof(float));

			stream.WriteBytes(&transform.m_translation.x, sizeof(float));
			stream.WriteBytes(&transform.m_translation.y, sizeof(float));
			stream.WriteBytes(&transform.m_translation.z, sizeof(float));
			stream.WriteBytes(&transform.m_translation.w, sizeof(float));
		}
	}

	stream.Close();
}

void Motion::LoadMotion(FileBinaryStream& stream, std::string motionName)
{
	std::string filePath = "Data\\FbxSaves\\";
	filePath += motionName;
	stream.OpenForRead(filePath.c_str());

	stream.ReadBytes(&m_framerate, sizeof(float));

	int nameSize = 0;
	stream.ReadBytes(&nameSize, sizeof(int));
	if (nameSize > 0)
	{
		m_name.resize(nameSize);
		stream.ReadBytes(&m_name[0], nameSize);
	}

	int numPoses = 0;
	stream.ReadBytes(&numPoses, sizeof(int));
	m_poses.resize(numPoses);

	for (int poseIndex = 0; poseIndex < numPoses; poseIndex++)
	{
		Pose &pose = m_poses[poseIndex];

		int numTransforms = 0;
		stream.ReadBytes(&numTransforms, sizeof(int));
		pose.localTransforms.resize(numTransforms);

		for (int transformIndex = 0; transformIndex < numTransforms; transformIndex++)
		{
			Matrix4 &transform = pose.localTransforms[transformIndex];

			stream.ReadBytes(&transform.m_iBasis.x, sizeof(float));
			stream.ReadBytes(&transform.m_iBasis.y, sizeof(float));
			stream.ReadBytes(&transform.m_iBasis.z, sizeof(float));
			stream.ReadBytes(&transform.m_iBasis.w, sizeof(float));

			stream.ReadBytes(&transform.m_jBasis.x, sizeof(float));
			stream.ReadBytes(&transform.m_jBasis.y, sizeof(float));
			stream.ReadBytes(&transform.m_jBasis.z, sizeof(float));
			stream.ReadBytes(&transform.m_jBasis.w, sizeof(float));

			stream.ReadBytes(&transform.m_kBasis.x, sizeof(float));
			stream.ReadBytes(&transform.m_kBasis.y, sizeof(float));
			stream.ReadBytes(&transform.m_kBasis.z, sizeof(float));
			stream.ReadBytes(&transform.m_kBasis.w, sizeof(float));

			stream.ReadBytes(&transform.m_translation.x, sizeof(float));
			stream.ReadBytes(&transform.m_translation.y, sizeof(float));
			stream.ReadBytes(&transform.m_translation.z, sizeof(float));
			stream.ReadBytes(&transform.m_translation.w, sizeof(float));
		}
	}

	stream.Close();
}
