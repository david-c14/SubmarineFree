//SubTag TM TW W11 W17 W23 W29 W41

#include <cmath>
#include "SubmarineFree.hpp"

namespace {

	static float FunctorClipboard = NAN;

	typedef float (*func_t)(float, float, float);

	struct Functor {
		unsigned int category;
		std::string name;
		func_t func;
		std::string description;
	};

	std::vector<std::string> categories {
		"Pass through",
		"Addition",
		"Subtraction",
		"Multiplication",
		"Division",
		"Modulo",
		"Quadratic",
		"Square Root",
		"Powers",
		"Modulus",
		"Min-max",
		"Trigonometric",
		"Inverse Trigonometric",
		"Logarithmic",
		"Exponential",
		"Conditional X and 0",
		"Conditional Y and 0",
		"Conditional X and Y",
		"Conditional X and C",
		"Conditional Y and C",
		"Rounding"
	};

#define LAMBDA(e) [](float x, float y, float c)->float { return e ; }
#define X "X"			// X
#define Y "Y"			// Y
#define C "C"			// C
#define A "+"			// Addition symbol
#define S "-"			// Subtraction symbol
#define O "%"			// Modulo symbol
#define OP "("			// Open Parenthesis
#define CP ")"			// Close Parenthesis
#define P "|"			// Pipe symbol
#define M "\xe2\xa8\xaf"	// Multiplication symbol
#define D "\xc3\xb7"		// Division symbol
#define R "\xe2\x88\x9a"	// Root symbol
#define S2 "\xc2\xb2"		// Superscript 2
#define S3 "\xc2\xb3"		// Superscript 3
#define s0 "\xe2\x82\x80"	// Subscript 0
#define s1 "\xe2\x82\x81"	// Subscript 1
#define s2 "\xe2\x82\x82"	// Subscript 2
#define E "\xe2\x84\xaf"	// e
#define SA "\xe2\x81\xba"	// Superscript +
#define SX "\xcb\xa3"		// Superscript x
#define SY "\xca\xb8"		// Superscript y
#define SC "\xe1\xb6\x9c"	// Superscript c
#define MIN "min"		// Minimum function
#define MAX "max"		// Maximum function
#define COMMA ","		// Comma symbol
#define SIN "sin"		// sine function
#define COS "cos"		// cosine function
#define TAN "tan"		// tangent function
#define ASIN "asin"		// arcsine function
#define ACOS "acos"		// arcosine function
#define ATAN "atan"		// arctangent function
#define LOG "log"		// log function
#define LOG2 LOG s2		// base-2 log function
#define LOG10 LOG s1 s0		// base-10 log function
#define IF "if "		// if conditional
#define G ">"			// Greater Than symbol
#define L "<"			// Less Than symbol
#define Q "="			// Equality symbol
#define Z "0"			// Zero
#define W "1"			// One
#define T "\xe2\x86\xa3"	// Right arrow
#define H "/"			// Slash
#define Pi "\xcf\x80"		// PI
#define TAU "\xcf\x84"		// TAU
#define F "\xc6\x92"		// frequency
#define LAM "\xce\xbb"		// lambda
#define LC "\xe2\x8c\x88"	// Left Ceiling
#define RC "\xe2\x8c\x89"	// Right Ceiling
#define LF "\xe2\x8c\x8a"	// Left Floor
#define RF "\xe2\x8c\x8b"	// Right Floor


