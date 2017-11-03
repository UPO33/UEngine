#pragma once

#include "Base.h"

namespace UCore
{
	struct Int2
	{
		int x, y;

		Int2() = default;

		explicit Int2(int xy) : x(xy), y(xy) {}
		explicit Int2(int _x, int _y) : x(_x), y(_y) {}
		
	};

	//////////////////////////////////////////////////////////////////////////Int2 op Int2
	inline Int2 operator +(Int2 l, Int2 r)
	{
		return Int2(l.x + r.x, l.y + r.y);
	}
	inline Int2 operator -(Int2 l, Int2 r)
	{
		return Int2(l.x - r.x, l.y - r.y);
	}
	inline Int2 operator *(Int2 l, Int2 r)
	{
		return Int2(l.x * r.x, l.y * r.y);
	}
	inline Int2 operator /(Int2 l, Int2 r)
	{
		return Int2(l.x / r.x, l.y / r.y);
	}

	//////////////////////////////////////////////////////////////////////////Int2 op= Int2
	inline Int2 operator += (Int2& l, const Int2& r)
	{
		l.x += r.x;
		l.y += r.y;
		return l;
	}
	inline Int2 operator -= (Int2& l, const Int2& r)
	{
		l.x -= r.x;
		l.y -= r.y;
		return l;
	}
	inline Int2 operator *= (Int2& l, const Int2& r)
	{
		l.x *= r.x;
		l.y *= r.y;
		return l;
	}
	inline Int2 operator /= (Int2& l, const Int2& r)
	{
		l.x /= r.x;
		l.y /= r.y;
	}
	//////////////////////////////////////////////////////////////////////////cmp
	inline bool operator == (Int2 l, Int2 r)
	{
		return l.x == r.x && l.y == r.y;
	}
	inline bool operator != (Int2 l, Int2 r)
	{
		return l.x != r.x || l.y != r.y;
	}
	inline bool operator > (Int2 l, Int2 r)
	{
		return l.x > r.x && l.y > r.y;
	}
	inline bool operator < (Int2 l, Int2 r)
	{
		return l.x < r.x && l.y < r.y;
	}
	inline bool operator >= (Int2 l, Int2 r)
	{
		return l.x >= r.x && l.y >= r.y;
	}
	inline bool operator <= (Int2 l, Int2 r)
	{
		return l.x <= r.x && l.y <= r.y;
	}
	inline StringStreamOut& operator << (StringStreamOut& l, Int2 r)
	{
		l << "{" << r.x << ", " << r.y << "}";
		return l;
	}

}