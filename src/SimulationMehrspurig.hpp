#pragma once

#include <unordered_set>
#include <random>
#include <chrono>
#include <iostream>
#include "Road.hpp"
#include "Vehicle.hpp"
#include "Visualization.hpp"

using namespace std;

class SimulationMehrspurig {
	public:
		SimulationMehrspurig(double minDallyFactor, double maxDallyFactor, double lambdaRiskFactor, double maxSpeedMean, double maxSpeedStd)
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

			//vis.show();
			vis.save();
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
			for (auto l = road.getLaneCount() - 1; l >=0; --l) {
				if (road.getVehicle(0, l) == nullptr && road.getDensity() < carDensity) {
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
			bernoulli_distribution laneChoiceDistribution(0.5);
			
			unordered_set<Vehicle*> processed;
			for (auto s = 0; s < road.getStreetLength(); ++s) {
				for (auto l = 0; l < road.getLaneCount(); ++l) {
					Vehicle *v = road.getVehicle(s, l);
					if (v != nullptr) {
						if(processed.count(v) > 0) {
							continue;
						}
						processed.insert(v);

						if(laneChoiceDistribution(randomEngine)) { //Try left lane first
							if(! changeToLeftLane(s, l)) {
								changeToRightLane(s, l);
							}
						} else { //Try right lane first
							if(! changeToRightLane(s, l)) {
								changeToLeftLane(s, l);
							}
						}
					}
				}
			}
		}
		
		bool changeToLeftLane(long s, long l) {
			if(l == 0) {
				return false; // There is no left lane
			}
			if(road.getVehicle(s, l-1) != nullptr) {
				return false; // Left lane not empty
			}
			
			Vehicle *v = road.getVehicle(s, l);
			if(uniform01distribution(randomEngine) < v->riskFactor) {
				road.moveVehicle(s, l, s, l-1); // Switch lanes regardless of distances
				return true;
			}

			long gap = 0; // Space ahead the car has on its current lane
			for(auto offset = 1; offset <= v->currentSpeed; ++offset) {
				if(s + offset >= road.getStreetLength()) {
					break; // Road ended, gap ends here
				}
				if(road.getVehicle(s + offset, l) != nullptr) {
					break; // Vehicle found, gap ends here
				}
				++gap;
			}
			
			long gapLeft = 0; // Space ahead the car would have on the left lane
			for(auto offset = 1; offset <= gap; ++offset) {
				if(s + offset >= road.getStreetLength()) {
					break; // Road ended, gap ends here
				}
				if(road.getVehicle(s + offset, l-1) != nullptr) {
					break; // Vehicle found, gap ends here
				}
				++gapLeft;
			}
			
			if(v->currentSpeed > gap && gapLeft >= gap) { // Desire to change lanes
				long gapBack = 0; // Space backwards the car would have on the left lane
				long vBack = 0; // Speed of the follow-up car on the left lane
				for(auto offset = -1; offset >= -7; --offset) {
					if(s + offset < 0) {
						break; // Road ended, gap ends here
					}
					if(road.getVehicle(s + offset, l-1) != nullptr) {
						vBack = road.getVehicle(s + offset, l-1)->currentSpeed;
						break; // Vehicle found, gap ends here. Remember its speed, though.
					}
					++gapBack;
				}
				
				if(vBack <= gapBack) { // Enough back gap for safe lane switch
					road.moveVehicle(s, l, s, l-1); // Switch lanes
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
		
		bool changeToRightLane(long s, long l) {
			if(l == road.getLaneCount()-1) {
				return false; // There is no right lane
			}
			if(road.getVehicle(s, l+1) != nullptr) {
				return false; // Right lane not empty
			}
			
			Vehicle *v = road.getVehicle(s, l);
			if(uniform01distribution(randomEngine) < v->riskFactor) {
				road.moveVehicle(s, l, s, l+1); // Switch lanes regardless of distances
				return true;
			}
			
			long vMax = v->currentSpeed;
			long vOffset = 1;

			long gap = 0; // Space ahead the car has on its current lane
			for(auto offset = 1; offset <= v->currentSpeed; ++offset) {
				if(s + offset >= road.getStreetLength()) {
					break; // Road ended, gap ends here
				}
				if(road.getVehicle(s + offset, l) != nullptr) {
					break; // Vehicle found, gap ends here
				}
				++gap;
			}

			long gapRight = 0; // Space ahead the car would have on the left lane
			for(auto offset = 1; offset <= gap; ++offset) {
				if(s + offset >= road.getStreetLength()) {
					break; // Road ended, gap ends here
				}
				if(road.getVehicle(s + offset, l+1) != nullptr) {
					break; // Vehicle found, gap ends here
				}
				++gapRight;
			}
			
			if( vMax + vOffset < gap && vMax + vOffset < gapRight) { // Desire to change lanes
				long gapBack = 0; // Space backwards the car would have on the right lane
				long vBack = 0; // Speed of the follow-up car on the right lane
				for(auto offset = -1; offset >= -7; --offset) {
					if(s + offset < 0) {
						break; // Road ended, gap ends here
					}
					if(road.getVehicle(s + offset, l-1) != nullptr) {
						vBack = road.getVehicle(s + offset, l+1)->currentSpeed;
						break; // Vehicle found, gap ends here. Remember its speed, though.
					}
					++gapBack;
				}

				if(vBack <= gapBack) { // Enough back gap for safe lane switch
					road.moveVehicle(s, l, s, l+1); // Switch lanes
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}

		void checkDistances() {
			for (auto s = 0; s < road.getStreetLength(); ++s) { // iterate over all segments
				for (auto l = 0; l < road.getLaneCount(); ++l) { // iterate over all lanes
					Vehicle* v = road.getVehicle(s, l);
					if (v != nullptr) {
						for (long leftLane = l; leftLane >= 0; --leftLane) { // iterate over lanes that are not on the right of the considered vehicle
							for (long offset = 1; offset <= v->currentSpeed; ++offset) {
								if(s + offset < road.getStreetLength()) {
									Vehicle* otherV = road.getVehicle(s + offset, leftLane);
									if (otherV != nullptr) {
										if (v->currentSpeed > offset - 1) {
											v->currentSpeed = offset - 1;
											break;
										}
									}
								}
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
