// This is PROPRIETARY CODE, do not modify, reuse, or share.
// All Rights Reserved.
// See LICENSE.md for details.

#include "compileconfig.h"

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "math2d.h"
#include "mathdefs.h"


double math_fixanglef(double deg) {
    if (unlikely(deg > 180))
        deg = fmod((deg - 180.0), 360.0) - 180.0;
    else if (unlikely(deg < -180))
        deg = -(fmod(((-deg) - 180.0), 360.0) - 180.0);
    return deg;
}

int32_t math_fixanglei(int32_t deg) {
    if (unlikely(deg > 180 * ANGLE_SCALAR))
        deg = ((deg - 180 * ANGLE_SCALAR) % (360 * ANGLE_SCALAR)) -
            180 * ANGLE_SCALAR;
    else if (unlikely(deg < -180 * ANGLE_SCALAR))
        deg = -((((-deg) - 180 * ANGLE_SCALAR) %
            (360 * ANGLE_SCALAR)) - 180 * ANGLE_SCALAR);
    return deg;
}

double math_angle2df(double x, double y) {
    // Angles: (1.0, 0.0) returns 0 degrees angle,
    // CCW rotation increases angle.
    return -((atan2(y, x) / M_PI) * 180.0);
}

int32_t math_angle2di(int64_t x, int64_t y) {
    // Angles: (1.0, 0.0) returns 0 degrees angle,
    // CCW rotation increases angle.
    return (int32_t)round((-(long double)((
        atan2l((long double)y, (long double)x) / M_PI)
    * 180.0)) * ANGLE_SCALAR);
}

void math_rotate2df(double *x, double *y, double rot) {
    // Axis: x right, y down, rot+ rotates CCW
    rot = (-rot / 180.0) * M_PI;
    double newy = (*y) * cos(rot) + (*x) * sin(rot);
    double newx = (*x) * cos(rot) - (*y) * sin(rot);
    *x = newx;
    *y = newy;
}

void math_rotate2di(int64_t *x, int64_t *y, int32_t rot) {
    // Axis: x right, y down, rot+ rotates CCW
    double rotf = ((double)rot) / ANGLE_SCALAR;
    rotf = (-rotf / 180.0) * M_PI;
    long double newy = ((long double)*y) * cosl(rotf) +
        ((long double)*x) * sinl(rotf);
    long double newx = ((long double)*x) * cosl(rotf) -
        ((long double)*y) * sinl(rotf);
    *x = newx;
    *y = newy;
}

int64_t math_veclen(int64_t x, int64_t y) {
    int64_t result = roundl(sqrtl(
        (long double)(x * x + y * y)
    ));
    return result;
}

