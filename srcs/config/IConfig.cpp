#include "config/IConfig.h"

const IConfig::physical_ports_cont& FakeConfig::GetPhysicalPorts() const
{
	return m_PhysicalPorts;
}
