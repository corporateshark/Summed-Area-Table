#pragma once

/**
 * \file SummedAreaTable.h
 * \brief
 *
 * Summed-Area Table
 *
 * \version 1.0.0
 * \date 14/02/2018
 * \author Sergey Kosarevsky, 2018
 * \author sk@linderdaum.com   http://www.linderdaum.com   http://blog.linderdaum.com
 */

/*
	Usage example:

		#define NO_SUMMED_AREA_TABLE_CLAMPING
		#include "SummedAreaTable.h"
		...
		// See 'example.cpp'
*/

#include <vector>
#include <assert.h>
#include <math.h>

#if !defined(NO_SUMMED_AREA_TABLE_CLAMPING)
#	define CLAMP_SUMMED_AREA_TABLE_ARGS\
		if (x0 < 0) x0 = 0; if (x0 >= width_) x0 = width_-1;\
		if (x1 < 0) x1 = 0; if (x1 >= width_) x1 = width_-1;\
		if (y0 < 0) y0 = 0; if (y0 >= height_) y0 = height_-1;\
		if (y1 < 0) y1 = 0; if (y1 >= height_) y1 = height_-1;\
		if (x0 > x1) std::swap(x0, x1);\
		if (y0 > y1) std::swap(y0, y1);
#else
#	define CLAMP_SUMMED_AREA_TABLE_ARGS
#endif // NO_SUMMED_AREA_TABLE_ASSERTS

/**
 * Single-header implementation of Summed-Area Tables
 *
 *
 * https://en.wikipedia.org/wiki/Summed-area_table
 **/
template <typename T = uint32_t, bool hasNonZeroTable = true>
class SummedAreaTable
{
public:
	SummedAreaTable() = delete;

	SummedAreaTable(const SummedAreaTable&) = default;
	SummedAreaTable(SummedAreaTable&&) = default;

	SummedAreaTable& operator= (const SummedAreaTable&) = default;
	SummedAreaTable& operator= (SummedAreaTable&&) = default;

	template <typename In> SummedAreaTable(const In* buffer, int width, int height)
	: width_(width)
	, height_(height)
	, sat_(width*height)
	, satNZ_(hasNonZeroTable ? width*height : 0)
	{
		assert(width);
		assert(height);

		initializeSAT(
			sat_.data(),
			[buffer](int ofs) { return buffer[ofs]; }
		);
		if (hasNonZeroTable)
		{
			initializeSAT(
				satNZ_.data(),
				[buffer](int ofs) { return buffer[ofs] > 0 ? 1 : 0; }
			);
		}
	}

	/// All coordinates are INCLUSIVE
	T getSum(int x0, int y0, int x1, int y1) const
	{
		CLAMP_SUMMED_AREA_TABLE_ARGS

		const auto* p = sat_.data();
		const auto w = width_;

		const auto A = (x0 > 0) && (y0 > 0) ? p[(y0-1) * w + x0 - 1] : 0;
		const auto B = y0 > 0 ? p[(y0-1) * w + x1  ] : 0;
		const auto C = x0 > 0 ? p[ y1    * w + x0-1] : 0;
		const auto D = p[y1 * w + x1];

		return D+A-B-C;
	}

	/// All coordinates are INCLUSIVE
	double getAverage(int x0, int y0, int x1, int y1) const
	{
		const double sum = static_cast<double>(getSum(x0, y0, x1, y1));
		const double numPixels = (abs(x1-x0)+1) * (abs(y1-y0)+1);

		return numPixels > 0.0 ? sum / numPixels : 0.0;
	}

	/// All coordinates are INCLUSIVE
	T getNonZeroCount(int x0, int y0, int x1, int y1) const
	{
		static_assert(hasNonZeroTable);

		CLAMP_SUMMED_AREA_TABLE_ARGS

		const auto* p = satNZ_.data();
		const auto w = width_;

		const auto A = (x0 > 0) && (y0 > 0) ? p[(y0-1) * w + x0 - 1] : 0;
		const auto B = y0 > 0 ? p[(y0-1) * w + x1  ] : 0;
		const auto C = x0 > 0 ? p[ y1    * w + x0-1] : 0;
		const auto D = p[y1 * w + x1];

		return D+A-B-C;
	}

	/// All coordinates are INCLUSIVE
	double getNonZeroAverage(int x0, int y0, int x1, int y1) const
	{
		static_assert(hasNonZeroTable);

		// zeros don't contribute to the sum
		const double sum = static_cast<double>(getSum(x0, y0, x1, y1));
		const double numNonZeroPixels = static_cast<double>(getNonZeroCount(x0, y0, x1, y1));

		return numNonZeroPixels > 0.0 ? sum / numNonZeroPixels : 0.0;
	}

	const T* getSAT() const { return sat_.data(); };
	const T* getSATNonZero() const { return satNZ_.data(); };

private:
	template <typename Lambda> void initializeSAT(T* ptr, Lambda l)
	{
		// top-left element
		ptr[0] = l(0);

		// topmost row
		for (auto x = 1; x != width_; x++)
			ptr[x] = l(x) + ptr[x-1];

		// leftmost column
		for (auto y = 1; y != height_; y++)
			ptr[y * width_] = l(y * width_) + ptr[(y-1) * width_];

		// compute the rest SAT elements
		for (auto y = 1; y != height_; y++)
			for (auto x = 1; x != width_; x++)
				ptr[y * width_ + x] =
					l(y * width_ + x) +
					ptr[(y-1) * width_ + x] +
					ptr[ y    * width_ + x-1] -
					ptr[(y-1) * width_ + x-1];
	}

private:
	int width_;
	int height_;
	/// Assuming max sum fits into T
	std::vector<T> sat_;
	std::vector<T> satNZ_;
};
