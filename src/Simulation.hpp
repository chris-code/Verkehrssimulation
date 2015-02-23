#pragma once

#include <random>
#include <chrono>
#include "Road.hpp"
#include "Vehicle.hpp"

using namespace std;

class Simulation {
	public:
		Simulation(double minDallyFactor, double maxDallyFactor, double lambdaRiskFactor, double maxSpeedMean, double maxSpeedStd)
		: road(0,0),
		  randomEngine(chrono::system_clock::now().time_since_epoch().count()),
		  dallyFactorDistribution(minDallyFactor, maxDallyFactor),
		  riskFactorDistribution(lambdaRiskFactor),
		  maxSpeedDistribution(maxSpeedMean, maxSpeedStd) {
		}

		void initialize(long streetLength, long laneCount, double carDensity) {
			road.resize(streetLength, laneCount);

			uniform_real_distribution<double> carDistribution(0,1);

			for (long s = 0; s < road.getStreetLength(); ++s) {
				for (long l = 0; l < laneCount; ++l) {
					if (carDistribution(randomEngine) < carDensity) {
						Vehicle* v = new Vehicle(randomEngine, dallyFactorDistribution, riskFactorDistribution, maxSpeedDistribution);
						road.insertVehicle(s, l, v);
					}
				}
			}
		}

		void update() {

		}

	private:
		Road road;
		default_random_engine randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistribution;
		normal_distribution<double> maxSpeedDistribution;
};
