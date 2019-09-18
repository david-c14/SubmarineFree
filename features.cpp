#ifdef _WIN32

		//  Windows
#define cpuid(info, x)    __cpuidex(info, x, 0)

#else

		//  GCC Intrinsics
#include <cpuid.h>
	void cpuid(int info[4], int InfoType) {
		__cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
	}

#endif


	//  Misc.
	bool HW_MMX;
	bool HW_x64;
	bool HW_ABM;      // Advanced Bit Manipulation
	bool HW_RDRAND;
	bool HW_BMI1;
	bool HW_BMI2;
	bool HW_ADX;
	bool HW_PREFETCHWT1;

	//  SIMD: 128-bit
	bool HW_SSE;
	bool HW_SSE2;
	bool HW_SSE3;
	bool HW_SSSE3;
	bool HW_SSE41;
	bool HW_SSE42;
	bool HW_SSE4a;
	bool HW_AES;
	bool HW_SHA;

	//  SIMD: 256-bit
	bool HW_AVX;
	bool HW_XOP;
	bool HW_FMA3;
	bool HW_FMA4;
	bool HW_AVX2;

	//  SIMD: 512-bit
	bool HW_AVX512F;    //  AVX512 Foundation
	bool HW_AVX512CD;   //  AVX512 Conflict Detection
	bool HW_AVX512PF;   //  AVX512 Prefetch
	bool HW_AVX512ER;   //  AVX512 Exponential + Reciprocal
	bool HW_AVX512VL;   //  AVX512 Vector Length Extensions
	bool HW_AVX512BW;   //  AVX512 Byte + Word
	bool HW_AVX512DQ;   //  AVX512 Doubleword + Quadword
	bool HW_AVX512IFMA; //  AVX512 Integer 52-bit Fused Multiply-Add
	bool HW_AVX512VBMI; //  AVX512 Vector Byte Manipulation Instructions

	bool OS_AVX;

	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned nExIds = info[0];

	//  Detect Features
	if (nIds >= 0x00000001) {
		cpuid(info, 0x00000001);
		HW_MMX = (info[3] & ((int)1 << 23)) != 0;
		HW_SSE = (info[3] & ((int)1 << 25)) != 0;
		HW_SSE2 = (info[3] & ((int)1 << 26)) != 0;
		HW_SSE3 = (info[2] & ((int)1 << 0)) != 0;

		HW_SSSE3 = (info[2] & ((int)1 << 9)) != 0;
		HW_SSE41 = (info[2] & ((int)1 << 19)) != 0;
		HW_SSE42 = (info[2] & ((int)1 << 20)) != 0;
		HW_AES = (info[2] & ((int)1 << 25)) != 0;

		HW_AVX = (info[2] & ((int)1 << 28)) != 0;
		HW_FMA3 = (info[2] & ((int)1 << 12)) != 0;

		HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;

		bool osUsesXSAVE_XRSTORE = info[2] & (1 << 27) || false;

		if (osUsesXSAVE_XRSTORE && HW_AVX)
		{
			unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			OS_AVX = (xcrFeatureMask & 0x6) == 0x6;
		}
	}
	if (nIds >= 0x00000007) {
		cpuid(info, 0x00000007);
		HW_AVX2 = (info[1] & ((int)1 << 5)) != 0;

		HW_BMI1 = (info[1] & ((int)1 << 3)) != 0;
		HW_BMI2 = (info[1] & ((int)1 << 8)) != 0;
		HW_ADX = (info[1] & ((int)1 << 19)) != 0;
		HW_SHA = (info[1] & ((int)1 << 29)) != 0;
		HW_PREFETCHWT1 = (info[2] & ((int)1 << 0)) != 0;

		HW_AVX512F = (info[1] & ((int)1 << 16)) != 0;
		HW_AVX512CD = (info[1] & ((int)1 << 28)) != 0;
		HW_AVX512PF = (info[1] & ((int)1 << 26)) != 0;
		HW_AVX512ER = (info[1] & ((int)1 << 27)) != 0;
		HW_AVX512VL = (info[1] & ((int)1 << 31)) != 0;
		HW_AVX512BW = (info[1] & ((int)1 << 30)) != 0;
		HW_AVX512DQ = (info[1] & ((int)1 << 17)) != 0;
		HW_AVX512IFMA = (info[1] & ((int)1 << 21)) != 0;
		HW_AVX512VBMI = (info[2] & ((int)1 << 1)) != 0;
	}
	if (nExIds >= 0x80000001) {
		cpuid(info, 0x80000001);
		HW_x64 = (info[3] & ((int)1 << 29)) != 0;
		HW_ABM = (info[2] & ((int)1 << 5)) != 0;
		HW_SSE4a = (info[2] & ((int)1 << 6)) != 0;
		HW_FMA4 = (info[2] & ((int)1 << 16)) != 0;
		HW_XOP = (info[2] & ((int)1 << 11)) != 0;
	}

	if (HW_MMX) printf("MMX ");
	if (HW_x64) printf("x64 ");
	if (HW_ABM) printf("ABM ");
	if (HW_RDRAND) printf("RDRAND ");
	if (HW_BMI1) printf("BMI1 ");
	if (HW_BMI2) printf("BMI2 ");
	if (HW_ADX) printf("ADX ");
	if (HW_PREFETCHWT1) printf("PREFETCHWT1 ");
	if (HW_SSE) printf("SSE ");
	if (HW_SSE2) printf("SSE2 ");
	if (HW_SSE3) printf("SSE3 ");
	if (HW_SSSE3) printf("SSSE3 ");
	if (HW_SSE41) printf("SSE41 ");
	if (HW_SSE42) printf("SSE42 ");
	if (HW_SSE4a) printf("SSE4a ");
	if (HW_AES) printf("AES ");
	if (HW_SHA) printf("SHA ");
	if (HW_AVX) printf("AVX ");
	if (HW_XOP) printf("XOP ");
	if (HW_FMA3) printf("FMA3 ");
	if (HW_FMA4) printf("FMA4 ");
	if (HW_AVX2) printf("AVX2 ");
	if (HW_AVX512F) printf("AVX512F ");
	if (HW_AVX512CD) printf("AVX512CD ");
	if (HW_AVX512PF) printf("AVX512PF ");
	if (HW_AVX512ER) printf("AVX512ER ");
	if (HW_AVX512VL) printf("AVX512VL ");
	if (HW_AVX512BW) printf("AVX512BW ");
	if (HW_AVX512DQ) printf("AVX512DQ ");
	if (HW_AVX512IFMA) printf("AVX512IFMA ");
	if (HW_AVX512VBMI) printf("AVX512VBMI ");
	if (OS_AVX) printf("OS-AVX ");
	printf("\n");


