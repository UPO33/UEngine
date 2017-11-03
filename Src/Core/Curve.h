#pragma once

#include "Base.h"

namespace UCore
{
	double CubicInterp(double p[4], double t)
	{
		return p[1] + 0.5 * t* (p[2] - p[0] + t*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + t*(3.0*(p[1] - p[2]) + p[3] - p[0])));
	}
	template< class T, class U > T CubicInterp(const T& P0, const T& T0, const T& P1, const T& T1, const U& A)
	{
		const float A2 = A  * A;
		const float A3 = A2 * A;

		return (T)(((2 * A3) - (3 * A2) + 1) * P0) + ((A3 - (2 * A2) + A) * T0) + ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1);
	}

	enum class ECureveInterpolation
	{
		EConstantFirst,
		EConstantSecond,
		ELinear,
		ECubic,
	};

	template<typename TValue> class TCurveBase
	{
		struct Point
		{
			float					mTime;
			TValue					mValue;
			float					mInTangent;
			float					mOutTangent;
			ECureveInterpolation	mInterpolation;
		};

		//must be sorted by minimum time
		TArray<Point>	mPoints;
		float			mMinTime;
		float			mMaxTime;

	public:
		TCurveBase()
		{
		}
		~TCurveBase()
		{
		}
		static float StaticPointEval(const Point& a, const Point& b, float t)
		{
			switch (a.mInterpolation)
			{
			case ECureveInterpolation::EConstantFirst:
				return a.mValue;
			case ECureveInterpolation::EConstantSecond:
				return b.mValue;
			case ECureveInterpolation::ELinear:
				return Lerp(a.mValue, b.mValue, t);
			case  ECureveInterpolation::ECubic:
				return CubicInterp(a.mValue, a.mOutTangent, b.mValue, b.mInTangent);
			}
		}
		void AddPoint(float x, float y, ECureveInterpolation interpolation)
		{
			Point newPoint;

			int index = 0;
			for (; index < mPoints.Length(); index++)
			{
				if (mPoints[index].mTime > x)
					break;
			}

			//index to put the item before
			if (index == 0)
			{
				mPoints.Add(newPoint);
			}
			else
			{
				//mPoints.InsertBefore(newPoint);
			}
		}

		float Evaluate(float x, float default = 0)
		{
			if (mPoints.Length() == 0) return default;
			if (mPoints.Length() == 1) return mPoints[0].mValue;

			int index = 0;	//the first greater found index
			for (; index < mPoints.Length(); index++)
			{
				if (mPoints[index].mTime > x)
					break;
			}

			if (index == 0)
			{
				return mPoints[0].mValue;
			}
			if (index >= mPoints.Length())
			{
				return mPoints.LastElement().mValue;
			}

			Point& a = mPoints[index - 1];
			Point& b = mPoints[index];

			float diff = mPoints[index].mTime - mPoints[index - 1].mTime;
			float diffX = x - mPoints[index - 1].mTime;
			if (diffX < 0.000001f) return mPoints[index - 1].mValue;
			float t = diff / diffX;

			return StaticPointEval(a, b, t);
		}
		float Evaluate(int index, float x)
		{

		}
		float GetMin(float default = 0) const
		{
			if (mPoints.Length()) return mPoints[0].mOffset;
			return default;
		}
		float GetMax(float default = 0) const
		{
			if (mPoints.Length()) return mPoints.LastElement().mOffset();
			return default;
		}
};