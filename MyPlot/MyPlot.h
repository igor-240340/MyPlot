#pragma once

#include <map>
#include <vector>
#include <atomic>
#include <thread>

#define DLL_EXPORT extern "C" __declspec(dllexport)

struct KeywordValue {
	const char* keyword;
	const char* value;
};

DLL_EXPORT bool Plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize);
DLL_EXPORT void SubPlot(int nRows, int nCols, int index);
DLL_EXPORT bool Scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s = 1.0f);
DLL_EXPORT void Clf();
DLL_EXPORT void Autoscale(const bool enable = true, const char* axis = "both", const bool tight = false);
DLL_EXPORT void Quiver(const float* x, const float* y, const float* u, const float* v, int xyuvSize, const KeywordValue* kw, int kwSize);

DLL_EXPORT void LabelLines();

DLL_EXPORT void Shutdown();

std::vector<double> floatArrayToVector(const float* arr, int size);
std::map<std::string, std::string> kwToKwMap(const KeywordValue* kw, int kwSize);
void uiLoop();
void StartThread();

extern std::atomic<bool> appIsQuitting;
