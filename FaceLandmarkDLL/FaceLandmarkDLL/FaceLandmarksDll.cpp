#include "FaceLandmarksDll.h"

namespace FaceLandmarks
{
	double Add(double a, double b)
	{
		return a + b;
	}
	double Subtract(double a, double b)
	{
		return a - b;
	}
	double Multiply(double a, double b)
	{
		return a*b;
	}
	double Divide(double a, double b)
	{
		if (b == 0)
			throw invalid_argument("b cannot be zero!");
		return a / b;
	}
	void CalculateFeaturePoints(int ID)
	{
		//Kod funkcji main z porgramu FaceLandmarkjjj



	}
}