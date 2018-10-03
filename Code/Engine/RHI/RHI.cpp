#include "Engine/RHI/RHI.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/RHIOutput.hpp"

RHI::RHI()
{
	m_rhiDevice = nullptr;
	m_rhiOutput = nullptr;
}

RHI::~RHI()
{
	delete m_rhiOutput;
	m_rhiOutput = nullptr;

	delete m_rhiDevice;
	m_rhiDevice = nullptr;
}

void RHI::RHIStartup()
{
}

void RHI::RHIShutdown()
{
	m_rhiOutput->Close();
}