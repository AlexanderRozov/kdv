// simple parallel KDV solution
// A.S .Rozov 02.01.2022

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DX 0.1
#define L  8
#define DT 1e-4
#define N  160

void rk4(double x[], double xnew[], double dt, double dx);
void kdv(double x[], double xnew[], double dt, double dx);
void kdvExact(double x[], double t, double v, double x0);

int main(void) {

	int i, j;
	double t;
	double *x    = (double *) malloc(N * sizeof(double));
	double *xnew = (double *) malloc(N * sizeof(double));

	for(i = 0; i < N; i++) {
	    x[i] = -L + i * DX;
		xnew[i] = -L + i * DX;
	}

	/*
	 * Initial Conditions
	 */
	kdvExact(x, 0., 16., 4.);
	kdvExact(xnew, 0, 4., -4.);

	for(i = 0; i < N; i++) {
		x[i] = x[i] + xnew[i];
	}

	t = 0.;
	j = 0;
	while(t < 2) {

		if((j % 100) == 0) {
	        for(i = 0; i < N; i++) {
		        printf("%e %e\n", -L + i * DX, x[i]);
	        }
			printf("\n\n");
		}

		rk4(x, xnew, DT, DX);

		for(i = 0; i < N; i++) {
			x[i] = xnew[i];
		}

		j++;
		t += DT;
	}

	free(x);
	free(xnew);

	return 0;
}

void kdv(double x[], double xnew[], double dt, double dx) {

	int i;
	double *f1 = malloc(sizeof(double) * N);
	double *f2 = malloc(sizeof(double) * N);
	double *f3 = malloc(sizeof(double) * N);
	double *f4 = malloc(sizeof(double) * N);
	double *b1 = malloc(sizeof(double) * N);
	double *b2 = malloc(sizeof(double) * N);
	double *b3 = malloc(sizeof(double) * N);
	double *b4 = malloc(sizeof(double) * N);
	double *ux1 = malloc(sizeof(double) * N);
	double *ux3 = malloc(sizeof(double) * N);

	for(i = 0; i < N-1; i++) {
		f1[i] = x[i+1];
	}
	f1[N-1] = x[0];

	for(i = 0; i < N-2; i++) {
		f2[i] = x[i+2];
	}
	f2[N-1] = x[1];
	f2[N-2] = x[0];

	for(i = 0; i < N-3; i++) {
		f3[i] = x[i+3];
	}
	f3[N-1] = x[2];
	f3[N-2] = x[1];
	f3[N-3] = x[0];

	for(i = 0; i < N-4; i++) {
		f4[i] = x[i+4];
	}
	f4[N-1] = x[3];
	f4[N-2] = x[2];
	f4[N-3] = x[1];
	f4[N-4] = x[0];

	for(i = 1; i < N; i++) {
		b1[i] = x[i-1];
	}
	b1[0] = x[N-1];

	for(i = 2; i < N; i++) {
		b2[i] = x[i - 2];
	}
	b2[0] = x[N-2];
	b2[1] = x[N-1];

	for(i = 3; i < N; i++) {
		b3[i] = x[i - 3];
	}
	b3[0] = x[N-3];
	b3[1] = x[N-2];
	b3[2] = x[N-1];

	for(i = 4; i < N; i++) {
		b4[i] = x[i - 4];
	}
	b4[0] = x[N-4];
	b4[1] = x[N-3];
	b4[2] = x[N-2];
	b4[3] = x[N-1];


	for(i = 0; i < N; i++) {
		/*
		 * The O(h^2) central difference
		 */
		/*
		ux1[i] =                         (f1[i] - b1[i]) / (2. * dx);
		ux3[i] = (f2[i] - 2 * f1[i] + 2 * b1[i] - b2[i]) / (2. * dx * dx * dx);
		*/

		/*
		 * The O(h^4) central difference
		 */
		/*
		ux1[i] = (                      - (f2[i] - b2[i]) +  8. * (f1[i] - b1[i])) / (12. * dx);
		ux3[i] = (- (f3[i] - b3[i]) + 8 * (f2[i] - b2[i]) - 13. * (f1[i] - b1[i])) / (8.  * dx * dx * dx);
		*/
		/*
		 * The O(h^6) central difference
		 */
		ux1[i] = (                            (f3[i] - b3[i]) -   9. * (f2[i] - b2[i]) +  45. * (f1[i] - b1[i])) / ( 60. * dx);
		ux3[i] = (7 * (f4[i] - b4[i]) - 72. * (f3[i] - b3[i]) + 338. * (f2[i] - b2[i]) - 488. * (f1[i] - b1[i])) / (240. * dx * dx * dx);

		xnew[i] = -6. * x[i] * ux1[i] - ux3[i];
	}

	free(f1);
	free(f2);
	free(f3);
	free(f4);
	free(b1);
	free(b2);
	free(b3);
	free(b4);
	free(ux1);
	free(ux3);
}

void kdvExact(double x[], double t, double v, double x0) {

	int i;
	double denom;

	for(i = 0; i < N; i++) {
      denom = cosh(0.5 * sqrt(v) * (x[i] - v * t - x0));
	  x[i] = v / (2. * denom * denom);
	}

}

void rk4(double x[], double xnew[], double dt, double dx) {

	int i;
	double *k1 = malloc(sizeof(double) * N);
	double *k2 = malloc(sizeof(double) * N);
	double *k3 = malloc(sizeof(double) * N);
	double *k4 = malloc(sizeof(double) * N);

	kdv(x, k1, dt, dx);
	for(i = 0; i < N; i++) {
		xnew[i] = x[i] + 0.5 * k1[i] * dt;
	}
	kdv(xnew, k2, dt, dx);
	for(i = 0; i < N; i++) {
		xnew[i] = x[i] + 0.5 * k2[i] * dt;
	}
	kdv(xnew, k3, dt, dx);
	for(i = 0; i < N; i++) {
		xnew[i] = x[i] + k3[i] * dt;
	}
	kdv(xnew, k4, dt, dx);
	for(i = 0; i < N; i++) {
		xnew[i] = x[i] + 1./6. * (k1[i] + 2. * k2[i] + 2. * k3[i] + k4[i]) * dt;
	}

	free(k1);
	free(k2);
	free(k3);
	free(k4);
}