	std::vector<Functor> functions {
#define CATEGORY 0
		{ CATEGORY, "",                      	LAMBDA(  0                      ), "" }, // Passthrough
#undef CATEGORY
#define CATEGORY 1
		{ CATEGORY, C,                       	LAMBDA(  c                      ), "Addition: Constant value" }, // Addition
		{ CATEGORY, X A C,                   	LAMBDA(  x + c                  ), "Addition: X plus constant" },
		{ CATEGORY, Y A C,                   	LAMBDA(  y + c                  ), "Addition: Y plus constant" },
		{ CATEGORY, X A Y A C,               	LAMBDA(  x + y + c              ), "Addition: X plus Y plus constant" },
#undef CATEGORY
#define CATEGORY 2
		{ CATEGORY, C S X,                   	LAMBDA(  c - x                  ), "Subtraction: Constant less X" }, // Subtraction
		{ CATEGORY, C S Y,                   	LAMBDA(  c - y                  ), "Subtraction: Constant less Y" },
		{ CATEGORY, X S OP Y A C CP,         	LAMBDA(  x - ( y + c )          ), "Subtraction: X less Y and constant" },
		{ CATEGORY, OP X A C CP S Y,         	LAMBDA(  ( x + c ) - y          ), "Subtraction: X and constant less Y" },
		{ CATEGORY, Y S OP X A C CP,         	LAMBDA(  y - ( x + c )          ), "Subtraction: Y less X and constant" },
		{ CATEGORY, OP Y A C CP S X,         	LAMBDA(  ( y + c ) - x          ), "Subtraction: Y and constant less X" },
#undef CATEGORY
#define CATEGORY 3
		{ CATEGORY, OP X M Y CP A C,         	LAMBDA(  ( x * y ) + c          ), "Multiplication: X by Y plus constant" }, // Multiplication
		{ CATEGORY, OP X A C CP M Y,         	LAMBDA(  ( x + c ) * y          ), "Multiplication: X and constant by Y" },
		{ CATEGORY, X M OP Y A C CP,         	LAMBDA(  x * ( y + c )          ), "Multiplication: X by Y and constant" },
		{ CATEGORY, X M C,                   	LAMBDA(  x * c                  ), "Multiplication: X by constant" },
		{ CATEGORY, Y M C,                   	LAMBDA(  y * c                  ), "Multiplication: Y by constant" },
		{ CATEGORY, X M Y M C,               	LAMBDA(  x * y * c              ), "Multiplication: X by Y by constant" },
		{ CATEGORY, Pi M OP X A C CP,	   	LAMBDA(  M_PI * ( x + c )	), "Multiplication: X and constant by pi" },
		{ CATEGORY, Pi M OP Y A C CP,	   	LAMBDA(  M_PI * ( y + c )	), "Multiplication: Y and constant by pi" },
		{ CATEGORY, TAU M OP X A C CP,	   	LAMBDA(  2 * M_PI * ( x + c )   ), "Multiplication: X and constant by tau" },
		{ CATEGORY, TAU M OP Y A C CP,	   	LAMBDA(  2 * M_PI * ( y + c )   ), "Multiplication: Y and constant by tau" },
#undef CATEGORY
#define CATEGORY 4
		{ CATEGORY, X D C,                   	LAMBDA(  x / c                  ), "Division: X over constant" }, // Division
		{ CATEGORY, C D X,                   	LAMBDA(  c / x                  ), "Division: Constant over X" },
		{ CATEGORY, Y D C,                   	LAMBDA(  y / c                  ), "Division: Y over constant" },
		{ CATEGORY, C D Y,                   	LAMBDA(  c / y                  ), "Division: Constant over Y" },
		{ CATEGORY, C A OP X D Y CP,         	LAMBDA(  c + ( x / y )          ), "Division: Constant plus X over Y" },
		{ CATEGORY, C A OP Y D X CP,      	LAMBDA(  c + ( y / x )          ), "Division: Constant plus Y over X" },
		{ CATEGORY, X A OP Y D C CP,	   	LAMBDA(  x + ( y / c )	        ), "Division: X plus Y over constant" },	
		{ CATEGORY, X A OP C D Y CP,	   	LAMBDA(  x + ( c / y ) 	        ), "Division: X plus constant over Y" },
 		{ CATEGORY, Y A OP X D C CP,	   	LAMBDA(  y + ( x / c )	        ), "Division: Y plus X over constant" },
		{ CATEGORY, Y A OP C D X CP,	   	LAMBDA(  y + ( c / x ) 	        ), "Division: Y plus constant over Y" },
		{ CATEGORY, OP X A C CP D Y,         	LAMBDA(  ( x + c ) / y          ), "Division: X and constant over Y" },
		{ CATEGORY, X D OP Y A C CP,         	LAMBDA(  x / ( y + c )          ), "Division: X over Y and constant" },
		{ CATEGORY, OP Y A C CP D X,         	LAMBDA(  ( y + c ) / x          ), "Division: Y and constant over X" },
		{ CATEGORY, Y D OP X A C CP,         	LAMBDA(  y / ( x + c )          ), "Division: Y over constant and Y" },
#undef CATEGORY
#define CATEGORY 5
		{ CATEGORY, OP X A C CP O Y,	   	LAMBDA(  fmodf( x + c , y )	), "Modulo: X and constant reduced modulo Y" }, // Modulo
		{ CATEGORY, OP Y A C CP O X,	   	LAMBDA(  fmodf( y + c , x )	), "Modulo: Y and constant reduced modulo X" },
		{ CATEGORY, X O OP Y A C CP,	   	LAMBDA(  fmodf( x , y + c )	), "Modulo: X reduced modulo Y and constant" },
		{ CATEGORY, Y O OP X A C CP,	   	LAMBDA(  fmodf( y , x + c)	), "Modulo: Y reduced modulo X and constant" },
		{ CATEGORY, X O C,		   	LAMBDA(  fmodf( x , c )	        ), "Modulo: X reduced modulo constant" },
		{ CATEGORY, Y O C,		   	LAMBDA(  fmodf( y , c )  	), "Modulo: Y reduced module constant" },
#undef CATEGORY
#define CATEGORY 6
		{ CATEGORY, X S2 A C,                	LAMBDA(  x * x + c              ), "Quadratic: X squared plus constant" }, // Quadratic
		{ CATEGORY, Y S2 A C,                	LAMBDA(  y * y + c              ), "Quadratic: Y squared plus constant" },
		{ CATEGORY, OP X A C CP S2,          	LAMBDA(  ( x + c ) * ( x + c )  ), "Quadratic: X and constant squared" },
		{ CATEGORY, OP Y A C CP S2,          	LAMBDA(  ( y + c ) * ( y + c )  ), "Quadratic: Y and constant squared" },
		{ CATEGORY, X S2 A Y A C,            	LAMBDA(  x * x + y + c          ), "Quadratic: X squared plus Y plus constant" },
		{ CATEGORY, Y S2 A X A C,            	LAMBDA(  y * y + x + c          ), "Quadratic: Y squared plus X plus constant" },
		{ CATEGORY, X S2 A C Y,              	LAMBDA(  x * x + c * y          ), "Quadratic: X squared plus Y by constant" },
		{ CATEGORY, Y S2 A C X,              	LAMBDA(  y * y + c * x          ), "Quadratic: Y squared plus X by constant" },
#undef CATEGORY
#define CATEGORY 7
		{ CATEGORY, R OP X A C CP,           	LAMBDA(  sqrt( x + c )          ), "Root: Square root of X and constant" }, // Square Root
		{ CATEGORY, R OP Y A C CP,           	LAMBDA(  sqrt( y + c )          ), "Root: Square root of Y and constant" },
#undef CATEGORY
#define CATEGORY 8
		{ CATEGORY, C SX,			LAMBDA(  powf( c , x )	        ), "Power: Constant to the power of X" }, // Powers
		{ CATEGORY, C SY,			LAMBDA(  powf( c , y )	        ), "Power: Constant to the power of Y" },
		{ CATEGORY, C SX SA SY,		   	LAMBDA(  powf( c , x + y ) 	), "Power: Constant to the power of X and Y" },
		{ CATEGORY, C SX SY,		   	LAMBDA(  powf( c , x * y )	), "Power: Constant to the power of X by Y" },
		{ CATEGORY, X SC,			LAMBDA(  powf( x , c ) 	        ), "Power: X to the power of constant" },
		{ CATEGORY, Y SC,			LAMBDA(  powf( y , c )	        ), "Power: Y to the power of constant" },
		{ CATEGORY, X SY SA SC, 		LAMBDA(  powf( x , y + c )      ), "Power: X to the power of Y and constant" },
		{ CATEGORY, Y SX SA SC,		   	LAMBDA(  powf( y , x + c )	), "Power: Y to the power of X and constant" },
		{ CATEGORY, X SC SY,		   	LAMBDA(  powf( x , c * y )	), "Power: X to the power of Y by constant" },
		{ CATEGORY, Y SC SX,		   	LAMBDA(  powf( y , c * x )	), "Power: Y to the power of X by constant" },
#undef CATEGORY
#define CATEGORY 9
                { CATEGORY, P X A C P,               	LAMBDA(  std::abs( x + c )      ), "Modulus: Magnitude of X and constant (without the sign)" }, // Modulus
		{ CATEGORY, P Y A C P,               	LAMBDA(  std::abs( y + c )      ), "Modulus: Magnitude of Y and constant (without the sign)" },
#undef CATEGORY
#define CATEGORY 10
		{ CATEGORY, MIN OP X A C COMMA Y CP, 	LAMBDA(  std::min( x + c, y )   ), "MinMax: Smaller of X and constant or Y" }, // Minmax
		{ CATEGORY, MIN OP X COMMA C CP,     	LAMBDA(  std::min( x, c )       ), "MinMax: Smaller of X or constant" },
      		{ CATEGORY, MIN OP Y COMMA C CP,     	LAMBDA(  std::min( y, c )       ), "MinMax: Smaller of Y or constant" },
     		{ CATEGORY, MAX OP X A C COMMA Y CP, 	LAMBDA(  std::max( x + c, y )   ), "MinMax: Larger of X and constant or Y" },
		{ CATEGORY, MAX OP X COMMA C CP,     	LAMBDA(  std::max( x, c )       ), "MinMax: Larger of X and constant" },
		{ CATEGORY, MAX OP Y COMMA C CP,     	LAMBDA(  std::max( y, c )       ), "MinMax: Larger of Y and constant" },
#undef CATEGORY
#define CATEGORY 11
		{ CATEGORY, SIN OP X A C CP,         	LAMBDA(  sin( x + c )           ), "Trigonometry: Sine of X and constant" }, // Trigonometric
		{ CATEGORY, SIN OP Y A C CP,         	LAMBDA(  sin( y + c )           ), "Trigonometry: Sine of Y and constant" },
		{ CATEGORY, SIN OP X A Y CP,         	LAMBDA(  sin( x + y )           ), "Trigonometry: Sine of X and Y" },
 		{ CATEGORY, SIN OP C X CP,           	LAMBDA(  sin( c * x )           ), "Trigonometry: Sine of X by constant" },
		{ CATEGORY, SIN OP C Y CP,           	LAMBDA(  sin( c * y )           ), "Trigonometry: Sine of Y by constant" },
		{ CATEGORY, SIN OP X Y CP,           	LAMBDA(  sin( x * y )           ), "Trigonometry: Sine of X by Y" },
		{ CATEGORY, COS OP X A C CP,         	LAMBDA(  cos( x + c )           ), "Trigonometry: Cosine of X and constant" },
		{ CATEGORY, COS OP Y A C CP,         	LAMBDA(  cos( y + c )           ), "Trigonometry: Cosine of Y and constant" },
		{ CATEGORY, COS OP X A Y CP,         	LAMBDA(  cos( x + y )           ), "Trigonometry: Cosine of X and Y" },
 		{ CATEGORY, COS OP C X CP,           	LAMBDA(  cos( c * x )           ), "Trigonometry: Cosine of X by constant" },
		{ CATEGORY, COS OP C Y CP,           	LAMBDA(  cos( c * y )           ), "Trigonometry: Cosine of Y by constant" },
		{ CATEGORY, COS OP X Y CP,           	LAMBDA(  cos( x * y )           ), "Trigonometry: Cosine of X by Y" },
		{ CATEGORY, TAN OP X A C CP,         	LAMBDA(  tan( x + c )           ), "Trigonometry: Tangent of X and constant" },
		{ CATEGORY, TAN OP Y A C CP,         	LAMBDA(  tan( y + c )           ), "Trigonometry: Tangent of Y and constant" },
		{ CATEGORY, TAN OP X A Y CP,         	LAMBDA(  tan( x + y )           ), "Trigonometry: Tangent of X and Y" },
 		{ CATEGORY, TAN OP C X CP,           	LAMBDA(  tan( c * x )           ), "Trigonometry: Tangent of X by constant" },
		{ CATEGORY, TAN OP C Y CP,           	LAMBDA(  tan( c * y )           ), "Trigonometry: Tangent of Y by constant" },
		{ CATEGORY, TAN OP X Y CP,           	LAMBDA(  tan( x * y )           ), "Trigonometry: Tangent of X by Y" },
#undef CATEGORY
#define CATEGORY 12
		{ CATEGORY, ASIN OP X A C CP,        	LAMBDA(  asin( x + c )          ), "Trigonometry: Arcsine of X and constant" }, // Inverse Trigonometric
		{ CATEGORY, ASIN OP Y A C CP,        	LAMBDA(  asin( y + c )          ), "Trigonometry: Arcsine of Y and constant" },
		{ CATEGORY, ASIN OP X A Y CP,        	LAMBDA(  asin( x + y )          ), "Trigonometry: Arcsine of X and Y" },
 		{ CATEGORY, ASIN OP C X CP,          	LAMBDA(  asin( c * x )          ), "Trigonometry: Arcsine of X by constant" },
		{ CATEGORY, ASIN OP C Y CP,          	LAMBDA(  asin( c * y )          ), "Trigonometry: Arcsine of Y by constant" },
		{ CATEGORY, ASIN OP X Y CP,          	LAMBDA(  asin( x * y )          ), "Trigonometry: Arcsine of X by Y" },
		{ CATEGORY, ACOS OP X A C CP,        	LAMBDA(  acos( x + c )          ), "Trigonometry: Arcosine of X and constant" },
		{ CATEGORY, ACOS OP Y A C CP,        	LAMBDA(  acos( y + c )          ), "Trigonometry: Arcosine of Y and constant" },
		{ CATEGORY, ACOS OP X A Y CP,        	LAMBDA(  acos( x + y )          ), "Trigonometry: Arcosine of X and Y" },
 		{ CATEGORY, ACOS OP C X CP,          	LAMBDA(  acos( c * x )          ), "Trigonometry: Arcosine of X by constant" },
		{ CATEGORY, ACOS OP C Y CP,          	LAMBDA(  acos( c * y )          ), "Trigonometry: Arcosine of Y by constant" },
		{ CATEGORY, ACOS OP X Y CP,          	LAMBDA(  acos( x * y )          ), "Trigonometry: Arcoside of X by Y" },
		{ CATEGORY, ATAN OP X A C CP,        	LAMBDA(  atan( x + c )          ), "Trigonometry: Arctangent of X and constant" },
		{ CATEGORY, ATAN OP Y A C CP,        	LAMBDA(  atan( y + c )          ), "Trigonometry: Arctangent of Y and constant" },
		{ CATEGORY, ATAN OP X A Y CP,        	LAMBDA(  atan( x + y )          ), "Trigonometry: Arctangent of X and Y" },
 		{ CATEGORY, ATAN OP C X CP,          	LAMBDA(  atan( c * x )          ), "Trigonometry: Arctangent of X by constant" },
		{ CATEGORY, ATAN OP C Y CP,          	LAMBDA(  atan( c * y )          ), "Trigonometry: Arctangent of Y by constant" },
		{ CATEGORY, ATAN OP X Y CP,          	LAMBDA(  atan( x * y )          ), "Trigonometry: Arctangent of X by Y" },
#undef CATEGORY
#define CATEGORY 13
		{ CATEGORY, LOG OP X A C CP,         	LAMBDA(  log( x + c )           ), "Logarithmic: Natural logarithm of X and constant" }, // Logarithmic
		{ CATEGORY, LOG OP Y A C CP,         	LAMBDA(  log( y + c )           ), "Logarithmic: Natural logarithm of Y and constant" },
		{ CATEGORY, LOG2 OP X A C CP,        	LAMBDA(  log2( x + c )          ), "Logarithmic: Base-2 logarithm of X and constant" },
		{ CATEGORY, LOG2 OP Y A C CP,        	LAMBDA(  log2( y + c )          ), "Logarithmic: Base-2 logarithm of Y and constant" },
		{ CATEGORY, LOG10 OP X A C CP,       	LAMBDA(  log10( x + c )         ), "Logarithmic: Base-10 logarithm of X and constant" },
		{ CATEGORY, LOG10 OP Y A C CP,       	LAMBDA(  log10( y + c )         ), "Logarithmic: Base-10 logarithm of Y and constant" },
#undef CATEGORY
#define CATEGORY 14
		{ CATEGORY, E SX SA SC,              	LAMBDA(  exp( x + c )           ), "Exponentiation: e to the power of X and constant" }, // Exponential
		{ CATEGORY, E SY SA SC,              	LAMBDA(  exp( y + c )           ), "Exponentiation: e to the power of Y and constant" },
		{ CATEGORY, E SC SX,                 	LAMBDA(  exp( c * x )           ), "Exponentiation: e to the power of X by constant" },
		{ CATEGORY, E SC SY,                 	LAMBDA(  exp( c * y )           ), "Exponentiation: e to the power of Y by constant" },
		{ CATEGORY, "2" SX SA SC,            	LAMBDA(  powf( 2, x + c )       ), "Exponentiation: 2 to the power of X and constant" },
		{ CATEGORY, "2" SY SA SC,            	LAMBDA(  powf( 2, y + c )       ), "Exponentiation: 2 to the power of Y and constant" },
		{ CATEGORY, "2" SC SX,               	LAMBDA(  powf( 2, c * x )       ), "Exponentiation: 2 to the power of X by constant" },
		{ CATEGORY, "2" SC SY,               	LAMBDA(  powf( 2, c * y )       ), "Exponentiation: 2 to the power of Y by constant" },
		{ CATEGORY, "10" SX SA SC,           	LAMBDA(  powf( 10, x + c )      ), "Exponentiation: 10 to the power of X and constant" },
		{ CATEGORY, "10" SY SA SC,           	LAMBDA(  powf( 10, y + c )      ), "Exponentiation: 10 to the power of Y and constant" },
		{ CATEGORY, "10" SC SX,              	LAMBDA(  powf( 10, c * x )      ), "Exponentiation: 10 to the power of X by constant" },
		{ CATEGORY, "10" SC SY,              	LAMBDA(  powf( 10, c * y )      ), "Exponentiation: 10 to the power of Y by constant" },
#undef CATEGORY
#define CATEGORY 15
		{ CATEGORY, IF X G Z T Y H C,	   	LAMBDA(  (x > 0) ? y : c	   ), "Conditional: If X is positive then Y otherwise constant" }, // Conditional X and 0
		{ CATEGORY, IF X L Z T Y H C,	   	LAMBDA(  (x < 0) ? y : c	   ), "Conditional: If X is negative then Y otherwise constant" },
		{ CATEGORY, IF X Q Z T Y H C,	   	LAMBDA(  (x == 0) ? y : c	   ), "Conditional: If X is zero then Y otherwise constant" },
		{ CATEGORY, IF X G Z T C H Y,	   	LAMBDA(  (x > 0) ? c : y	   ), "Conditional: If X is positive then constant otherwise Y" },
		{ CATEGORY, IF X L Z T C H Y,	   	LAMBDA(  (x < 0) ? c : y	   ), "Conditional: If X is negative then constant otherwise Y" },
		{ CATEGORY, IF X Q Z T C H Y,	   	LAMBDA(  (x == 0) ? c : y	   ), "Conditional: If X is zero then constant otherwise Y" },
		{ CATEGORY, IF X G Z T W H Z,	   	LAMBDA(  (x > 0) ? 1 : 0	   ), "Conditional: If X is positive then 1 otherwise 0" },
		{ CATEGORY, IF X L Z T W H Z,	   	LAMBDA(  (x < 0) ? 1 : 0	   ), "Conditional: If X is negative then 1 otherwise 0" },
		{ CATEGORY, IF X Q Z T W H Z,	   	LAMBDA(  (x == 0) ? 1 : 0	   ), "Conditional: If X is zero then 1 otherwise 0" },
		{ CATEGORY, IF X G Z T X H C,	   	LAMBDA(  (x > 0) ? x : c	   ), "Conditional: If X is positive then X otherwise constant" },
		{ CATEGORY, IF X L Z T X H C,	   	LAMBDA(  (x < 0) ? x : c	   ), "Conditional: If X is negative then X otherwise constant" },
		{ CATEGORY, IF X Q Z T X H C,	   	LAMBDA(  (x == 0) ? x : c	   ), "Conditional: If X is zero then X otherwise constant" },
		{ CATEGORY, IF X G Z T C H X,	   	LAMBDA(  (x > 0) ? c : x	   ), "Conditional: If X is positive then constant otherwise X" },
		{ CATEGORY, IF X L Z T C H X,	   	LAMBDA(  (x < 0) ? c : x	   ), "Conditional: If X is negative then constant otherwise X" },
		{ CATEGORY, IF X Q Z T C H X,	   	LAMBDA(  (x == 0) ? c : x	   ), "Conditional: If X is zero then constant otherwise X" },
#undef CATEGORY
#define CATEGORY 16
		{ CATEGORY, IF Y G Z T X H C,	   	LAMBDA(  (y > 0) ? x : c	   ), "Conditional: If Y is positive then X otherwise constant" }, // Conditional Y and 0
		{ CATEGORY, IF Y L Z T X H C,	   	LAMBDA(  (y < 0) ? x : c	   ), "Conditional: If Y is negative then X otherwise constant" },
		{ CATEGORY, IF Y Q Z T X H C,	   	LAMBDA(  (y == 0) ? x : c	   ), "Conditional: If Y is zero then X otherwise constant" },
		{ CATEGORY, IF Y G Z T C H X,	   	LAMBDA(  (y > 0) ? c : x	   ), "Conditional: If Y is positive then constant otherwise X" },
		{ CATEGORY, IF Y L Z T C H X,	   	LAMBDA(  (y < 0) ? c : x	   ), "Conditional: If Y is negative then constant otherwise X" },
		{ CATEGORY, IF Y Q Z T C H X,	   	LAMBDA(  (y == 0) ? c : x	   ), "Conditional: If Y is zero then constant otherwise X" },
		{ CATEGORY, IF Y G Z T W H Z,	   	LAMBDA(  (y > 0) ? 1 : 0	   ), "Conditional: If Y is positive then 1 otherwise 0" },
		{ CATEGORY, IF Y L Z T W H Z,	   	LAMBDA(  (y < 0) ? 1 : 0	   ), "Conditional: If Y is negative then 1 otherwise 0" },
		{ CATEGORY, IF Y Q Z T W H Z,	   	LAMBDA(  (y == 0) ? 1 : 0	   ), "Conditional: If Y is zero then 1 otherwise 0" },
		{ CATEGORY, IF Y G Z T Y H C,	   	LAMBDA(  (y > 0) ? y : c	   ), "Conditional: If Y is positive then Y otherwise constant" },
		{ CATEGORY, IF Y L Z T Y H C,	   	LAMBDA(  (y < 0) ? y : c	   ), "Conditional: If Y is negative then Y otherwise constant" },
		{ CATEGORY, IF Y Q Z T Y H C,	   	LAMBDA(  (y == 0) ? y : c	   ), "Conditional: If Y is zero then Y otherwise constant" },
		{ CATEGORY, IF Y G Z T C H Y,	   	LAMBDA(  (y > 0) ? c : y	   ), "Conditional: If Y is positive then constant otherwise Y" },
		{ CATEGORY, IF Y L Z T C H Y,	   	LAMBDA(  (y < 0) ? c : y	   ), "Conditional: If Y is negative then constant otherwise Y" },
		{ CATEGORY, IF Y Q Z T C H Y,	   	LAMBDA(  (y == 0) ? c : y	   ), "Conditional: If Y is zero then constant otherwise Y" },
#undef CATEGORY
#define CATEGORY 17
		{ CATEGORY, IF X G Y T C H Z,	   	LAMBDA(  (x > y) ? c : 0	   ), "Conditional: If X is greater than Y then constant otherwise 0" }, // Conditional X and Y
		{ CATEGORY, IF X L Y T C H Z,        	LAMBDA(  (x < y) ? c : 0	   ), "Conditional: If X is less than Y then constant otherwise 0" },
		{ CATEGORY, IF X Q Y T C H Z,	   	LAMBDA(  (x == y) ? c : 0	   ), "Conditional: If X is equal to Y then constant otherwise 0" },
		{ CATEGORY, IF Y G X T C H Z,	   	LAMBDA(  (y > x) ? c : 0	   ), "Conditional: If Y is greater than X then constant otherwise 0" },
		{ CATEGORY, IF Y L X T C H Z,        	LAMBDA(  (y < x) ? c : 0	   ), "Conditional: If Y is less than X then constant otherwise 0" },
		{ CATEGORY, IF X G Y T X H Z,	   	LAMBDA(  (x > y) ? x : 0	   ), "Conditional: If X is greater than Y then X otherwise 0" },
		{ CATEGORY, IF X L Y T X H Z,        	LAMBDA(  (x < y) ? x : 0	   ), "Conditional: If X is less than Y then X otherwise 0" },
		{ CATEGORY, IF X Q Y T X H Z,	   	LAMBDA(  (x == y) ? x : 0	   ), "Conditional: If X is equal to Y then X otherwise 0" },
		{ CATEGORY, IF Y G X T X H Z,	   	LAMBDA(  (y > x) ? x : 0	   ), "Conditional: If Y is greater than X then X otherwise 0" },
		{ CATEGORY, IF Y L X T X H Z,        	LAMBDA(  (y < x) ? x : 0	   ), "Conditional: If Y is less than X then X otherwise 0" },
		{ CATEGORY, IF X G Y T Y H Z,	   	LAMBDA(  (x > y) ? y : 0	   ), "Conditional: If X is greater than Y then Y otherwise 0" },
		{ CATEGORY, IF X L Y T Y H Z,        	LAMBDA(  (x < y) ? y : 0	   ), "Conditional: If X is less than Y then Y otherwise 0" },
		{ CATEGORY, IF X Q Y T Y H Z,	   	LAMBDA(  (x == y) ? y : 0	   ), "Conditional: If X is equal to Y then Y otherwise 0" },
		{ CATEGORY, IF Y G X T Y H Z,	   	LAMBDA(  (y > x) ? y : 0	   ), "Conditional: If Y is greater than X then Y otherwise 0" },
		{ CATEGORY, IF Y L X T Y H Z,        	LAMBDA(  (y < x) ? y : 0	   ), "Conditional: If Y is less than X then Y otherwise 0" },
#undef CATEGORY
#define CATEGORY 18
		{ CATEGORY, IF X G C T Y H Z,	   	LAMBDA(  (x > c) ? y : 0	   ), "Conditional: If X is greater than constant then Y otherwise 0" }, // Conditional X and C
		{ CATEGORY, IF X L C T Y H Z,        	LAMBDA(  (x < c) ? y : 0	   ), "Conditional: If X is less than constant then Y otherwise 0" },
		{ CATEGORY, IF X Q C T Y H Z,	   	LAMBDA(  (x == c) ? y : 0	   ), "Conditional: If X is equal to constant then Y otherwise 0" },
		{ CATEGORY, IF C G X T Y H Z,	   	LAMBDA(  (c > x) ? y : 0	   ), "Conditional: If constant is greater than X then Y otherwise 0" },
		{ CATEGORY, IF C L X T Y H Z,        	LAMBDA(  (c < x) ? y : 0	   ), "Conditional: If constant is less than X then Y otherwise 0" },
		{ CATEGORY, IF X G C T X H Z,	   	LAMBDA(  (x > c) ? x : 0	   ), "Conditional: If X is greater than constant then X otherwise 0" },
		{ CATEGORY, IF X L C T X H Z,        	LAMBDA(  (x < c) ? x : 0	   ), "Conditional: If X is less than constant then X otherwise 0" },
		{ CATEGORY, IF X Q C T X H Z,	   	LAMBDA(  (x == c) ? x : 0	   ), "Conditional: If X is equal to constant then X otherwise 0" },
		{ CATEGORY, IF C G X T X H Z,	   	LAMBDA(  (c > x) ? x : 0	   ), "Conditional: If constant is greater than X then X otherwise 0" },
		{ CATEGORY, IF C L X T X H Z,        	LAMBDA(  (c < x) ? x : 0	   ), "Conditional: If constant is less than X then X otherwise 0" },
		{ CATEGORY, IF X G C T X H Y,	   	LAMBDA(  (x > c) ? x : y	   ), "Conditional: If X is greater than constant then X otherwise Y" },
		{ CATEGORY, IF X L C T X H Y,        	LAMBDA(  (x < c) ? x : y	   ), "Conditional: If X is less than constant then X otherwise Y" },
		{ CATEGORY, IF X Q C T X H Y,	   	LAMBDA(  (x == c) ? x : y	   ), "Conditional: If X is equal to constant then X otherwise Y" },
		{ CATEGORY, IF C G X T X H Y,	   	LAMBDA(  (c > x) ? x : y	   ), "Conditional: If constant is greater than X then X otherwise Y" },
		{ CATEGORY, IF C L X T X H Y,        	LAMBDA(  (c < x) ? x : y	   ), "Conditional: If constant is less than X then X otherwise Y" },
#undef CATEGORY
#define CATEGORY 19
		{ CATEGORY, IF Y G C T X H Z,	   	LAMBDA(  (y > c) ? x : 0	   ), "Conditional: If Y is greater than constant then X otherwise 0" },	// Conditional Y and C
		{ CATEGORY, IF Y L C T X H Z,        	LAMBDA(  (y < c) ? x : 0	   ), "Conditional: If Y is less than constant then X otherwise 0" },
		{ CATEGORY, IF Y Q C T X H Z,	   	LAMBDA(  (y == c) ? x : 0	   ), "Conditional: If Y is equal to constant then X otherwise 0" },
		{ CATEGORY, IF C G Y T X H Z,	   	LAMBDA(  (c > y) ? x : 0	   ), "Conditional: If constant is greater than Y then X otherwise 0" },
		{ CATEGORY, IF C L Y T X H Z,        	LAMBDA(  (c < y) ? x : 0	   ), "Conditional: If constant is less than Y then X otherwise 0" },
		{ CATEGORY, IF Y G C T Y H Z,	   	LAMBDA(  (y > c) ? y : 0	   ), "Conditional: If Y is greater than constant then Y otherwise 0" },
		{ CATEGORY, IF Y L C T Y H Z,        	LAMBDA(  (y < c) ? y : 0	   ), "Conditional: If Y is less than constant then Y otherwise 0" },
		{ CATEGORY, IF Y Q C T Y H Z,	   	LAMBDA(  (y == c) ? y : 0	   ), "Conditional: If Y is equal to constant then Y otherwise 0" },
		{ CATEGORY, IF C G Y T Y H Z,	   	LAMBDA(  (c > y) ? y : 0	   ), "Conditional: If constant is greater than Y then Y otherwise 0" },
		{ CATEGORY, IF C L Y T Y H Z,        	LAMBDA(  (c < y) ? y : 0	   ), "Conditional: If constant is less than Y then Y otherwise 0" },
		{ CATEGORY, IF Y G C T Y H X,	   	LAMBDA(  (y > c) ? y : x	   ), "Conditional: If Y is greater than constant then Y otherwise X" },
		{ CATEGORY, IF Y L C T Y H X,        	LAMBDA(  (y < c) ? y : x	   ), "Conditional: If Y is less than constant then Y otherwise X" },
		{ CATEGORY, IF Y Q C T Y H X,	   	LAMBDA(  (y == c) ? y : x	   ), "Conditional: If Y is equal to constant then Y otherwise X" },
		{ CATEGORY, IF C G Y T Y H X,	   	LAMBDA(  (c > y) ? y : x	   ), "Conditional: If constant is greater than Y then Y otherwise X" },
		{ CATEGORY, IF C L Y T Y H X,        	LAMBDA(  (c < y) ? y : x	   ), "Conditional: If constant is less than Y then Y otherwise X" },
#undef CATEGORY
#define CATEGORY 20
		{ CATEGORY, LF X A C RF, 		LAMBDA(  std::floor(x + c)         ), "Rounding: Round down X and C" },
		{ CATEGORY, LF Y A C RF, 		LAMBDA(  std::floor(y + c)         ), "Rounding: Round down Y and C" },
		{ CATEGORY, LF X A Y A C RF, 		LAMBDA(  std::floor(x + y + c)     ), "Rounding: Round down X and Y and C" },
		{ CATEGORY, LF X M C RF, 		LAMBDA(  std::floor(x * c)         ), "Rounding: Round down X by C" },
		{ CATEGORY, LF Y M C RF, 		LAMBDA(  std::floor(y * c)         ), "Rounding: Round down Y by C" },
		{ CATEGORY, LF X M Y C RF, 		LAMBDA(  std::floor(x * y * c)     ), "Rounding: Round down X by Y by C" },
		{ CATEGORY, LC X A C RC, 		LAMBDA(  std::ceil(x + c)          ), "Rounding: Round up X and C" },
		{ CATEGORY, LC Y A C RC, 		LAMBDA(  std::ceil(y + c)          ), "Rounding: Round up Y and C" },
		{ CATEGORY, LC X A Y A C RC, 		LAMBDA(  std::ceil(x + y + c)      ), "Rounding: Round up X and Y and C" },
		{ CATEGORY, LC X M C RC, 		LAMBDA(  std::ceil(x * c)          ), "Rounding: Round up X by C" },
		{ CATEGORY, LC Y M C RC, 		LAMBDA(  std::ceil(y * c)          ), "Rounding: Round up Y by C" },
		{ CATEGORY, LC X M Y C RC, 		LAMBDA(  std::ceil(x * y * c)      ), "Rounding: Round up X by Y by C" },
	};	

#undef X
#undef Y
#undef C
#undef A
#undef S
#undef O
#undef OP
#undef CP
#undef P
#undef M
#undef D
#undef R
#undef S2
#undef S3
#undef s0
#undef s1
#undef s2
#undef E
#undef SA
#undef SX
#undef SY
#undef SC
#undef COMMA
#undef MIN
#undef MAX
#undef SIN
#undef COS
#undef TAN
#undef ASIN
#undef ACOS
#undef ATAN
#undef LOG
#undef LOG2
#undef LOG10
#undef IF
#undef G
#undef L
#undef Q
#undef Z
#undef W
#undef T
#undef H
#undef Pi
#undef TAU
#undef F
#undef LAM
#undef LC
#undef RC
#undef LF
#undef RF

