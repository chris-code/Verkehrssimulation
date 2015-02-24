#pragma once

#include <random>
#include <cmath>

using namespace std;

class Vehicle {
	public:
		double dallyFactor;
		double riskFactor;
		long maxSpeed;
		long currentSpeed;

		Vehicle(default_random_engine& randomEngine,
				uniform_real_distribution<double>& dallyFactorDistribution,
				exponential_distribution<double>& riskFactorDistribution,
				normal_distribution<double>& maxSpeedDistribution) {

			// set dally factor
			this->dallyFactor = dallyFactorDistribution(randomEngine);

			// set risk factor
			this->riskFactor = riskFactorDistribution(randomEngine);
			if (this->riskFactor > 0.8) this->riskFactor = 0.8;

			// set max speed
			this->maxSpeed = round(maxSpeedDistribution(randomEngine));
			if (this->maxSpeed > 7) this->maxSpeed = 7;
			if (this->maxSpeed < 3) this->maxSpeed = 3;

			// set current speed
			uniform_int_distribution<long> currentSpeedDistribution(0, this->maxSpeed);
			this->currentSpeed = currentSpeedDistribution(randomEngine);
		}

		void accelerate(long deltaV = 1) {
			currentSpeed += deltaV;
			if (currentSpeed > maxSpeed)
				currentSpeed = maxSpeed;
			if (currentSpeed < 0)
				currentSpeed = 0;
		}

};


