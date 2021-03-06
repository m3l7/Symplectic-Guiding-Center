/**
 * @file splineexpl.c
 * @brief Cubic spline interpolation of a 1D data set
 */
#include <stdlib.h>
#include "spline.h"
#include "./common_spline.h"

/**
 * @brief Calculate tricubic spline interpolation coefficients for 3D data
 *
 * This function calculates the cubic interpolation coefficients
 * for a 1D data set using one of two possible boundary conditions.
 * Function returns pointer to coefficient array.
 *
 * @param f 1D data to be interpolated
 * @param n number of data points
 * @param bc boundary condition flag
 * @param c array for coefficient storage
 */
void spline(realnum* f, int n, int bc, realnum* c) {

    /* Allocate needed variables */

    /* Array for RHS of matrix equation */
    realnum* Y = (realnum*)malloc(n*sizeof(realnum));
    /* Array superdiagonal values */
    realnum* p = (realnum*)malloc((n-1)*sizeof(realnum));
    /* Array for derivative vector to solve */
    realnum* D = (realnum*)malloc(n*sizeof(realnum));

    if(bc == NATURALBC) {

        /* Initialize RHS of equation */
        Y[0] = 3*(f[1]-f[0]);
        for(int i=1; i<n-1; i++) {
            Y[i] = 3*(f[i+1] - f[i-1]);
        }
        Y[n-1] = 3*(f[n-1] - f[n-2]);

        /* Thomas algorithm is used*/

        /* Forward sweep */
        p[0] = 1.0/2;
        Y[0] = Y[0]/2;
        for(int i=1; i<n-1; i++) {
            p[i] = 1/(4-p[i-1]);
            Y[i] = (Y[i]-Y[i-1])/(4-p[i-1]);
        }
        Y[n-1] = (Y[n-1]-Y[n-2])/(2-p[n-2]);

        /* Back substitution */
        D[n-1] = Y[n-1];
        for(int i=n-2; i>-1; i--) {
            D[i] = Y[i]-p[i]*D[i+1];
        }
    }
    else if(bc== PERIODICBC) {
        /* PERIODIC */
        /* Initialize some additional necessary variables */
        realnum l = 1.0; /* Value that starts from lower left corner and moves right */
        realnum dlast = 4.0; /* Last diagonal value */
        realnum* r = (realnum*)malloc((n-2)*sizeof(realnum)); /* Matrix right column values */
        realnum blast; /* Last subdiagonal value */
        /* Initialize RHS of equation */
        Y[0] = 3*(f[1] - f[n-1]);
        for(int i=1; i<n-1; i++) {
            Y[i] = 3*(f[i+1]-f[i-1]);
        }
        Y[n-1] = 3*(f[0]-f[n-2]);
        /* Simplified Gauss elimination is used (own algorithm) */
        /* Forward sweep */
        p[0] = 1.0/4;
        r[0] = 1.0/4;
        Y[0] = Y[0]/4;
        for(int i=1; i<n-2; i++) {
            dlast = dlast-l*r[i-1];
            Y[n-1] = Y[n-1]-l*Y[i-1];
            l = -l*p[i-1];
            p[i] = 1/(4-p[i-1]);
            r[i] = (-r[i-1])/(4-p[i-1]);
            Y[i] = (Y[i]-Y[i-1])/(4-p[i-1]);
        }
        blast = 1.0-l*p[n-3];
        dlast = dlast-l*r[n-3];
        Y[n-1] = Y[n-1]-l*Y[n-3];
        p[n-2] = (1-r[n-3])/(4-p[n-3]);
        Y[n-2] = (Y[n-2]-Y[n-3])/(4-p[n-3]);
        Y[n-1] = (Y[n-1]-blast*Y[n-2])/(dlast-blast*p[n-2]);
        /* Back substitution */
        D[n-1] = Y[n-1];
        D[n-2] = Y[n-2]-p[n-2]*D[n-1];
        for(int i=n-3; i>-1; i--) {
            D[i] = Y[i]-p[i]*D[i+1]-r[i]*D[n-1];
        }
        /* Free allocated memory */
        free(r);

        /* Period-closing spline coefficients */
        c[(n-1)*4]   = f[n-1];
        c[(n-1)*4+1] = D[n-1];
        c[(n-1)*4+2] = 3*(f[0]-f[n-1])-2*D[n-1]-D[0];
        c[(n-1)*4+3] = 2*(f[n-1]-f[0])+D[n-1]+D[0];
    }

    /* Derive spline coefficients from solved derivatives */
    for(int i=0; i<n-1; i++) {
        c[i*4]   = f[i];
        c[i*4+1] = D[i];
        c[i*4+2] = 3*(f[i+1]-f[i])-2*D[i]-D[i+1];
        c[i*4+3] = 2*(f[i]-f[i+1])+D[i]+D[i+1];
    }

    /* Free allocated memory */
    free(Y);
    free(p);
    free(D);
}
