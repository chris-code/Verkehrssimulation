#pragma once

#include <random>
#include <chrono>
#include "Road.hpp"
#include "Vehicle.hpp"
#include "Visualization.hpp"
#include <iostream>

using namespace std;

class Simulation {
	public:
		Simulation(double minDallyFactor, double maxDallyFactor, double lambdaRiskFactor, double maxSpeedMean, double maxSpeedStd)
		: road(0,0),
		  randomEngine(chrono::system_clock::now().time_since_epoch().count()),
		  dallyFactorDistribution(minDallyFactor, maxDallyFactor),
		  riskFactorDistribution(lambdaRiskFactor),
		  maxSpeedDistribution(maxSpeedMean, maxSpeedStd),
		  uniform01distribution(0., 1.) {
		}

		void initialize(long streetLength, long laneCount, double carDensity) {
			this->carDensity = carDensity;
			road.resize(streetLength, laneCount);

			uniform_real_distribution<double> carDistribution(0,1);

			for (auto s = 0; s < streetLength; ++s) {
				for (auto l = 0; l < laneCount; ++l) {
					if (carDistribution(randomEngine) < carDensity) {
						Vehicle* v = new Vehicle(randomEngine, dallyFactorDistribution, riskFactorDistribution, maxSpeedDistribution);
						road.insertVehicle(s, l, v);
					}
				}
			}
		}

		void simulate(long runs) {
			Visualization vis(road.getStreetLength());

			vis.appendRoad(road);

			for (long i = 0; i < runs; ++i) {
				update();
				vis.appendRoad(road);
			}

			vis.show();
		}

		void update() {
			addCars();
			
			// Accelerate vehicles
			accelerate();

			// Change lane
			changeLanes();

			// Check distances / slow down
			checkDistances();

			// Dally
			dally();

			// Car Motion
			move();
		}
		
		void addCars() {
			for (auto l = 0; l < road.getLaneCount(); ++l) {
				if (road.getVehicle(0, l) == nullptr && uniform01distribution(randomEngine) < carDensity) {
					Vehicle* v = new Vehicle(randomEngine, dallyFactorDistribution, riskFactorDistribution, maxSpeedDistribution);
					road.insertVehicle(0, l, v);
				}
			}
		}

		void accelerate() {
			for (auto s = 0; s < road.getStreetLength(); ++s) {
				for (auto l = 0; l < road.getLaneCount(); ++l) {
					Vehicle* v = road.getVehicle(s, l);
					if (v != nullptr)
						v->accelerate();
				}
			}
		}

		void changeLanes() {

		}

		void checkDistances() {
			for (auto s = 0; s < road.getStreetLength(); ++s) {
				for (auto l = 0; l < road.getLaneCount(); ++l) {
					Vehicle* v = road.getVehicle(s, l);
					if (v != nullptr) {
						for (int offset = 1; offset <= v->currentSpeed; ++offset) {
							Vehicle* otherV = road.getVehicle(s + offset, l);
							if (otherV != nullptr) {
								v->currentSpeed = offset - 1;
								break;
							}
						}
					}
				}
			}
		}

		void dally() {
			for (auto s = 0; s < road.getStreetLength(); ++s) {
				for (auto l = 0; l < road.getLaneCount(); ++l) {
					Vehicle* v = road.getVehicle(s, l);
					if (v != nullptr) {
						if (uniform01distribution(randomEngine) < v->dallyFactor) {
							v->accelerate(-1);
						}
					}
				}
			}
		}

		void move() {
			for (auto s = road.getStreetLength() - 1; s >= 0; --s) {
				for (auto l = road.getLaneCount() - 1; l >= 0; --l) {
					Vehicle* v = road.getVehicle(s, l);
					if (v != nullptr && v->currentSpeed > 0) {
						road.moveVehicle(s, l, s + v->currentSpeed, l);
					}
				}
			}
		}

	private:
		Road road;
		default_random_engine randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistribution;
		normal_distribution<double> maxSpeedDistribution;
		uniform_real_distribution<double> uniform01distribution;
		double carDensity;
};
