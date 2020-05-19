#include <math.h>
//#include "haversine.h"

#define d2r (M_PI / 180.0)
#define radius 6367.77

//calculate haversine distance for linear distance
double haversine_dist(double lat1, double long1, double lat2, double long2)
{
    double dlong = (long2 - long1) * d2r;
    double dlat = (lat2 - lat1) * d2r;
    double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = radius * c;
    return d*1000;
}
