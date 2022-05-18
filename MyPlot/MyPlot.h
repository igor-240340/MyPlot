#pragma once

struct Vector3
{
	float x;
	float y;
	float z;
};

extern "C" __declspec(dllexport) void plot(Vector3 * particlePos, Vector3 * particleVel, int particleCount);
