#include <math.h>
#include <stdio.h>
#include <cstdint>
#include <cstdlib>
#include <cfloat>

inline float rsqrt(float number) {
	return 1/sqrt(number);
}

inline float Q_rsqrt(float number) {
	int32_t i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = * ((int32_t *) (&y));
	i = 0x5f3759df - (i >> 1);
	y = * ((float *) (&i));
	y = y * (threehalfs - (x2 * y * y));
}

int main (int argc, char *argv[]) {
	for (unsigned int j = 0; j < 1; j++) {
		for(unsigned int i = 0; i < 1; i++) {
			float x = 20.0f * rand() / RAND_MAX - 10.0f;;
			float y = 20.0f * rand() / RAND_MAX - 10.0f;;
			float result = x * x + y * y;
			printf("%d %f  %f  %f  %f\n", FLT_MANT_DIG, x, y, rsqrt(result), Q_rsqrt(result));
		}
	}
}

/* 
FLT_MANT_DIG = 24 for IEEE 754 32-bit fp
FLT_MANT_DIG = 53 for IEEE 754 64-bit fp
Magic number = 0x5FE6EB50C7B537A9 for 64-bit fp
*/