	struct AOFuncDisplay : Knob {
		Module *module;
		int index;
		AOFuncDisplay() {
			box.size.x = 80;
			box.size.y = 15;
			snap = true;
			smooth = false;
			speed = 0.5f;
		}
		void onButton(const event::Button &e) override;
		void drawLayer(const DrawArgs &args, int layer) override {
			if ((layer) == 1 && module) {
				nvgFontSize(args.vg, 16);
				nvgFontFaceId(args.vg, gScheme.font()->handle);
				nvgFillColor(args.vg, SUBLIGHTBLUE);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
				nvgText(args.vg, 41.5, 13, functions[getParamQuantity()->getValue()].name.c_str(), NULL);
			}
			Widget::drawLayer(args, layer);
		}
	};

	struct AOConstDisplay : Knob {
		Module *module;
		int index;
		AOConstDisplay() {
			box.size.x = 80;
			box.size.y = 15;
			snap = true;
			speed = 0.005;
		}
		void drawLayer(const DrawArgs &args, int layer) override {
			if ((layer == 1) && module) {
				char mtext[41];
				sprintf(mtext, "C=%4.2f", ((int)(getParamQuantity()->getValue()))/100.0f);
				nvgFontSize(args.vg, 16);
				nvgFontFaceId(args.vg, gScheme.font()->handle);
				nvgFillColor(args.vg, SUBLIGHTBLUE);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
				nvgText(args.vg, 41.5, 13, mtext, NULL);
			}
			Widget::drawLayer(args, layer);
		}
	};

