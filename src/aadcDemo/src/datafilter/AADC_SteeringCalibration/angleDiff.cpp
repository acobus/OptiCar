#include "stdafx.h"

#include "angleDiff.h"

double angleDiff(double ang1, double ang2)
{
    double d;
    ang1 = angleWrap(ang1);
    ang2 = angleWrap(ang2);
    d = ang1-ang2;
    if ( d > cStdMath::MATH_PI )
        d -= 2*(double)cStdMath::MATH_PI;
    else if ( d < -(double)cStdMath::MATH_PI )
        d += 2*(double)cStdMath::MATH_PI;
    return d;
}

double angleDiff2(double ang1, double ang2)
{
    double d;
    ang1 = angleWrap2(ang1);
    ang2 = angleWrap2(ang2);
    d = ang1-ang2;
    if ( d > cStdMath::MATH_PI / 2 )
        d -= (double)cStdMath::MATH_PI;
    else if ( d < -(double)cStdMath::MATH_PI/2 )
        d += (double)cStdMath::MATH_PI;
    return d;
}


double angleWrap(double ang)
{
    if (ang > cStdMath::MATH_PI)
        return ang-2*(double)cStdMath::MATH_PI;
    else if (ang <= -(double)cStdMath::MATH_PI)
        return ang+2*(double)cStdMath::MATH_PI;
    else
        return ang;
}

double angleWrap2(double ang)
{
    if (ang > cStdMath::MATH_PI/2)
        return ang-(double)cStdMath::MATH_PI;
    else if (ang <= -(double)cStdMath::MATH_PI/2)
        return ang+(double)cStdMath::MATH_PI;
    else
        return ang;
}

double arcRadius(double arcDistance, double alpha)
{
	//check for div by zero
	if(alpha == 0.0){
		return 0.0;
	}
    alpha = alpha * cStdMath::MATH_RAD2DEG;
	return  (180 * arcDistance)/(cStdMath::MATH_PI * alpha);
}