int math_lineintersect2df(
        double l1x1, double l1y1, double l1x2, double l1y2,
        double l2x1, double l2y1, double l2x2, double l2y2,
        double *ix, double *iy) {
    // If the two line segments intersect,
    // returns 1 and sets ix, iy coordinates of intersection.
    // Otherwise, returns 0.
    int l1vert = (
        fabs(l1x2 - l1x1) < 0.001 && (
        fabs(l1x2 - l1x1) <=
            fabs(l1y2 - l1y1) * 0.1 ||
        l1x2 - l2x1 == 0)
    );
    int l2vert = (
        fabs(l1x2 - l1x1) < 0.001 && (
        fabs(l1x2 - l1x1) <=
            fabs(l1y2 - l1y1) * 0.1 ||
        l2x2 - l2x1 == 0)
    );
    if (l2vert && !l1vert) {
        int tvert = l1vert;
        l1vert = l2vert;
        l2vert = tvert;
        double t1x1, t1y1, t1x2, t1y2;
        t1x1 = l1x1; t1y1 = l1y1; t1x2 = l1x2; t1y2 = l1y2;
        l1x1 = l2x1; l1y1 = l2y1; l1x2 = l2x2; l1y2 = l2y2;
        l2x1 = t1x1; l2y1 = t1y1; l2x2 = t1x2; l2y2 = t1y2;
    }
    if (l1vert) {
        if (l1y1 < l1y2) {
            double t = l1y1;
            l1y1 = l1y2;
            l1y2 = t;
        }
        if (l2vert) {
            if (fabs(l1x1 - l2x2) > 0.00001) {
                return 0;
            }
            if (l2y1 < l2y2) {
                double t = l2y1;
                l2y1 = l2y2;
                l2y2 = t;
            }
            if (l2y1 > l1y2 || l2y2 < l1y1) {
                return 0;
            }
            *ix = l1x1;
            *iy = l1y1;
            if (l2y1 > l1y1) *iy = l2y1;
            return 1;
        }
        double l2d = (l2y2 - l2y1) / (l2x2 - l2y1);
        double y2 = l2y1 + l2d * (
            (l1x1 - l2x1)
        );
        if (y2 < l1y1 || y2 > l1y2) {
            return 0;
        }
        *ix = l1x1;
        *iy = y2;
        return 1;
    }
    // Based on https://github.com/psalaets/line-intersect,
    // also see 3RDPARTYCREDITS.md:
    double dval = ((l2y2 - l2y1) * (l1x2 - l1x1)) -
        ((l2x2 - l2x1) * (l1y2 - l1y1));
    double valA = ((l2x2 - l2x1) * (l1y1 - l2y1)) -
        ((l2y2 - l2y1) * (l1x1 - l2x1));
    double valB = ((l1x2 - l1x1) * (l1y1 - l2y1)) -
        ((l1y2 - l1y1) * (l1x1 - l2x1));
    if (dval == 0) {
        return 0;
    }
    double slopeA = valA / dval;
    double slopeB = valB / dval;
    if (slopeA >= 0 && slopeA <= 1 && slopeB >= 0 && slopeB <= 1) {
        *ix = (l1x1 + (slopeA * (l1x2 - l1x1)));
        *iy = (l1y1 + (slopeA * (l1y2 - l1y1)));
        return 1;
    }
    return 0;
}

int math_lineintersect2di(
        int64_t l1x1, int64_t l1y1, int64_t l1x2, int64_t l1y2,
        int64_t l2x1, int64_t l2y1, int64_t l2x2, int64_t l2y2,
        int64_t *ix, int64_t *iy
        ) {
    // If the two line segments intersect,
    // returns 1 and sets ix, iy coordinates of intersection.
    // Otherwise, returns 0.
    int l1vert = (l1x2 - l1x1 == 0LL);
    int l2vert = (l1x2 - l1x1 == 0LL);
    if (l2vert && !l1vert) {
        int tvert = l1vert;
        l1vert = l2vert;
        l2vert = tvert;
        int64_t t1x1, t1y1, t1x2, t1y2;
        t1x1 = l1x1; t1y1 = l1y1; t1x2 = l1x2; t1y2 = l1y2;
        l1x1 = l2x1; l1y1 = l2y1; l1x2 = l2x2; l1y2 = l2y2;
        l2x1 = t1x1; l2y1 = t1y1; l2x2 = t1x2; l2y2 = t1y2;
    }
    if (l1vert) {
        if (l1y1 < l1y2) {
            int64_t t = l1y1;
            l1y1 = l1y2;
            l1y2 = t;
        }
        if (l2vert) {
            if (l1x1 - l2x2 > 0) {
                return 0;
            }
            if (l2y1 < l2y2) {
                int64_t t = l2y1;
                l2y1 = l2y2;
                l2y2 = t;
            }
            if (l2y1 > l1y2 || l2y2 < l1y1) {
                return 0;
            }
            *ix = l1x1;
            *iy = l1y1;
            if (l2y1 > l1y1) *iy = l2y1;
            return 1;
        }
        long double l2d = (
            ((long double)(l2y2 - l2y1)) /
            ((long double)(l2x2 - l2y1))
        );
        int64_t y2 = roundl((long double)l2y1 + l2d * (
            (l1x1 - l2x1)
        ));
        if (y2 < l1y1 || y2 > l1y2) {
            return 0;
        }
        *ix = l1x1;
        *iy = y2;
        return 1;
    }
    // Based on https://github.com/psalaets/line-intersect,
    // also see 3RDPARTYCREDITS.md:
    int64_t dval = ((l2y2 - l2y1) * (l1x2 - l1x1)) -
        ((l2x2 - l2x1) * (l1y2 - l1y1));
    int64_t valA = ((l2x2 - l2x1) * (l1y1 - l2y1)) -
        ((l2y2 - l2y1) * (l1x1 - l2x1));
    int64_t valB = ((l1x2 - l1x1) * (l1y1 - l2y1)) -
        ((l1y2 - l1y1) * (l1x1 - l2x1));
    if (dval == 0) {
        return 0;
    }
    long double slopeA = valA / (long double)dval;
    long double slopeB = valB / (long double)dval;
    if (slopeA >= 0 && slopeA <= 1 && slopeB >= 0 && slopeB <= 1) {
        *ix = roundl(l1x1 + (slopeA * (l1x2 - l1x1)));
        *iy = roundl(l1y1 + (slopeA * (l1y2 - l1y1)));
        return 1;
    }
    return 0;
}

