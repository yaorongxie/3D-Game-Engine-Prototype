#pragma once
#include "engineMath.h"

namespace Lights
{
	enum
	{
		MMAX_POINT_LIGHTS = 8
	};

	// Size = 48 bytes
	struct PointLightData
	{
		Vector3 lightColor; // 12 bytes at 4-byte alignment
		char lcPadding[4];
		Vector3 position;
		float innerRadius;
		float outerRadius;
		bool isEnabled;
		char padding[11];
	};

	// Need 16-byte alignment
	struct LightingConstants
	{
		Vector3 c_ambient;
		char padding[4];
		PointLightData c_pointLight[8];
	};
}