	std::vector<std::string> AODescriptions;

	std::vector<std::string> AOGetDescriptions() {
		if (AODescriptions.size() == 0) {
			for (unsigned int i = 0; i < functions.size(); i++) {
				AODescriptions.push_back(functions[i].description);
			}
		}
		return AODescriptions;
	}

} // end namespace

template <unsigned int x, unsigned int y>
struct AO1 : Module {
	enum ParamIds {
		PARAM_FUNC_1,
		PARAM_CONST_1 = x * y,
		NUM_PARAMS = 2 * x * y
	};
	enum InputIds {
		INPUT_X_1,
		INPUT_Y_1 = x,
		NUM_INPUTS = x + y
	};
	enum OutputIds {
		OUTPUT_X_1,
		OUTPUT_Y_1 = x,
		NUM_OUTPUTS = x + y
	};
	enum LightIds {
		NUM_LIGHTS
};

	AO1() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		std::vector<std::string> descriptions = AOGetDescriptions();
		for (unsigned int ix = 0; ix < x; ix++) {
			for (unsigned int iy = 0; iy < y; iy++) {
				configSwitch(PARAM_FUNC_1 + ix + iy * x, 0.0f, functions.size() - 1.0f, 0.0f, "Algorithm", descriptions );
				configParam(PARAM_CONST_1 + ix + iy * x, -10000.0f, 10000.0f, 0.0f, "Constant", "", 0.f, 0.01f);
				
			}
			configInput(INPUT_X_1 + ix, string::f("Signal X%d", ix + 1));
			configOutput(OUTPUT_X_1 + ix, string::f("Signal X%d", ix + 1));		
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			configInput(INPUT_Y_1 + iy, string::f("Signal Y%d", iy + 1));
			configOutput(OUTPUT_Y_1 + iy, string::f("Signal Y%d", iy + 1));		
		}
	}
	void process(const ProcessArgs &args) override {
		float vx[x];
		for (unsigned int ix = 0; ix < x; ix++) {
			vx[ix] = inputs[INPUT_X_1 + ix].getVoltageSum();
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			float vy = inputs[INPUT_Y_1 + iy].getVoltageSum();
			for (unsigned int ix = 0; ix < x; ix++) {
				unsigned int f = params[PARAM_FUNC_1 + ix + iy * x].getValue();
				if (f >= functions.size())
					f = functions.size() - 1;
				if (f > 0)
					vy = vx[ix] = functions[f].func(vx[ix], vy, ((int)params[PARAM_CONST_1 + ix + iy * x].getValue())/100.0f);
				// if f is equal to 0, then both x and y pass (crossing) through the module unchanged.
			}
			outputs[OUTPUT_Y_1 + iy].setVoltage(std::isfinite(vy)?vy:0.0f);
		}
		for (unsigned int ix = 0; ix < x; ix++) {
			outputs[OUTPUT_X_1 + ix].setVoltage(std::isfinite(vx[ix])?vx[ix]:0.0f);
		}
	}
};

