# AO-1xx Algorithms

## Arithmetical

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> </td><td> </td><td> Pass-through. X passes across unchanged, Y passes down unchanged </td></tr>
<tr><td> C </td><td> c </td><td> The value of the selected constant C </td></tr>
<tr><td> X+C </td><td> x + c </td><td> X added to C </td></tr>
<tr><td> Y+C </td><td> y + c </td><td> Y added to C</td></tr>
<tr><td> X+Y+C </td><td> x + y + c </td><td> X and Y added to C </td></tr>
<tr><td> C-X </td><td> c - x </td><td> X subtracted from C </td></tr>
<tr><td> C-Y </td><td> c - y </td><td> Y subtracted from C </td></tr>
<tr><td> X-(Y+C) </td><td> x - ( y + c ) </td><td> Y added to C then all subtracted from X </td></tr>
<tr><td> (X+C)-Y </td><td> ( x + c ) - y </td><td> Y subtracted from X and C </td></tr>
<tr><td> Y-(X+C) </td><td> y - ( x + c ) </td><td> X added to C then all subtracted from Y </td></tr>
<tr><td> (Y+C)-X </td><td> ( y + c ) - x </td><td> X subtracted from Y and C </td></tr>
<tr><td> (X&#x2a2f;Y)+C </td><td> ( x * y ) + c </td><td> X times Y added to C </td></tr>
<tr><td> (X+C)&#x2a2f;Y </td><td> ( x + c ) * y </td><td> X and C multiplied by Y </td></tr>
<tr><td> X&#x2a2f;(Y+C) </td><td> x * ( y + c ) </td><td> Y and C multiplied by X </td></tr>
<tr><td> X&#x2a2f;C </td><td> x * c </td><td> X times C </td></tr>
<tr><td> Y&#x2a2f;C </td><td> y * c </td><td> Y times C </td></tr>
<tr><td> X&#x2a2f;Y&#x2a2f;C </td><td> x * y * c </td><td> X times Y times C</td></tr>
<tr><td> &#x3c0;&#x2a2f;(X+C) </td><td> M_PI * ( x + c ) </td><td> X and C multiplied by pi </td></tr>
<tr><td> &#x3c0;&#x2a2f;(Y+C) </td><td> M_PI * ( y + c ) </td><td> Y and C multiplied by pi </td></tr>
<tr><td> &#x3c4;&#x2a2f;(X+C) </td><td> 2 * M_PI * ( x + c ) </td><td> X and C multiplied by tau </td></tr>
<tr><td> &#x3c4;&#x2a2f;(Y+C) </td><td> 2 * M_PI * ( y + c ) </td><td> Y and C multiplied by tau </td></tr>
<tr><td> X&#xf7;C </td><td> x / c </td><td> X divided by C </td></tr>
<tr><td> C&#xf7;X </td><td> c / x </td><td> C divided by X </td></tr>
<tr><td> Y&#xf7;C </td><td> y / c </td><td> Y divided by C </td></tr>
<tr><td> C&#xf7;Y </td><td> c / y </td><td> C divided by Y </td></tr>
<tr><td> C+(X&#xf7;Y) </td><td> c + ( x / y ) </td><td> C added to X divided by Y </td></tr>
<tr><td> C+(Y&#xf7;X) </td><td> c + ( y / x ) </td><td> C added to Y divided by X </td></tr>
<tr><td> X+(Y&#xf7;C) </td><td> x + ( y / c ) </td><td> X added to Y divided by C </td></tr>
<tr><td> X+(C&#xf7;Y) </td><td> x + ( c / y ) </td><td> X added to C divided by Y </td></tr>
<tr><td> Y+(X&#xf7;C) </td><td> y + ( x / c ) </td><td> Y added to X divided by C </td></tr>
<tr><td> Y+(C&#xf7;X) </td><td> y + ( c / x ) </td><td> Y added to C divided by X </td></tr>
<tr><td> (X+C)&#xf7;Y </td><td> ( x + c ) / y </td><td> X and C divided by Y </td></tr>
<tr><td> X&#xf7;(Y+C) </td><td> x / ( y + c ) </td><td> X divided by Y and C </td></tr>
<tr><td> (Y+C)&#xf7;X </td><td> ( y + c ) / x </td><td> Y and C divided by X </td></tr>
<tr><td> Y&#xf7;(X+C) </td><td> y / ( x + c ) </td><td> Y divided by X and C </td></tr>
</table>

## Modular

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> (X+C)%Y </td><td> fmodf( x + c , y ) </td><td> The remainder of X and C divided by Y </td></tr>
<tr><td> (Y+C)%X </td><td> fmodf( y + c , x ) </td><td> The remainder of Y and C divided by X </td></tr>
<tr><td> X%(Y+C) </td><td> fmodf( x , y + c ) </td><td> The remainder of X divided by Y and C </td></tr>
<tr><td> Y%(X+C) </td><td> fmodf( y , x + c) </td><td> The remainder of Y divided by X and C </td></tr>
<tr><td> X%C </td><td> fmodf( x , c ) </td><td> The remainder of X divided by C </td></tr>
<tr><td> Y%C </td><td> fmodf( y , c ) </td><td> The remainder of Y divided by C </td></tr>
</table>

## Quadratic

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> X&#xb2;+C </td><td> x * x + c </td><td> X squared added to C </td></tr>
<tr><td> Y&#xb2;+C </td><td> y * y + c </td><td> Y squared added to C </td></tr>
<tr><td> (X+C)&#xb2; </td><td> ( x + c ) * ( x + c ) </td><td> X and C squared </td></tr>
<tr><td> (Y+C)&#xb2; </td><td> ( y + c ) * ( y + c ) </td><td> Y and C squared </td></tr>
<tr><td> X&#xb2;+Y+C </td><td> x * x + y + c </td><td> X squared added to Y and C </td></tr>
<tr><td> Y&#xb2;+X+C </td><td> y * y + x + c </td><td> Y squared added to X and C </td></tr>
<tr><td> X&#xb2;+CY </td><td> x * x + c * y </td><td> X squared added to Y multiplied by C </td></tr>
<tr><td> Y&#xb2;+CX </td><td> y * y + c * x </td><td> Y squared added to X multiplied by C </td></tr>
</table>

## Powers

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> &#x221a;(X+C) </td><td> sqrt( x + c ) </td><td> The square root of X and C </td></tr>
<tr><td> &#x221a;(Y+C) </td><td> sqrt( y + c ) </td><td> The square root of Y and C </td></tr>
<tr><td> C&#x2e3; </td><td> powf( c , x ) </td><td> C raised to the power of X </td></tr>
<tr><td> C&#x2b8; </td><td> powf( c , y ) </td><td> C raised to the power of Y </td></tr>
<tr><td> C&#x2e3;&#x207a;&#x2b8; </td><td> powf( c , x + y ) </td><td> C raised to the power of X and Y </td></tr>
<tr><td> C&#x2e3;&#x2b8; </td><td> powf( c , x * y ) </td><td> C raised to the power of X multiplied by Y </td></tr>
<tr><td> X&#x1D9c; </td><td> powf( x , c ) </td><td> X raised to the power of C </td></tr>
<tr><td> Y&#x1D9c; </td><td> powf( y , c ) </td><td> Y raised to the power of C </td></tr>
<tr><td> X&#x2b8;&#x207a;&#x1D9c; </td><td> powf( x , y + c ) </td><td> X raised to the power of Y and C </td></tr>
<tr><td> Y&#x2e3;&#x207a;&#x1D9c; </td><td> powf( y , x + c ) </td><td> Y raised to the power of X and C </td></tr>
<tr><td> X&#x1D9c;&#x2b8; </td><td> powf( x , c * y ) </td><td> X raised to the power of Y multiplied by C </td></tr>
<tr><td> Y&#x1D9c;&#x2e3; </td><td> powf( y , c * x ) </td><td> Y raised to the power of X multiplied by C </td></tr>
</table>

## Positive values only

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> &#124;X+C&#124; </td><td> abs( x + c ) </td><td> X Added to C with any minus sign removed </td></tr>
<tr><td> &#124;Y+C&#124; </td><td> abs( y + c ) </td><td> Y added to C with any minus sign removed </td></tr>
</table>

## Maximum and Minimum

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> min(X+C,Y) </td><td> min( x + c, y ) </td><td> The smaller of X Added to C, or Y </td></tr>
<tr><td> min(X,C) </td><td> min( x, c ) </td><td> The smaller of X or C </td></tr>
<tr><td> min(Y,C) </td><td> min( y, c ) </td><td> The smaller of Y or C </td></tr>
<tr><td> max(X+C,Y) </td><td> max( x + c, y ) </td><td> The larger of X added to C, or Y </td></tr>
<tr><td> max(X,C) </td><td> max( x, c ) </td><td> The larger of X or C </td></tr>
<tr><td> max(Y,C) </td><td> max( y, c ) </td><td> The larger of Y or C </td></tr>
</table>

## Trigonometric

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> sin(X+C) </td><td> sin( x + c ) </td><td> The sine of X and C </td></tr>
<tr><td> sin(Y+C) </td><td> sin( y + c ) </td><td> The sine of Y and C </td></tr>
<tr><td> sin(X+Y) </td><td> sin( x + y ) </td><td> The sine of X and Y </td></tr>
<tr><td> sin(CX) </td><td> sin( c * x ) </td><td> The sine of X mulitplied by C </td></tr>
<tr><td> sin(CY) </td><td> sin( c * y ) </td><td> The sine of Y multiplied by C </td></tr>
<tr><td> sin(XY) </td><td> sin( x * y ) </td><td> The sine of X multiplied by Y </td></tr>
<tr><td> cos(X+C) </td><td> cos( x + c ) </td><td> The cosine of X and C </td></tr>
<tr><td> cos(Y+C) </td><td> cos( y + c ) </td><td></td></tr>
<tr><td> cos(X+Y) </td><td> cos( x + y ) </td><td></td></tr>
<tr><td> cos(CX) </td><td> cos( c * x ) </td><td></td></tr>
<tr><td> cos(CY) </td><td> cos( c * y ) </td><td></td></tr>
<tr><td> cos(XY) </td><td> cos( x * y ) </td><td></td></tr>
<tr><td> tan(X+C) </td><td> tan( x + c ) </td><td> The tangent of X and C </td></tr>
<tr><td> tan(Y+C) </td><td> tan( y + c ) </td><td></td></tr>
<tr><td> tan(X+Y) </td><td> tan( x + y ) </td><td></td></tr>
<tr><td> tan(CX) </td><td> tan( c * x ) </td><td></td></tr>
<tr><td> tan(CY) </td><td> tan( c * y ) </td><td></td></tr>
<tr><td> tan(XY) </td><td> tan( x * y ) </td><td></td></tr>
<tr><td> asin(X+C) </td><td> asin( x + c ) </td><td> The arcsine of X and C </td></tr>
<tr><td> asin(Y+C) </td><td> asin( y + c ) </td><td></td></tr>
<tr><td> asin(X+Y) </td><td> asin( x + y ) </td><td></td></tr>
<tr><td> asin(CX) </td><td> asin( c * x ) </td><td></td></tr>
<tr><td> asin(CY) </td><td> asin( c * y ) </td><td></td></tr>
<tr><td> asin(XY) </td><td> asin( x * y ) </td><td></td></tr>
<tr><td> acos(X+C) </td><td> acos( x + c ) </td><td> The arcosine of X and C </td></tr>
<tr><td> acos(Y+C) </td><td> acos( y + c ) </td><td></td></tr>
<tr><td> acos(X+Y) </td><td> acos( x + y ) </td><td></td></tr>
<tr><td> acos(CX) </td><td> acos( c * x ) </td><td></td></tr>
<tr><td> acos(CY) </td><td> acos( c * y ) </td><td></td></tr>
<tr><td> acos(XY) </td><td> acos( x * y ) </td><td></td></tr>
<tr><td> atan(X+C) </td><td> atan( x + c ) </td><td> The arctangent of X and C </td></tr>
<tr><td> atan(Y+C) </td><td> atan( y + c ) </td><td></td></tr>
<tr><td> atan(X+Y) </td><td> atan( x + y ) </td><td></td></tr>
<tr><td> atan(CX) </td><td> atan( c * x ) </td><td></td></tr>
<tr><td> atan(CY) </td><td> atan( c * y ) </td><td></td></tr>
<tr><td> atan(XY) </td><td> atan( x * y ) </td><td></td></tr>
</table>

## Logarithmic

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> log(X+C) </td><td> log( x + c ) </td><td> The natural logarithm of X and C </td></tr>
<tr><td> log(Y+C) </td><td> log( y + c ) </td><td> The natural logarithm of Y and C </td></tr>
<tr><td> log&#x2082;(X+C) </td><td> log2( x + c ) </td><td> The base-2 logarithm of X and C </td></tr>
<tr><td> log&#x2082;(Y+C) </td><td> log2( y + c ) </td><td> The base-2 logarithm of Y and C </td></tr>
<tr><td> log&#x2081;&#x2080;(X+C) </td><td> log10( x + c ) </td><td> The base-10 logarithm of X and C </td></tr>
<tr><td> log&#x2081;&#x2080;(Y+C) </td><td> log10( y + c ) </td><td> The base-10 logarithm of Y and C </td></tr>
</table>

## Exponential

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> &#x212f;&#x2e3;&#x207a;&#x1D9c; </td><td> exp( x + c ) </td><td> e raised to the power of X and C </td></tr>
<tr><td> &#x212f;&#x2b8;&#x207a;&#x1D9c; </td><td> exp( y + c ) </td><td> e raised to the power of Y and C </td></tr>
<tr><td> &#x212f;&#x1D9c;&#x2e3; </td><td> exp( c * x ) </td><td> e raised to the power of X multiplied by C </td></tr>
<tr><td> &#x212f;&#x1D9c;&#x2b8; </td><td> exp( c * y ) </td><td> e raised to the power of Y multiplied by C </td></tr>
<tr><td> 2&#x2e3;&#x207a;&#x1D9c; </td><td> powf( 2, x + c ) </td><td> 2 raised to the power of X and C </td></tr>
<tr><td> 2&#x2b8;&#x207a;&#x1D9c; </td><td> powf( 2, y + c ) </td><td> 2 raised to the power of Y and C </td></tr>
<tr><td> 2&#x1D9c;&#x2e3; </td><td> powf( 2, c * x ) </td><td> 2 raised to the power of X multiplied by C </td></tr>
<tr><td> 2&#x1D9c;&#x2b8; </td><td> powf( 2, c * y ) </td><td> 2 raised to the power of Y multiplied by C </td></tr>
<tr><td> 10&#x2e3;&#x207a;&#x1D9c; </td><td> powf( 10, x + c ) </td><td> 10 raised to the power of X and C </td></tr>
<tr><td> 10&#x2b8;&#x207a;&#x1D9c; </td><td> powf( 10, y + c ) </td><td> 10 raised to the power of Y and C </td></tr>
<tr><td> 10&#x1D9c;&#x2e3; </td><td> powf( 10, c * x ) </td><td> 10 raised to the power of X multiplied by C </td></tr>
<tr><td> 10&#x1D9c;&#x2b8; </td><td> powf( 10, c * y ) </td><td> 10 raised to the power of Y multiplied by C </td></tr>
</table>

## Conditional

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> if X>0&#x21a3;Y/C </td><td> (x > 0) ? y : c </td><td> Y if X is greater than 0 otherwise C </td></tr>
<tr><td> if X<0&#x21a3;Y/C </td><td> (x < 0) ? y : c </td><td> Y if X is less than 0 otherwise C </td></tr>
<tr><td> if X=0&#x21a3;Y/C </td><td> (x == 0) ? y : c </td><td> Y if X is 0 otherwise C </td></tr>
<tr><td> if X>0&#x21a3;C/Y </td><td> (x > 0) ? c : y </td><td> C if X is greater than 0 otherwise Y </td></tr>
<tr><td> if X<0&#x21a3;C/Y </td><td> (x < 0) ? c : y </td><td> C if X is less that 0 otherwise Y </td></tr>
<tr><td> if X=0&#x21a3;C/Y </td><td> (x == 0) ? c : y </td><td> C if X is 0 otherwise Y </td></tr>
<tr><td> if X>0&#x21a3;1/0 </td><td> (x > 0) ? 1 : 0 </td><td> 1 if X is greater than 0 otherwise 0 </td></tr>
<tr><td> if X<0&#x21a3;1/0 </td><td> (x < 0) ? 1 : 0 </td><td></td></tr>
<tr><td> if X=0&#x21a3;1/0 </td><td> (x == 0) ? 1 : 0 </td><td></td></tr>
<tr><td> if X>0&#x21a3;X/C </td><td> (x > 0) ? x : c </td><td> X if X is greater than 0 otherwise C </td></tr>
<tr><td> if X<0&#x21a3;X/C </td><td> (x < 0) ? x : c </td><td></td></tr>
<tr><td> if X=0&#x21a3;X/C </td><td> (x == 0) ? x : c </td><td></td></tr>
<tr><td> if X>0&#x21a3;C/X </td><td> (x > 0) ? c : x </td><td> C if X is greater than 0 otherwise X </td></tr>
<tr><td> if X<0&#x21a3;C/X </td><td> (x < 0) ? c : x </td><td></td></tr>
<tr><td> if X=0&#x21a3;C/X </td><td> (x == 0) ? c : x </td><td></td></tr>
<tr><td> if Y>0&#x21a3;X/C </td><td> (y > 0) ? x : c </td><td> X if Y is greater than 0 otherwise C </td></tr>
<tr><td> if Y<0&#x21a3;X/C </td><td> (y < 0) ? x : c </td><td></td></tr>
<tr><td> if Y=0&#x21a3;X/C </td><td> (y == 0) ? x : c </td><td></td></tr>
<tr><td> if Y>0&#x21a3;C/X </td><td> (y > 0) ? c : x </td><td> C if Y is greater than 0 otherwise X </td></tr>
<tr><td> if Y<0&#x21a3;C/X </td><td> (y < 0) ? c : x </td><td></td></tr>
<tr><td> if Y=0&#x21a3;C/X </td><td> (y == 0) ? c : x </td><td></td></tr>
<tr><td> if Y>0&#x21a3;1/0 </td><td> (y > 0) ? 1 : 0 </td><td></td></tr>
<tr><td> if Y<0&#x21a3;1/0 </td><td> (y < 0) ? 1 : 0 </td><td></td></tr>
<tr><td> if Y=0&#x21a3;1/0 </td><td> (y == 0) ? 1 : 0 </td><td></td></tr>
<tr><td> if Y>0&#x21a3;Y/C </td><td> (y > 0) ? y : c </td><td></td></tr>
<tr><td> if Y<0&#x21a3;Y/C </td><td> (y < 0) ? y : c </td><td></td></tr>
<tr><td> if Y=0&#x21a3;Y/C </td><td> (y == 0) ? y : c </td><td></td></tr>
<tr><td> if Y>0&#x21a3;C/Y </td><td> (y > 0) ? c : y </td><td></td></tr>
<tr><td> if Y<0&#x21a3;C/Y </td><td> (y < 0) ? c : y </td><td></td></tr>
<tr><td> if Y=0&#x21a3;C/Y </td><td> (y == 0) ? c : y </td><td></td></tr>
<tr><td> if X>Y&#x21a3;C/0 </td><td> (x > y) ? c : 0 </td><td> C if X is greater than Y otherwise 0 </td></tr>
<tr><td> if X<Y&#x21a3;C/0 </td><td> (x < y) ? c : 0 </td><td></td></tr>
<tr><td> if X=Y&#x21a3;C/0 </td><td> (x == y) ? c : 0 </td><td></td></tr>
<tr><td> if Y>X&#x21a3;C/0 </td><td> (y > x) ? c : 0 </td><td></td></tr>
<tr><td> if Y<X&#x21a3;C/0 </td><td> (y < x) ? c : 0 </td><td></td></tr>
<tr><td> if X>Y&#x21a3;X/0 </td><td> (x > y) ? x : 0 </td><td> X if X is greater than Y otherwise 0 </td></tr>
<tr><td> if X<Y&#x21a3;X/0 </td><td> (x < y) ? x : 0 </td><td></td></tr>
<tr><td> if X=Y&#x21a3;X/0 </td><td> (x == y) ? x : 0 </td><td></td></tr>
<tr><td> if Y>X&#x21a3;X/0 </td><td> (y > x) ? x : 0 </td><td></td></tr>
<tr><td> if Y<X&#x21a3;X/0 </td><td> (y < x) ? x : 0 </td><td></td></tr>
<tr><td> if X>Y&#x21a3;Y/0 </td><td> (x > y) ? y : 0 </td><td></td></tr>
<tr><td> if X<Y&#x21a3;Y/0 </td><td> (x < y) ? y : 0 </td><td></td></tr>
<tr><td> if X=Y&#x21a3;Y/0 </td><td> (x == y) ? y : 0 </td><td></td></tr>
<tr><td> if Y>X&#x21a3;Y/0 </td><td> (y > x) ? y : 0 </td><td></td></tr>
<tr><td> if Y<X&#x21a3;Y/0 </td><td> (y < x) ? y : 0 </td><td></td></tr>
<tr><td> if X>C&#x21a3;Y/0 </td><td> (x > c) ? y : 0 </td><td></td></tr>
<tr><td> if X<C&#x21a3;Y/0 </td><td> (x < c) ? y : 0 </td><td></td></tr>
<tr><td> if X=C&#x21a3;Y/0 </td><td> (x == c) ? y : 0 </td><td></td></tr>
<tr><td> if C>X&#x21a3;Y/0 </td><td> (c > x) ? y : 0 </td><td></td></tr>
<tr><td> if C<X&#x21a3;Y/0 </td><td> (c < x) ? y : 0 </td><td></td></tr>
<tr><td> if X>C&#x21a3;X/0 </td><td> (x > c) ? x : 0 </td><td></td></tr>
<tr><td> if X<C&#x21a3;X/0 </td><td> (x < c) ? x : 0 </td><td></td></tr>
<tr><td> if X=C&#x21a3;X/0 </td><td> (x == c) ? x : 0 </td><td></td></tr>
<tr><td> if C>X&#x21a3;X/0 </td><td> (c > x) ? x : 0 </td><td></td></tr>
<tr><td> if C<X&#x21a3;X/0 </td><td> (c < x) ? x : 0 </td><td></td></tr>
<tr><td> if X>C&#x21a3;X/Y </td><td> (x > c) ? x : y </td><td></td></tr>
<tr><td> if X<C&#x21a3;X/Y </td><td> (x < c) ? x : y </td><td></td></tr>
<tr><td> if X=C&#x21a3;X/Y </td><td> (x == c) ? x : y </td><td></td></tr>
<tr><td> if C>X&#x21a3;X/Y </td><td> (c > x) ? x : y </td><td></td></tr>
<tr><td> if C<X&#x21a3;X/Y </td><td> (c < x) ? x : y </td><td></td></tr>
<tr><td> if Y>C&#x21a3;X/0 </td><td> (y > c) ? x : 0 </td><td></td></tr>
<tr><td> if Y<C&#x21a3;X/0 </td><td> (y < c) ? x : 0 </td><td></td></tr>
<tr><td> if Y=C&#x21a3;X/0 </td><td> (y == c) ? x : 0 </td><td></td></tr>
<tr><td> if C>Y&#x21a3;X/0 </td><td> (c > y) ? x : 0 </td><td></td></tr>
<tr><td> if C<Y&#x21a3;X/0 </td><td> (c < y) ? x : 0 </td><td></td></tr>
<tr><td> if Y>C&#x21a3;Y/0 </td><td> (y > c) ? y : 0 </td><td></td></tr>
<tr><td> if Y<C&#x21a3;Y/0 </td><td> (y < c) ? y : 0 </td><td></td></tr>
<tr><td> if Y=C&#x21a3;Y/0 </td><td> (y == c) ? y : 0 </td><td></td></tr>
<tr><td> if C>Y&#x21a3;Y/0 </td><td> (c > y) ? y : 0 </td><td></td></tr>
<tr><td> if C<Y&#x21a3;Y/0 </td><td> (c < y) ? y : 0 </td><td></td></tr>
<tr><td> if Y>C&#x21a3;Y/X </td><td> (y > c) ? y : x </td><td> Y if Y is greater than C otherwise X </td></tr>
<tr><td> if Y<C&#x21a3;Y/X </td><td> (y < c) ? y : x </td><td> Y if Y is less than C otherwise X </td></tr>
<tr><td> if Y=C&#x21a3;Y/X </td><td> (y == c) ? y : x </td><td> Y if Y is C otherwise X </td></tr>
<tr><td> if C>Y&#x21a3;Y/X </td><td> (c > y) ? y : x </td><td> Y if C is greater than Y otherwise X </td></tr>
<tr><td> if C<Y&#x21a3;Y/X </td><td> (c < y) ? y : x </td><td> Y if C is less than Y otherwise X </td></tr>
</table>

## Rounding

<table>
<thead><th> Display                  </th><th> Code               </th><th> Description          </th></thead>
<tr><td> if X>0&#x21a3;Y/C </td><td> (x > 0) ? y : c </td><td> Y if X is greater than 0 otherwise C </td></tr>
<tr><td>&#x230a;X+C&#x230b;</td><td> floor(x + c) </td><td>Round down x and c</td></tr>
<tr><td>&#x230a;Y+C&#x230b;</td><td> floor(y + c) </td><td>Round down y and c</td></tr>
<tr><td>&#x230a;X+Y+C&#x230b;</td><td> floor(x + y + c) </td><td>Round down x and y and c</td></tr>
<tr><td>&#x230a;CX&#x230b;</td><td> floor(x * c) </td><td>Round down x by c</td></tr>
<tr><td>&#x230a;CY&#x230b;</td><td> floor(y * c) </td><td>Round down x by c</td></tr>
<tr><td>&#x230a;CXY&#x230b;</td><td> floor(x * y * c) </td><td>Round down x by y by c</td></tr>
<tr><td>&#x2308;X+C&#x2309;</td><td> ceil(x + c) </td><td>Round up x and c</td></tr>
<tr><td>&#x2308;Y+C&#x2309;</td><td> ceil(y + c) </td><td>Round up y and c</td></tr>
<tr><td>&#x2308;X+Y+C&#x2309;</td><td> ceil(x + y + c) </td><td>Round up x and y and c</td></tr>
<tr><td>&#x2308;CX&#x2309;</td><td> ceil(x * c) </td><td>Round up x by c</td></tr>
<tr><td>&#x2308;CY&#x2309;</td><td> ceil(y * c) </td><td>Round up x by c</td></tr>
<tr><td>&#x2308;CXY&#x2309;</td><td> ceil(x * y * c) </td><td>Round up x by y by c</td></tr>
</table>
