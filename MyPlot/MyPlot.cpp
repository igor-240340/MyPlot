#include "pch.h"
#include "MyPlot.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

std::map<std::string, std::string> keywords{ {"color", "C1"}, /*{"scale", "1"},*/ {"scale_units", "xy"}};

void plot(Vector3* particlePos, Vector3* particleVel, int particleCount) {
	std::vector<float> posX;
	std::vector<float> posY;

	std::vector<float> velX;
	std::vector<float> velY;

	for (int i = 0; i < particleCount; i++) {
		posX.push_back(particlePos[i].x);
		posY.push_back(particlePos[i].y);

		velX.push_back(particleVel[i].x);
		velY.push_back(particleVel[i].y);
	}
	plt::scatter(posX, posY);
	plt::quiver(posX, posY, velX, velY, keywords);
	plt::show();
}