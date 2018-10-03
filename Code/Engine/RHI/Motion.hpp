#pragma once
#include "Engine/RHI/Pose.hpp"

class FileBinaryStream;
class Skeleton;

class Motion
{
public:
	float m_framerate;
	float m_currentTime;
	std::string m_name;
	std::vector<Pose> m_poses;

	float SetDuration(float time);
	void SetFramerate(float framerate);

	float GetDuration() const { return (m_poses.size() - 1) / m_framerate; }
	unsigned int GetFrameCount() const { return m_poses.size(); }

	void Evaluate(Pose &out, Pose& prevPose, float time);

	void SetName(std::string newName);
	void AddToCurrentTime(float timeToAdd);
	Pose* GetPose(unsigned int poseIndex);

	void SaveMotion(FileBinaryStream& stream, std::string motionName);
	void LoadMotion(FileBinaryStream& stream, std::string motionName);
};
