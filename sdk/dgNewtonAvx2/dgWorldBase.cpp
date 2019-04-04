/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "dgNewtonPluginStdafx.h"
#include "dgWorldBase.h"


// This is an example of an exported function.
dgWorldPlugin* GetPlugin(dgWorld* const world, dgMemoryAllocator* const allocator)
{
#ifdef _WIN32
	union cpuInfo
	{
		int m_data[4];
		struct
		{
			int m_eax;
			int m_ebx;
			int m_ecx;
			int m_edx;
		};
	} info;

	__cpuid(info.m_data, 0);
	if (info.m_eax < 7) {
		return NULL;
	}

	// check for instruction set support (avx2 is bit 5 in reg ebx)
	__cpuid(info.m_data, 7);
	if (!(info.m_ebx & (1 << 5))) {
		return NULL;
	}

	static dgWorldBase module(world, allocator);
	union {
		char m_vendor[256];
		int m_reg[3];
	};
	memset(m_vendor, 0, sizeof(m_vendor));
	__cpuid(info.m_data, 0);

	m_reg[0] = info.m_ebx;
	m_reg[1] = info.m_edx;
	m_reg[2] = info.m_ecx;
	module.m_score = _stricmp(m_vendor, "GenuineIntel") ? 3 : 4;
	return &module;
#elif __linux__
	if(__builtin_cpu_supports("avx2")) {
		static dgWorldBase module(world, allocator);
		module.m_score = 4;
		return &module;
	} else {
		return NULL;
	}
#elif defined (_MACOSX_VER)
	// must macs support avx2 bu for now let use do avx only
	//static dgWorldBase module(world, allocator);
	//module.m_score = 4;
	//return &module;
	return NULL;
#endif
}

dgWorldBase::dgWorldBase(dgWorld* const world, dgMemoryAllocator* const allocator)
	:dgWorldPlugin(world, allocator)
	,dgSolver(world, allocator)
{
}

dgWorldBase::~dgWorldBase()
{
}

const char* dgWorldBase::GetId() const
{
#ifdef _DEBUG
	return "newtonAVX2_d";
#else
	return "newtonAVX2";
#endif
}

dgInt32 dgWorldBase::GetScore() const
{
	return m_score;
}

void dgWorldBase::CalculateJointForces(const dgBodyCluster& cluster, dgBodyInfo* const bodyArray, dgJointInfo* const jointArray, dgFloat32 timestep)
{
	dgSolver::CalculateJointForces(cluster, bodyArray, jointArray, timestep);
}