namespace {

	struct AlgorithmMenu : MenuItem {
		Module *module;
		int index;
		AOFuncDisplay *widget;
		unsigned int algorithm;
		void onAction(const event::Action &e) override;
	};

	struct CategoryMenu : MenuItem {
		Module *module;
		int index;
		AOFuncDisplay *widget;
		unsigned int category;
		Menu *createChildMenu() override {
			Menu *menu = new Menu();
			for (unsigned int i = 1; i < functions.size(); i++) {
				if (functions[i].category == category) {
					AlgorithmMenu *am = new AlgorithmMenu();
					am->module = module;
					am->index = index;
					am->widget = widget;
					am->algorithm = i;
					am->text = functions[i].name;
					menu->addChild(am);
				}
			}
			return menu;
		}
	};

	struct FCopyMenu : MenuItem {
		Module *module;
		int index;
		AOFuncDisplay *widget;
		void onAction(const event::Action &e) override {
			FunctorClipboard = widget->getParamQuantity()->getValue();
		}
	};
	
	struct FPasteMenu : MenuItem {
		Module *module;
		int index;
		AOFuncDisplay *widget;
		void onAction(const event::Action &e) override {
			if (!std::isnan(FunctorClipboard))
				widget->getParamQuantity()->setValue(FunctorClipboard);
		}
	};

