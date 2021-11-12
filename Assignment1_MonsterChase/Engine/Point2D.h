#pragma once
#include <assert.h>

namespace Engine
{
	class Point2D
	{
	public:
		Point2D(int i_x, int i_y);

		//getters
		int X() const;
		int Y() const;

		//setters
		void X(int i_x);
		void Y(int i_y);


		//operator overloading
		bool operator== (Point2D i_other);
		bool operator!= (Point2D i_other);
		void operator+= (Point2D i_other);
		void operator-= (Point2D i_other);
		void operator*= (int i_other);
		void operator/= (int i_other);

	private:
		int _x, _y;
	};

	Point2D operator+ (const Point2D& i_lhs, const Point2D& i_rhs);
	Point2D operator- (const Point2D& i_lhs, const Point2D& i_rhs);
	Point2D operator* (const Point2D& i_lhs, int i_rhs);
	Point2D operator/ (const Point2D& i_lhs, int i_rhs);
	Point2D operator* (int i_lhs, const Point2D& i_rhs);
	Point2D operator- (const Point2D& i_rhs);

	void Point2DUnitTest() {
		Point2D A(0, 1);
		Point2D B(2, 3);

		// equality
		bool bEqual = A == B;
		assert(bEqual == false);

		bEqual = B == Point2D(2, 3);
		assert(bEqual == true);

		// Inequality
		bool bNotEqual = A != B;
		assert(bNotEqual == true);

		bNotEqual = B != Point2D(2, 3);
		assert(bNotEqual == false);

		// Point2D + Point2D
		Point2D C = A + B;
		assert(C == Point2D(2, 4));

		// Point2D - Point2D
		C = B - A;
		assert(C == Point2D(2, 2));

		// Point2D * int
		C = A * 2;
		assert(C == Point2D(0, 2));

		// Point2D / int
		C = Point2D(6, 4) / 2;
		assert(C == Point2D(3, 2));

		// int * Point2D
		C = 2 * B;
		assert(C == Point2D(4, 6));

		// negate
		C = -B;
		assert(C == Point2D(-2, -3));

		// Point2D += Point2D
		B += Point2D(2, 1);
		assert(B == Point2D(4, 4));

		// Point2D -= Point2d
		B -= Point2D(2, 1);
		assert(B == Point2D(2, 3));

		// Point2D *= int
		B *= 2;
		assert(B == Point2D(4, 6));

		// Point2D /= int
		B /= 2;
		assert(B == Point2D(2, 3));
	}


}