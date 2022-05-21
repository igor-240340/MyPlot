#include "pch.h"

#include <thread>
#include <queue>
#include <mutex>

#include "MyPlot.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

std::queue<std::function<void()>> plotFuncsQueue;	// Accumulates plot-functions that must be called in uiThread
std::mutex plotFuncsQueueMutex;

// We need a separate thread so that we can interact with matplotlib UI even when the main thread is paused from Unity
std::thread uiThread = std::thread(uiLoop);

bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize) {
	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	auto f = [xVector, yVector, kwMap]() {
		std::cout << "plt::plot()\n";
		plt::plot(xVector, yVector, kwMap);
	};
	plotFuncsQueue.push(f);

	return true;
}

void clf() {
	plotFuncsQueueMutex.lock();

	// If UI-loop don't handle some plot-functions by the next drawing iteration, 
	// we just discard them to avoid buffer growing and, hence, wating for execution all old functions
	// when main thread is paused from Unity - when we pause a game from Unity we need to see only last drawing.
	std::queue<std::function<void()>> empty;
	std::swap(plotFuncsQueue, empty);

	plotFuncsQueue.push([]() {
		std::cout << "plt::clf()\n";
		plt::clf();
		});

	plotFuncsQueueMutex.unlock();
}

bool scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s) {
	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	auto f = [xVector, yVector, s, kwMap]() {
		std::cout << "plt::scatter()\n";
		plt::scatter(xVector, yVector, s, kwMap);
	};
	plotFuncsQueue.push(f);
	
	return true;
}

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

void uiLoop() {
	while (1) {
		// Lock mutex to prevent queue wiping from the main thread and pop() call on an empty queue (that cause segfault)
		if (!plotFuncsQueue.empty() && plotFuncsQueueMutex.try_lock()) {
			while (!plotFuncsQueue.empty()) {
				auto f = plotFuncsQueue.front();
				f();
				plotFuncsQueue.pop();
			}
			plotFuncsQueueMutex.unlock();
		}

		plt::pause(0.01); // Draw
	}
}
