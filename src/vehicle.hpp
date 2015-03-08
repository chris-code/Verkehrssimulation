#pragma once

#include <random>
#include <cmath>

using namespace std;

class Vehicle {
	public:
		double dallyFactor;
		double riskFactorL2R;
		double riskFactorR2L;
		long maxSpeed;
		long currentSpeed;
		
		Vehicle( default_random_engine& randomEngine,
		         uniform_real_distribution<double>& dallyFactorDistribution,
		         exponential_distribution<double>& riskFactorDistributionL2R,
				 exponential_distribution<double>& riskFactorDistributionR2L,
		         normal_distribution<double>& maxSpeedDistribution ) {
		         
			// set dally factor
			this->dallyFactor = dallyFactorDistribution( randomEngine );
			
			// set risk factor Left to Right
			double lambdaL2R = riskFactorDistributionL2R.lambda();
			double maxRiskFactorL2R = (1. / lambdaL2R) * (-1) * log(1 - 0.97); // 0.97 fractile of exp. distribution
			this->riskFactorL2R = riskFactorDistributionL2R( randomEngine );
			if( this->riskFactorL2R > maxRiskFactorL2R ) this->riskFactorL2R = maxRiskFactorL2R;

			// set risk factor Right to Left
			double lambdaR2L = riskFactorDistributionR2L.lambda();
			double maxRiskFactorR2L = (1. / lambdaR2L) * (-1) * log(1 - 0.97); // 0.97 fractile of exp. distribution
			this->riskFactorR2L = riskFactorDistributionR2L( randomEngine );
			if( this->riskFactorR2L > maxRiskFactorR2L ) this->riskFactorR2L = maxRiskFactorR2L;
			
			// set max speed
			this->maxSpeed = round( maxSpeedDistribution( randomEngine ) );
			if( this->maxSpeed > 7 ) this->maxSpeed = 7;
			if( this->maxSpeed < 3 ) this->maxSpeed = 3;
			
			// set current speed
			uniform_int_distribution<long> currentSpeedDistribution( 0, this->maxSpeed );
			this->currentSpeed = currentSpeedDistribution( randomEngine );
		}
		
		void accelerate( long deltaV = 1 ) {
			currentSpeed += deltaV;
			if( currentSpeed > maxSpeed )
				currentSpeed = maxSpeed;
			if( currentSpeed < 0 )
				currentSpeed = 0;
		}
		
};
