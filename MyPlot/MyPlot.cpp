#include "pch.h"
#include "MyPlot.h"
#include "matplotlibcpp.h"
#include "windows.h"

namespace plt = matplotlibcpp;

bool plot(const float* x, const float* y, int xySize, const KeywordValue* kw, int kwSize) {
	//AllocConsole();
	//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	std::vector<double> xVector;
	xVector.assign(x, x + xySize);

	std::vector<double> yVector;
	yVector.assign(y, y + xySize);

	/*for (int i = 0; i < xySize; i++) 
		std::cout << x[i] << "," << y[i] << "\n";
	}*/

	std::map<std::string, std::string> kwMap;
	for (int i = 0; i < kwSize; i++) {
		kwMap.insert(std::pair<std::string, std::string>(kw[i].keyword, kw[i].value));
		//std::cout << kw[i].keyword << "," << kw[i].value;
	}

	return plt::plot(xVector, yVector, kwMap);
}

void show(const bool block) {
	plt::show(block);
}