static __attribute__((constructor)) void _test_math_lineintersect2d() {
    // x,y_x,y 128,-128_0,128   AND  x,y_x,y 0,0_17363,9924
    #ifndef NDEBUG
    int64_t ix, iy;
    int result = math_lineintersect2di(
        128,-128,0,128,
        0,0,17363,9924,
        &ix, &iy
    );
    assert(result != 0);
    #endif
}

/*int math_polyintersect2df(
        double lx1, double ly1, double lx2, double ly2,
        int corner_count, const double *cx, const double *cy,
        int *iwall, double *ix, double *iy
        ) {
    // If intersection found, returns 1 and sets
    // iwall to intersected section number, and
    // ix/iy to the intersection point.
    // Otherwise, returns 0.

    int contains1 = math_polycontains2df(
        lx1, ly1, corner_count, cx, cy
    );
    int contains2 = math_polycontains2df(
        lx2, ly2, corner_count, cx, cy
    );
    if (contains1 == contains2) {
        return 0;
    }
    if (!contains1) {
        double tx = lx1;
        double ty = ly1;
        lx1 = lx2;
        ly1 = ly2;
        lx2 = tx;
        ly2 = ty;
        contains1 = 1;
        contains2 = 0;
    }
    int i = 0;
    int imax = corner_count;
    int iprev = imax - 1;
    while (i < imax) {
        double wix, wiy;
        if (math_lineintersect2df(
                cx[iprev],
                cy[iprev],
                cx[i],
                cy[i],
                lx1, ly2, lx2, ly2,
                &wix, &wiy
                )) {
            *iwall = iprev;
            *ix = wix;
            *iy = wiy;
            return 1;
        }
        iprev = i;
        i++;
    }
    return 0;
}*/

int math_polycontains2di(int64_t px, int64_t py,
        int corner_count, const int64_t *cx, const int64_t *cy) {
    // Based on an algorithm by Randolph Franklin.
    const int imax = corner_count;
    int i = 0;
    int j = imax - 1;
    int c = 0;
    while (i < imax) {
        int cond1 = ((cy[i] > py) !=
            (cy[j] > py));
        int cond2 = 0;
        if (cy[j] - cy[i] != 0)  // Dividing by this won't be infinity
            cond2 = (px < (cx[j] - cx[i]) *
                (py - cy[i]) /
                (cy[j] - cy[i]) + cx[i]);
        if (cond1 && cond2) {
            c = !c;
        }
        j = i;
        i++;
    }
    return c;
}

