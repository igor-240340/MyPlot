#pragma once

#define DLL_EXPORT extern "C" __declspec(dllexport)

struct KeywordValue {
	const char* keyword;
	const char* value;
};

DLL_EXPORT bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize);
DLL_EXPORT void show(const bool block = true);
DLL_EXPORT bool scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s = 1.0f);
DLL_EXPORT void pause(float interval);
DLL_EXPORT void clf();

DLL_EXPORT void test();
DLL_EXPORT void runInThread();
DLL_EXPORT void update(double a, double b);