	void AOFuncDisplay::onButton(const event::Button &e) {
		if (module) {
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
				e.consume((Knob *)this);
				Menu *menu = createMenu();
				FCopyMenu *cm = new FCopyMenu();
				cm->module = module;
				cm->index = index;
				cm->widget = this;
				cm->text = "Copy";
				menu->addChild(cm);
				if (!std::isnan(FunctorClipboard)) {
					FPasteMenu *pm = new FPasteMenu();
					pm->module = module;
					pm->index = index;
					pm->widget = this;
					pm->text = "Paste";
					menu->addChild(pm);
				}
				menu->addChild(new MenuEntry);
				
				AlgorithmMenu *item = new AlgorithmMenu();
				item->module = module;
				item->index = index;
				item->widget = this;
				item->algorithm = 0;
				item->text = categories[0];
				menu->addChild(item);	
				for (unsigned int i = 1; i < categories.size(); i++) {
					CategoryMenu *cm = new CategoryMenu();
					cm->module = module;
					cm->index = index;
					cm->widget = this;
					cm->category = i;
					cm->text = categories[i];
					cm->rightText = SUBMENU;
					menu->addChild(cm);
				}
				return;
			}
		}
		Knob::onButton(e);
	}
	
	void AlgorithmMenu::onAction(const event::Action &e) {
		widget->getParamQuantity()->setValue(algorithm);
	}
	
} // end namespace

