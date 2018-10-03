#pragma once
#include "Engine/RHI/RHIInstance.hpp"

class RHI
{
public:
	RHIDevice *m_rhiDevice;
	RHIOutput *m_rhiOutput;

	RHI();
	~RHI();

	void RHIStartup();
	void RHIShutdown();
};
