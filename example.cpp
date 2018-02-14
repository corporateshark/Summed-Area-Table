#include <stdint.h>
#include <stdio.h>

//#define NO_SUMMED_AREA_TABLE_CLAMPING
#include "SummedAreaTable.h"

template <typename T>
void printPixels(T* buf, int w, int h)
{
	printf("\n%5c", ' '); for (auto x = 0; x != w; x++) printf("%4u ", x);
	printf("\n%5c", ' '); for (auto x = 0; x != 5*w; x++) printf("%c", '-');

	for (auto y = 0; y != h; y++)
	{
		printf("\n%4u|", y);
		for (auto x = 0; x != w; x++)
			printf("%4u ", (unsigned int)buf[y * w + x]);
	}
	printf("\n\n");
}

void test1()
{
	printf("\nTest 1:\n\n");

	const uint8_t Pixels[] =
		{ 0, 4, 0, 2, 1, 0 };

	SummedAreaTable<uint32_t> ps( Pixels, 6, 1 );

	printf("Sum/Avg:          %4u   %f\n", ps.getSum(0,0,5,0), ps.getAverage(0,0,5,0));
	printf("NonZeroCount/Avg: %4u   %f\n", ps.getNonZeroCount(0,0,5,0), ps.getNonZeroAverage(0,0,5,0));
}

void test2()
{
	printf("\nTest 2:\n\n");

	const uint8_t Pixels[] =
		{
			0, 0, 0, 2, 0, 0,
			0, 0, 2, 0, 2, 0,
			0, 0, 2, 3, 2, 0,
			0, 0, 2, 0, 2, 0,
		};

	SummedAreaTable<uint32_t> ps( Pixels, 6, 4 );

	printf("Input:");
	printPixels(Pixels, 6, 4);
	printf("SAT:");
	printPixels(ps.getSAT(), 6, 4);
	printf("SAT NonZero:");
	printPixels(ps.getSATNonZero(), 6, 4);

	printf("%4u %f\n", ps.getSum(2,1,2,1), ps.getAverage(2,1,2,1));
	printf("%4u %f\n", ps.getSum(1,1,2,2), ps.getAverage(1,1,2,2));
	printf("%4u %f\n", ps.getSum(0,0,5,3), ps.getAverage(0,0,5,3));
	printf("%4u %f\n", ps.getSum(4,0,5,3), ps.getAverage(4,0,5,3));

	printf("\n");
	printf("NonZeroCount/Avg: %4u   %f\n", ps.getNonZeroCount(0,0,5,3), ps.getNonZeroAverage(0,0,5,3));
}

int main()
{
	test1();
	test2();

	return 0;
}
