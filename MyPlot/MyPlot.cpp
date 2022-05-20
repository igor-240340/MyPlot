#include "pch.h"
#include "MyPlot.h"
#include "matplotlibcpp.h"
#include <thread>
#include "processthreadsapi.h"
#include <mutex>
#include <queue>

namespace plt = matplotlibcpp;

std::thread t;

std::vector<double> x = { 0.0f };
std::vector<double> y = { 0.0f };

std::function<void()> callMeLater;
std::queue<std::function<void()>> cachedFuncs;

DWORD dwThreadId;
HANDLE hThread;

std::mutex cachedFuncsMutex;

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

	//std::lock_guard<std::mutex> guard(cacheMutex);
	//if (cachedFuncsMutex.try_lock()) {
		auto cachedCall = [xVector, yVector, kwMap]() {
			std::cout << "plt::plot()\n";
			plt::plot(xVector, yVector, kwMap);
		};
		cachedFuncs.push(cachedCall);
		//cachedFuncsMutex.unlock();
	//}

	return true;
}

void show(const bool block) {
	plt::show(block);
}

void clf() {
	cachedFuncsMutex.lock();
	
	std::queue<std::function<void()>> empty;
	std::swap(cachedFuncs, empty);

	cachedFuncs.push([]() {
		std::cout << "plt::clf()\n";
		plt::clf();
	});

	cachedFuncsMutex.unlock();
}

bool scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s) {
	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	std::cout << "scatter()\n";
	//std::lock_guard<std::mutex> guard(cachedFuncsMutex);
	//if (cachedFuncsMutex.try_lock()) {
		auto cachedCall = [xVector, yVector, s, kwMap]() {
			std::cout << "plt::scatter()\n";
			plt::scatter(xVector, yVector, s, kwMap);
		};
		cachedFuncs.push(cachedCall);
		//cachedFuncsMutex.unlock();
	//}

	return true;
	//plt::scatter(xVector, yVector, s, kwMap);
}

void pause(float interval) {
	plt::pause(interval);
}

void callCached() {
	while (1) {
		//std::lock_guard<std::mutex> guard(cachedFuncsMutex);
		//if (cachedFuncsMutex.try_lock()) {
		if (!cachedFuncs.empty()) {
			if (cachedFuncsMutex.try_lock()) {
				while (!cachedFuncs.empty()) {
					auto f = cachedFuncs.front();
					f();
					cachedFuncs.pop();
				};
				cachedFuncsMutex.unlock();
			}
		}
			//cachedFuncsMutex.unlock();
		//}

		plt::pause(0.01);
	}
}

void runInThread() {
	t = std::thread(callCached);
}
