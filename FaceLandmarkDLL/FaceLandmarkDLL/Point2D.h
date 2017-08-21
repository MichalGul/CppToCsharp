#pragma once
class Point2D
{
public:

	long int X;
	long int Y;

	Point2D(long int PointX, long PointY)
	{
		X = PointX;
		Y = PointY;
	}
	Point2D()
	{
		X = 0;
		Y = 0;
	}
	~Point2D()
	{}
};


