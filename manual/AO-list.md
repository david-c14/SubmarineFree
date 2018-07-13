#AO-1xx Algorithms

## Arithmetical
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| | ||
| C | c ||
| X+C | x + c ||
| Y+C | y + c ||
| X+Y+C | x + y + c ||
| C-X | c - x ||
| C-Y | c - y ||
| X-(Y+C) | x - ( y + c ) ||
| (X+C)-Y | ( x + c ) - y ||
| Y-(X+C) | y - ( x + c ) ||
| (Y+C)-X | ( y + c ) - x ||
| (X&#x2a2f;Y)+C | ( x * y ) + c ||
| (X+C)&#x2a2f;Y | ( x + c ) * y ||
| X&#x2a2f;(Y+C) | x * ( y + c ) ||
| X&#x2a2f;C | x * c ||
| Y&#x2a2f;C | y * c ||
| X&#x2a2f;Y&#x2a2f;C | x * y * c ||
| &#x3c0;&#x2a2f;(X+C) | M_PI * ( x + c ) ||
| &#x3c0;&#x2a2f;(Y+C) | M_PI * ( y + c ) ||
| &#x3c4;&#x2a2f;(X+C) | 2 * M_PI * ( x + c ) ||
| &#x3c4;&#x2a2f;(Y+C) | 2 * M_PI * ( y + c ) ||
| X&#xf7;C | x / c ||
| C&#xf7;X | c / x ||
| Y&#xf7;C | y / c ||
| C&#xf7;Y | c / y ||
| C+(X&#xf7;Y) | c + ( x / y ) ||
| C+(Y&#xf7;X) | c + ( y / x ) ||
| X+(Y&#xf7;C) | x + ( y / c ) ||
| X+(C&#xf7;Y) | x + ( c / y ) ||
| Y+(X&#xf7;C) | y + ( x / c ) ||
| Y+(C&#xf7;X) | y + ( c / x ) ||
| (X+C)&#xf7;Y | ( x + c ) / y ||
| X&#xf7;(Y+C) | x / ( y + c ) ||
| (Y+C)&#xf7;X | ( y + c ) / x ||
| Y&#xf7;(X+C) | y / ( x + c ) ||
## Modular
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| (X+C)%Y | fmodf( x + c , y ) ||
| (Y+C)%X | fmodf( y + c , x ) ||
| X%(Y+C) | fmodf( x , y + c ) ||
| Y%(X+C) | fmodf( y , x + c) ||
| X%C | fmodf( x , c ) ||
| Y%C | fmodf( y , c ) ||
## Quadratic
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| X&#xb2;+C | x * x + c ||
| Y&#xb2;+C | y * y + c ||
| (X+C)&#xb2; | ( x + c ) * ( x + c ) ||
| (Y+C)&#xb2; | ( y + c ) * ( y + c ) ||
| X&#xb2;+Y+C | x * x + y + c ||
| Y&#xb2;+X+C | y * y + x + c ||
| X&#xb2;+CY | x * x + c * y ||
| Y&#xb2;+CX | y * y + c * x ||
## Powers
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| &#x221a;(X+C) | sqrt( x + c ) ||
| &#x221a;(Y+C) | sqrt( y + c ) ||
| C&#x2e3; | powf( c , x ) ||
| C&#x2b8; | powf( c , y ) ||
| C&#x2e3;&#x207a;&#x2b8; | powf( c , x + y ) ||
| C&#x2e3;&#x2b8; | powf( c , x * y ) ||
| X&#x1D9c; | powf( x , c ) ||
| Y&#x1D9c; | powf( y , c ) ||
| X&#x2b8;&#x207a;&#x1D9c; | powf( x , y + c ) ||
| Y&#x2e3;&#x207a;&#x1D9c; | powf( y , x + c ) ||
| X&#x1D9c;&#x2b8; | powf( x , c * y ) ||
| Y&#x1D9c;&#x2e3; | powf( y , c * x ) ||
## Positive values only
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| &#124;X+C&#124; | abs( x + c ) ||
| &#124;Y+C&#124; | abs( y + c ) ||
## Maximum and Minimum
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| min(X+C,Y) | min( x + c, y ) ||
| min(X,C) | min( x, c ) ||
| min(Y,C) | min( y, c ) ||
| max(X+C,Y) | max( x + c, y ) ||
| max(X,C) | max( x, c ) ||
| max(Y,C) | max( y, c ) ||
## Trigonometric
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| sin(X+C) | sin( x + c ) ||
| sin(Y+C) | sin( y + c ) ||
| sin(X+Y) | sin( x + y ) ||
| sin(CX) | sin( c * x ) ||
| sin(CY) | sin( c * y ) ||
| sin(XY) | sin( x * y ) ||
| cos(X+C) | cos( x + c ) ||
| cos(Y+C) | cos( y + c ) ||
| cos(X+Y) | cos( x + y ) ||
| cos(CX) | cos( c * x ) ||
| cos(CY) | cos( c * y ) ||
| cos(XY) | cos( x * y ) ||
| tan(X+C) | tan( x + c ) ||
| tan(Y+C) | tan( y + c ) ||
| tan(X+Y) | tan( x + y ) ||
| tan(CX) | tan( c * x ) ||
| tan(CY) | tan( c * y ) ||
| tan(XY) | tan( x * y ) ||
| asin(X+C) | asin( x + c ) ||
| asin(Y+C) | asin( y + c ) ||
| asin(X+Y) | asin( x + y ) ||
| asin(CX) | asin( c * x ) ||
| asin(CY) | asin( c * y ) ||
| asin(XY) | asin( x * y ) ||
| acos(X+C) | acos( x + c ) ||
| acos(Y+C) | acos( y + c ) ||
| acos(X+Y) | acos( x + y ) ||
| acos(CX) | acos( c * x ) ||
| acos(CY) | acos( c * y ) ||
| acos(XY) | acos( x * y ) ||
| atan(X+C) | atan( x + c ) ||
| atan(Y+C) | atan( y + c ) ||
| atan(X+Y) | atan( x + y ) ||
| atan(CX) | atan( c * x ) ||
| atan(CY) | atan( c * y ) ||
| atan(XY) | atan( x * y ) ||
## Logarithmic
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| log(X+C) | log( x + c ) ||
| log(Y+C) | log( y + c ) ||
| log&#x2082;(X+C) | log2( x + c ) ||
| log&#x2082;(Y+C) | log2( y + c ) ||
| log&#x2081;&#x2080;(X+C) | log10( x + c ) ||
| log&#x2081;&#x2080;(Y+C) | log10( y + c ) ||
## Exponential
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| &#x212f;&#x2e3;&#x207a;&#x1D9c; | exp( x + c ) ||
| &#x212f;&#x2b8;&#x207a;&#x1D9c; | exp( y + c ) ||
| &#x212f;&#x1D9c;&#x2e3; | exp( c * x ) ||
| &#x212f;&#x1D9c;&#x2b8; | exp( c * y ) ||
| 2&#x2e3;&#x207a;&#x1D9c; | powf( 2, x + c ) ||
| 2&#x2b8;&#x207a;&#x1D9c; | powf( 2, y + c ) ||
| 2&#x1D9c;&#x2e3; | powf( 2, c * x ) ||
| 2&#x1D9c;&#x2b8; | powf( 2, c * y ) ||
| 10&#x2e3;&#x207a;&#x1D9c; | powf( 10, x + c ) ||
| 10&#x2b8;&#x207a;&#x1D9c; | powf( 10, y + c ) ||
| 10&#x1D9c;&#x2e3; | powf( 10, c * x ) ||
| 10&#x1D9c;&#x2b8; | powf( 10, c * y ) ||
## Conditional
| Display                  | Code               | Description          |
| ------------------------ | ------------------ | -------------------- |
| if X>0&#x21a3;Y/C | (x > 0) ? y : c ||
| if X<0&#x21a3;Y/C | (x < 0) ? y : c ||
| if X=0&#x21a3;Y/C | (x == 0) ? y : c ||
| if X>0&#x21a3;C/Y | (x > 0) ? c : y ||
| if X<0&#x21a3;C/Y | (x < 0) ? c : y ||
| if X=0&#x21a3;C/Y | (x == 0) ? c : y ||
| if X>0&#x21a3;1/0 | (x > 0) ? 1 : 0 ||
| if X<0&#x21a3;1/0 | (x < 0) ? 1 : 0 ||
| if X=0&#x21a3;1/0 | (x == 0) ? 1 : 0 ||
| if X>0&#x21a3;X/C | (x > 0) ? x : c ||
| if X<0&#x21a3;X/C | (x < 0) ? x : c ||
| if X=0&#x21a3;X/C | (x == 0) ? x : c ||
| if X>0&#x21a3;C/X | (x > 0) ? c : x ||
| if X<0&#x21a3;C/X | (x < 0) ? c : x ||
| if X=0&#x21a3;C/X | (x == 0) ? c : x ||
| if Y>0&#x21a3;X/C | (y > 0) ? x : c ||
| if Y<0&#x21a3;X/C | (y < 0) ? x : c ||
| if Y=0&#x21a3;X/C | (y == 0) ? x : c ||
| if Y>0&#x21a3;C/X | (y > 0) ? c : x ||
| if Y<0&#x21a3;C/X | (y < 0) ? c : x ||
| if Y=0&#x21a3;C/X | (y == 0) ? c : x ||
| if Y>0&#x21a3;1/0 | (y > 0) ? 1 : 0 ||
| if Y<0&#x21a3;1/0 | (y < 0) ? 1 : 0 ||
| if Y=0&#x21a3;1/0 | (y == 0) ? 1 : 0 ||
| if Y>0&#x21a3;Y/C | (y > 0) ? y : c ||
| if Y<0&#x21a3;Y/C | (y < 0) ? y : c ||
| if Y=0&#x21a3;Y/C | (y == 0) ? y : c ||
| if Y>0&#x21a3;C/Y | (y > 0) ? c : y ||
| if Y<0&#x21a3;C/Y | (y < 0) ? c : y ||
| if Y=0&#x21a3;C/Y | (y == 0) ? c : y ||
| if X>Y&#x21a3;C/0 | (x > y) ? c : 0 ||
| if X<Y&#x21a3;C/0 | (x < y) ? c : 0 ||
| if X=Y&#x21a3;C/0 | (x == y) ? c : 0 ||
| if Y>X&#x21a3;C/0 | (y > x) ? c : 0 ||
| if Y<X&#x21a3;C/0 | (y < x) ? c : 0 ||
| if X>Y&#x21a3;X/0 | (x > y) ? x : 0 ||
| if X<Y&#x21a3;X/0 | (x < y) ? x : 0 ||
| if X=Y&#x21a3;X/0 | (x == y) ? x : 0 ||
| if Y>X&#x21a3;X/0 | (y > x) ? x : 0 ||
| if Y<X&#x21a3;X/0 | (y < x) ? x : 0 ||
| if X>Y&#x21a3;Y/0 | (x > y) ? y : 0 ||
| if X<Y&#x21a3;Y/0 | (x < y) ? y : 0 ||
| if X=Y&#x21a3;Y/0 | (x == y) ? y : 0 ||
| if Y>X&#x21a3;Y/0 | (y > x) ? y : 0 ||
| if Y<X&#x21a3;Y/0 | (y < x) ? y : 0 ||
| if X>C&#x21a3;Y/0 | (x > c) ? y : 0 ||
| if X<C&#x21a3;Y/0 | (x < c) ? y : 0 ||
| if X=C&#x21a3;Y/0 | (x == c) ? y : 0 ||
| if C>X&#x21a3;Y/0 | (c > x) ? y : 0 ||
| if C<X&#x21a3;Y/0 | (c < x) ? y : 0 ||
| if X>C&#x21a3;X/0 | (x > c) ? x : 0 ||
| if X<C&#x21a3;X/0 | (x < c) ? x : 0 ||
| if X=C&#x21a3;X/0 | (x == c) ? x : 0 ||
| if C>X&#x21a3;X/0 | (c > x) ? x : 0 ||
| if C<X&#x21a3;X/0 | (c < x) ? x : 0 ||
| if X>C&#x21a3;X/Y | (x > c) ? x : y ||
| if X<C&#x21a3;X/Y | (x < c) ? x : y ||
| if X=C&#x21a3;X/Y | (x == c) ? x : y ||
| if C>X&#x21a3;X/Y | (c > x) ? x : y ||
| if C<X&#x21a3;X/Y | (c < x) ? x : y ||
| if Y>C&#x21a3;X/0 | (y > c) ? x : 0 ||
| if Y<C&#x21a3;X/0 | (y < c) ? x : 0 ||
| if Y=C&#x21a3;X/0 | (y == c) ? x : 0 ||
| if C>Y&#x21a3;X/0 | (c > y) ? x : 0 ||
| if C<Y&#x21a3;X/0 | (c < y) ? x : 0 ||
| if Y>C&#x21a3;Y/0 | (y > c) ? y : 0 ||
| if Y<C&#x21a3;Y/0 | (y < c) ? y : 0 ||
| if Y=C&#x21a3;Y/0 | (y == c) ? y : 0 ||
| if C>Y&#x21a3;Y/0 | (c > y) ? y : 0 ||
| if C<Y&#x21a3;Y/0 | (c < y) ? y : 0 ||
| if Y>C&#x21a3;Y/X | (y > c) ? y : x ||
| if Y<C&#x21a3;Y/X | (y < c) ? y : x ||
| if Y=C&#x21a3;Y/X | (y == c) ? y : x ||
| if C>Y&#x21a3;Y/X | (c > y) ? y : x ||
| if C<Y&#x21a3;Y/X | (c < y) ? y : x ||
