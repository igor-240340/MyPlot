#pragma once

#define DLL_EXPORT extern "C" __declspec(dllexport)

struct KeywordValue {
	const char* keyword;
	const char* value;
};

DLL_EXPORT bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize);
DLL_EXPORT void show(const bool block = true);