template <unsigned int x, unsigned int y>
struct AOWidget : SchemeModuleWidget {
	AOWidget(AO1<x,y> *module) {
		setModule(module);
		this->box.size = Vec(y * 90 + 75, 380);
		addChild(new SchemePanel(this->box.size));
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(createInputCentered<SilverPort>(Vec(16.5, 73.5 + ix * 46), module, AO1<x,y>::INPUT_X_1 + ix));
			addOutput(createOutputCentered<SilverPort>(Vec(58.5 + y * 90, 73.5 + ix * 46), module, AO1<x,y>::OUTPUT_X_1 + ix));
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			addInput(createInputCentered<SilverPort>(Vec(82.5 + 90 * iy, 31.5), module, AO1<x,y>::INPUT_Y_1 + iy));
			addOutput(createOutputCentered<SilverPort>(Vec(82.5 + 90 * iy, 347.5), module, AO1<x,y>::OUTPUT_Y_1 + iy));
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			for (unsigned int ix = 0; ix < x; ix++) {
				AOFuncDisplay *fd = createParam<AOFuncDisplay>(Vec(42.5 + 90 * iy, 59 + 46 * ix), module, AO1<x,y>::PARAM_FUNC_1 + ix + iy * x);
				fd->module = module;
				fd->index = AO1<x,y>::PARAM_FUNC_1 + ix + iy * x;
				addParam(fd);
				AOConstDisplay *cd = createParam<AOConstDisplay>(Vec(42.5 + 90 * iy, 78 + 46 * ix), module, AO1<x,y>::PARAM_CONST_1 + ix + iy * x);
				cd->module = module;
				cd->index = AO1<x,y>::PARAM_CONST_1 + ix + iy * x;
				addParam(cd);
			}
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		char workingSpace[10];
		snprintf(workingSpace, 10, "AO-1%02d", x * y);
		drawBase(vg, workingSpace);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		for (unsigned int iy = 0; iy < 7; iy++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 37.5, 51.5 + iy * 46);
			for (unsigned int ix = 0; ix < y; ix++) {
				nvgLineTo(vg, 77.5 + ix * 90, 51.5 + iy * 46);
				nvgLineTo(vg, 82.5 + ix * 90, 55.5 + iy * 46);
				nvgLineTo(vg, 87.5 + ix * 90, 51.5 + iy * 46);
				nvgLineTo(vg, 127.5 + ix * 90, 51.5 + iy * 46);
			}
			nvgStroke(vg);
		}
		for (unsigned int ix = 0; ix < y + 1; ix++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 37.5 + ix * 90, 51.5);
			for (unsigned int iy = 0; iy < x; iy++) {
				nvgLineTo(vg, 37.5 + ix * 90, 69.5 + iy * 46);
				nvgLineTo(vg, 41.5 + ix * 90, 74.5 + iy * 46);
				nvgLineTo(vg, 37.5 + ix * 90, 79.5 + iy * 46);
				nvgLineTo(vg, 37.5 + ix * 90, 97.5 + iy * 46);
			}
			nvgStroke(vg);
		}
		for (unsigned int iy = 0; iy < x - 1; iy++) {
			drawText(vg, 16.5, 100 + iy * 46, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "X IN");
			drawText(vg, 58.5 + y * 90, 100 + iy * 46, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "X OUT");
		}
		if (y > 1) {
			for (unsigned int ix = 0; ix < y - 1; ix++) {
				drawText(vg, 127.5 + ix * 90, 34, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "Y IN");
				drawText(vg, 127.5 + ix * 90, 351, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "Y OUT");
			}
		}
		else {
			drawText(vg, 97, 34, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "Y IN");
			drawText(vg, 97, 351, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "Y OUT");
		}
		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		for (unsigned int iy = 0; iy < 6; iy++) {
			for (unsigned int ix = 0; ix < y; ix++) {
				nvgRoundedRect(vg, 42.5 + 90 * ix, 59 + 46 * iy, 83, 16, 2);
				nvgRoundedRect(vg, 42.5 + 90 * ix, 78 + 46 * iy, 83, 16, 2);
			}
		}
		nvgFill(vg);
	}
};

Model *modelAO106 = createModel<AO1<6,1>, AOWidget<6,1>>("A0-106");
Model *modelAO112 = createModel<AO1<6,2>, AOWidget<6,2>>("A0-112");
Model *modelAO118 = createModel<AO1<6,3>, AOWidget<6,3>>("A0-118");
Model *modelAO124 = createModel<AO1<6,4>, AOWidget<6,4>>("A0-124");
Model *modelAO136 = createModel<AO1<6,6>, AOWidget<6,6>>("A0-136");