static __attribute__((constructor)) void _test_math_polycontains2d() {
    double corner_x_f[3];
    double corner_y_f[3];
    int64_t corner_x[3];
    int64_t corner_y[3];
    const int ONE_METER_IN_UNITS_c = 128;
    corner_x_f[0] = -ONE_METER_IN_UNITS_c;
    corner_y_f[0] = -ONE_METER_IN_UNITS_c;
    corner_x_f[1] = ONE_METER_IN_UNITS_c;
    corner_y_f[1] = -ONE_METER_IN_UNITS_c;
    corner_x_f[2] = 0;
    corner_y_f[2] = ONE_METER_IN_UNITS_c;
    int result = 1; /*math_polycontains2df(
        0, 0, 3, corner_x_f, corner_y_f
    );*/
    assert(result != 0);
    corner_x[0] = -ONE_METER_IN_UNITS_c;
    corner_y[0] = -ONE_METER_IN_UNITS_c;
    corner_x[1] = ONE_METER_IN_UNITS_c;
    corner_y[1] = -ONE_METER_IN_UNITS_c;
    corner_x[2] = 0;
    corner_y[2] = ONE_METER_IN_UNITS_c;
    result = math_polycontains2di(0, 0, 3, corner_x, corner_y);
    assert(result != 0);
    corner_x[0] = -10240;
    corner_y[0] = -10240;
    corner_x[1] = 10240;
    corner_y[1] = -10240;
    corner_x[2] = 0;
    corner_y[2] = 10240;
    result = math_polycontains2di(
        22460, -61540, 3, corner_x, corner_y
    );
    assert(!result);
}

int math_vecsopposite2di(
        int64_t v1x, int64_t v1y, int64_t v2x, int64_t v2y
        ) {
    int64_t result = (v1x * v2x + v1y * v2y);
    return (result < 0);
}

static __attribute__((constructor))
        void _test_math_vecsopposite2di() {
    int result;
    result = math_vecsopposite2di(
        1, 0, -1, -1
    );
    assert(result != 0);
    result = math_vecsopposite2di(
        1, 0, 1, -1
    );
    assert(result == 0);
}

int math_polyintersect2di(
        int64_t lx1, int64_t ly1, int64_t lx2, int64_t ly2,
        int corner_count, const int64_t *cx, const int64_t *cy,
        int *iwall, int64_t *ix, int64_t *iy
        ) {
    // If intersection found, returns 1 and sets
    // iwall to intersected section number, and
    // ix/iy to the intersection point.
    // Otherwise, returns 0.

    int contains1 = math_polycontains2di(
        lx1, ly1, corner_count, cx, cy
    );
    int contains2 = math_polycontains2di(
        lx2, ly2, corner_count, cx, cy
    );
    if (contains1 == contains2) {
        /*printf("EARLY ABORT. line is fully %s "
            "the polygon: %" PRId64 ",%" PRId64
            " to %" PRId64 ",%" PRId64 "\n",
            (contains1 ? "inside": "outside"),
            lx1, ly1, lx2, ly2);
        int i = 0;
        printf("polypoints:");
        while (i < corner_count) {
            printf(" %" PRId64 ",%" PRId64,
                cx[i], cy[i]);
            i++;
        }
        printf("\n");*/
        return 0;
    }
    if (!contains1) {
        int64_t tx = lx1;
        int64_t ty = ly1;
        lx1 = lx2;
        ly1 = ly2;
        lx2 = tx;
        ly2 = ty;
        contains1 = 1;
        contains2 = 0;
    }
    int i = 0;
    int imax = corner_count;
    int iprev = imax - 1;
    while (i < imax) {
        int64_t wix, wiy;
        /*printf("checking %" PRId64 ",%" PRId64 " "
            "to %" PRId64 ",%" PRId64 "  VS  "
            "%" PRId64 ",%" PRId64 " to "
            "%" PRId64 ",%" PRId64 "\n",
            cx[iprev], cy[iprev],
            cx[i], cy[i],
            lx1, ly1, lx2, ly2);*/
        if (math_lineintersect2di(
                cx[iprev],
                cy[iprev],
                cx[i],
                cy[i],
                lx1, ly1, lx2, ly2,
                &wix, &wiy
                )) {
            *iwall = iprev;
            *ix = wix;
            *iy = wiy;
            return 1;
        }
        iprev = i;
        i++;
    }
    return 0;
}
