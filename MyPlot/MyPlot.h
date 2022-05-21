#pragma once

#include <map>

#define DLL_EXPORT extern "C" __declspec(dllexport)

struct KeywordValue {
	const char* keyword;
	const char* value;
};

DLL_EXPORT bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize);
DLL_EXPORT bool scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s = 1.0f);
DLL_EXPORT void clf();

std::vector<double> floatArrayToVector(const float* arr, int size);
std::map<std::string, std::string> kwToKwMap(const KeywordValue* kw, int kwSize);
void uiLoop();
