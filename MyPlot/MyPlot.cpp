#include "pch.h"

#include <queue>
#include <mutex>

#include "MyPlot.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

std::queue<std::function<void()>> plotFuncsQueue;	// Accumulates plot-functions that must be called from uiThread
std::queue<std::function<void()>> wasteFuncs;
std::mutex plotFuncsQueueMutex;

// We need a separate thread so that we can interact with matplotlib UI even when the main thread is paused from Unity
std::thread uiThread;

const double* pXArray;
std::atomic<bool> appIsQuitting = false;

void LabelLines() {
	OutputDebugString(L"[plot debug] MyPlot.cpp|LabelLines()\n");
	StartThread();

	plotFuncsQueueMutex.lock();

	auto f = []() {
		//std::cout << "labelLines()\n";
		//std::cout << "pXArray [" << *pXArray << ", " << *(pXArray + 1) << "]" << std::endl;
		plt::labelLines();
		//std::cout << "after ll pXArray [" << *pXArray << ", " << *(pXArray + 1) << "]" << std::endl;
		//std::cout << "wasteFuncs.size()" << wasteFuncs.size() << std::endl;
	};
	plotFuncsQueue.push(f);

	plotFuncsQueueMutex.unlock();
}

void Quiver(const float* x, const float* y, const float* u, const float* v, int xyuvSize, const KeywordValue* kw, int kwSize) {
	OutputDebugString(L"[plot debug] MyPlot.cpp|Quiver()\n");
	StartThread();

	std::vector<double> xVector = floatArrayToVector(x, xyuvSize);
	std::vector<double> yVector = floatArrayToVector(y, xyuvSize);
	std::vector<double> uVector = floatArrayToVector(u, xyuvSize);
	std::vector<double> vVector = floatArrayToVector(v, xyuvSize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	plotFuncsQueueMutex.lock();

	plotFuncsQueue.push([xVector, yVector, uVector, vVector, kwMap]() {
		plt::quiver(xVector, yVector, uVector, vVector, kwMap);
		});

	plotFuncsQueueMutex.unlock();
}

void SubPlot(int nRows, int nCols, int index) {
	OutputDebugString(L"[plot debug] MyPlot.cpp|SubPlot()\n");
	StartThread();

	plotFuncsQueueMutex.lock();

	plotFuncsQueue.push([nRows, nCols, index]() {
		plt::subplot(nRows, nCols, index);
		});

	plotFuncsQueueMutex.unlock();
}

void Autoscale(const bool enable, const char* axis, const bool tight) {
	OutputDebugString(L"[plot debug] MyPlot.cpp|autoscale()\n");
	StartThread();
	//std::cout << "enable: " << enable << std::endl;
	//std::cout << "axis: " << axis << std::endl;
	//std::cout << "tight: " << tight << std::endl;

	plotFuncsQueueMutex.lock();

	std::string axisString = axis;
	auto f = [enable, axisString, tight]() {
		std::cout << "plt::autoscale()\n";
		plt::autoscale(enable, axisString, tight);
	};
	plotFuncsQueue.push(f);

	plotFuncsQueueMutex.unlock();
}

bool Plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize) {
	OutputDebugString(L"[plot debug] MyPlot.cpp|Plot()\n");
	StartThread();

	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	plotFuncsQueueMutex.lock();

	auto f = [xVector, yVector, kwMap]() {
		std::cout << "plot()\n";
		pXArray = xVector.data();
		plt::plot(xVector, yVector, kwMap);
	};
	plotFuncsQueue.push(f);

	plotFuncsQueueMutex.unlock();

	return true;
}

void Clf() {
	OutputDebugString(L"[plot debug] MyPlot.cpp|clf()\n");
	StartThread();

	plotFuncsQueueMutex.lock();

	// If UI-loop doesn't handle some plot-functions by the next drawing iteration, 
	// we just discard them to avoid buffer growing and, hence, wating for execution all old functions
	// when main thread is paused from Unity - when we pause a game from Unity we need to see only last drawing.
	std::queue<std::function<void()>> empty;
	std::swap(plotFuncsQueue, empty);

	plotFuncsQueue.push([]() {
		std::cout << "plt::clf()\n";
		plt::clf();

		std::cout << "wipe waste\n";
		std::queue<std::function<void()>> empty;
		std::swap(wasteFuncs, empty);
		});

	plotFuncsQueueMutex.unlock();
}

bool Scatter(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize, const float s) {
	OutputDebugString(L"[plot debug] MyPlot.cpp|scatter()\n");
	StartThread();

	std::vector<double> xVector = floatArrayToVector(x, xySize);
	std::vector<double> yVector = floatArrayToVector(y, xySize);

	std::map<std::string, std::string> kwMap = kwToKwMap(kw, kwSize);

	plotFuncsQueueMutex.lock();

	auto f = [xVector, yVector, s, kwMap]() {
		std::cout << "plt::scatter()\n";
		plt::scatter(xVector, yVector, s, kwMap);
	};
	plotFuncsQueue.push(f);

	plotFuncsQueueMutex.unlock();

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
	std::cout << "thread id: " << uiThread.get_id() << std::endl;

	OutputDebugString(L"[thread debug] MyPlot.cpp|uiLoop()\n");

	while (!appIsQuitting) {
		if (plotFuncsQueueMutex.try_lock()) {
			if (!plotFuncsQueue.empty()) {
				while (!plotFuncsQueue.empty()) {
					auto& func = plotFuncsQueue.front();
					func();
					wasteFuncs.push(std::move(func)); // To prolong lifetime of lambda's local variables we move it to another queue
					plotFuncsQueue.pop();
				}
			}
			plotFuncsQueueMutex.unlock();
		}

		plt::pause(0.1); // Run the GUI event loop for some time
	}
	appIsQuitting = false;

	OutputDebugString(L"[thread debug] MyPlot.cpp:144\n");
	plt::detail::_interpreter::kill();

	OutputDebugString(L"[thread debug] MyPlot.cpp|uiLoop()|return\n");
}

void StartThread() {
	OutputDebugString(L"[thread debug] MyPlot.cpp|StartThread()\n");
	if (!uiThread.joinable()) {
		uiThread = std::thread(uiLoop);
		OutputDebugString(L"[thread debug] MyPlot.cpp|StartThread()|std::thread(uiLoop)\n");
	}
}

// Don't call Shutdown if running from Unity Editor.
// It leads repeated Python initialization and modules import each time we enter Play Mode
// but Unity Editor will crash because of an issue with an import of numpy after repeated Py_Initialize call (https://github.com/numpy/numpy/issues/8097)
void Shutdown() {
	OutputDebugString(L"[thread debug] MyPlot.cpp|Shutdown()\n");
	appIsQuitting = true;
	OutputDebugString(L"[thread debug] MyPlot.cpp|Shutdown()|uiThread.join()\n");
	uiThread.join();
	OutputDebugString(L"[thread debug] MyPlot.cpp|Shutdown()|return\n");
}
