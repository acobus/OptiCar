#ifndef ANGLEDIFF_H
#define ANGLEDIFF_H

/** Finds the minimum angle difference ang1-ang2 such that difference
 * is between [-PI,PI]
 * @param[in] ang1 
 * @param[in] ang2
 * @return angle difference ang1-ang2
 */
double angleDiff(double ang1, double ang2);

/** Finds the minimum angle difference ang1-ang2 such that difference
 * is between [-PI/2,PI/2]
 * @param[in] ang1
 * @param[in] ang2
 * @return angle difference ang1-ang2
 */
double angleDiff2(double ang1, double ang2);


/** Wraps angle from [0,2PI] or [-2PI,0]
 * @param[in] ang Angle in radians to wrap
 * @return Wrapped angle from [-PI,PI]
 */
double angleWrap(double ang);

/** Wraps angle from [0,PI] or [-PI,0]
 * @param[in] ang Angle in radians to wrap
 * @return Wrapped angle from [-PI,PI]
 */
double angleWrap2(double ang);

/** Calculate arc radius
 * @param[in] arcDistance length of arc
 * @param[in] alpha angle
*/
double arcRadius(double arcDistance, double alpha);

#endif