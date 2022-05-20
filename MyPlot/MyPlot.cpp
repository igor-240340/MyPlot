#include "pch.h"
#include "MyPlot.h"
#include "matplotlibcpp.h"
#include <thread>
#include "processthreadsapi.h"

namespace plt = matplotlibcpp;

std::thread t;

std::vector<double> x = { 0.0f };
std::vector<double> y = { 0.0f };

std::function<void()> callMeLater;

DWORD dwThreadId;
HANDLE hThread;

std::vector<double> floatArrayToVector(const float* arr, int size) {
	std::vector<double> v;
	v.assign(arr, arr + size);
	return v;
}

std::map<std::string, std::string> kwToKwMap(const KeywordValue* kw, int kwSize) {
	std::map<std::string, std::string> kwMap;

	for (int i = 0; i < kwSize; i++)
		kwMap.insert(std::pair<std::string, std::string>(kw[i].keyword, kw[i].value));

	return kwMap;
}

bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize) {
	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	callMeLater = [xVector, yVector, kwMap]() {
		plt::plot(xVector, yVector, kwMap);
	};

	return true;
}

void show(const bool block) {
	plt::show(block);
}

void clf() {
	plt::clf();
}

bool scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s) {
	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	callMeLater = [xVector, yVector, s, kwMap]() {
		plt::scatter(xVector, yVector, s, kwMap);
	};

	//plt::scatter(xVector, yVector, s, kwMap);
}

void pause(float interval) {
	plt::pause(interval);
}

void callCached() {
	std::cout << "callCached\n";
	callMeLater();
	show();
}

int MyHook() {
	std::cout << "MyHook\n";
	return 0;
}

void runInThread() {
	t = std::thread(callCached);
}

void update(double a, double b) {
	x.clear();
	x.push_back(a);

	y.clear();
	y.push_back(b);
}
