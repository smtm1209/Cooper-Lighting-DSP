#include "constants.h"
#include <cstring>
#include <complex>
#include <iostream>
#include <valarray>
 
const double PI = 3.141592653589793238460;
 
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
 
/** 
 * Function for adaptive filtering
 * @param x pastMask
 * @param d input from mic
 * @param e array to hold the result of adaptive filtering
 */
void nlms(float x[INPUT_BUFFER_SIZE], float d[INPUT_BUFFER_SIZE], float e[INPUT_BUFFER_SIZE])
{
    float b_xx[FILTER_LENGTH];
    float b_w;
    float xx;
    int temp = FILTER_LENGTH -1;
    // mu: adaptation factor
    // w: filter weight
    // dhat: filter output(predicted value)
    float mu = 0.002; //adaptation factor
    float w[FILTER_LENGTH]; 
    float dhat[INPUT_BUFFER_SIZE];

    //initialize to array of zeros
    std::memset(&dhat[0], 0, INPUT_BUFFER_SIZE * sizeof(float));
    std::memset(&e[0], 0, INPUT_BUFFER_SIZE * sizeof(float));
    std::memset(&w[0], 0, FILTER_LENGTH * sizeof(float));

    
    //  now implement the adaptive filter
    for (int n = 0; n < INPUT_BUFFER_SIZE-temp; n++) {

        //  produce filtered output sample
        b_w = 0.0;

        //  update the filter coefficients
        xx = 0.0;
        for (int i = 0; i < FILTER_LENGTH; i++) {
            float b_d;
            b_d = x[(n - i) + temp];
            b_xx[i] = b_d;
            b_w += w[i] * b_d;
            xx += b_d * b_d;
        }

        dhat[n + temp] = b_w;
        b_w = d[n + temp] - b_w;
        e[n + temp] = b_w;
        for (int i = 0; i < FILTER_LENGTH; i++) {
            w[i] += 2*mu * b_xx[i] * b_w / xx;
        }
    }
    

    
}


// source: https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// !!! Warning : in some cases this code make result different from not optimased version above (need to fix bug)
// The bug is now fixed @2017/05/30 
void fft(CArray &x)
{
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
	//// Normalize (This section make it not working correctly)
	//Complex f = 1.0 / sqrt(N);
	//for (unsigned int i = 0; i < N; i++)
	//	x[i] *= f;
}
 
// inverse fft (in-place)
void ifft(CArray& x)
{
    // conjugate the complex numbers
    x = x.apply(std::conj);
 
    // forward fft
    fft( x );
 
    // conjugate the complex numbers again
    x = x.apply(std::conj);
 
    // scale the numbers
    x /= x.size();
}
 
