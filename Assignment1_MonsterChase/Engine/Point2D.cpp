#include <conio.h>
#include "Point2D.h"

namespace Engine
{
	Point2D::Point2D(int i_x, int i_y) :
		_x(i_x),
		_y(i_y)
	{}

	//getters
	int Point2D::X() const { return _x; }
	int Point2D::Y() const { return _y; }

	//setters
	void Point2D::X(int i_x) { _x = i_x; }
	void Point2D::Y(int i_y) { _y = i_y; }

	//operator overloading
	bool Point2D::operator==(Point2D i_other)
	{
		return _x == i_other._x && _y == i_other._y;
	}

	bool Point2D::operator!=(Point2D i_other)
	{
		return _x != i_other._x && _y != i_other._y;
	}

	Point2D operator+(const Point2D& i_lhs, const Point2D& i_rhs)
	{
		return Point2D(i_lhs.X() + i_rhs.X(), i_lhs.Y() + i_rhs.Y());
	}

	Point2D operator-(const Point2D& i_lhs, const Point2D& i_rhs)
	{
		return Point2D(i_lhs.X() - i_rhs.X(), i_lhs.Y() - i_rhs.Y());
	}

	Point2D operator*(const Point2D& i_lhs, int i_rhs)
	{
		return Point2D(i_lhs.X() * i_rhs, i_lhs.Y() * i_rhs);
	}

	Point2D operator/(const Point2D& i_lhs, int i_rhs)
	{
		return Point2D(i_lhs.X() / i_rhs, i_lhs.Y() / i_rhs);
	}

	Point2D operator*(int i_lhs, const Point2D& i_rhs)
	{
		return Point2D(i_lhs * i_rhs.X(), i_lhs * i_rhs.Y());
	}

	Point2D operator-(const Point2D& i_rhs)
	{
		return Point2D(-i_rhs.X(), -i_rhs.Y());
	}

	void Point2D::operator+=(Point2D i_other)
	{
		_x += i_other._x;
		_y += i_other._y;
	}

	void Point2D::operator-=(Point2D i_other)
	{
		_x -= i_other._x;
		_y -= i_other._y;
	}

	void Point2D::operator*=(int i_other)
	{
		_x *= i_other;
		_y *= i_other;
	}

	void Point2D::operator/=(int i_other)
	{
		_x /= i_other;
		_y /= i_other;
	}
}
