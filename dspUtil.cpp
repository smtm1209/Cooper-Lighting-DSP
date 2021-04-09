#include "constants.h"
#include <cstring>
#include <complex>
#include <iostream>
#include <valarray>
#include <math.h>

const double PI = 3.141592653589793238460;
 
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
 
float max_corr(float*x, float*d)
{
    float sum_x = 0;
    float sum_d = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        sum_x = sum_x + x[i];
        sum_d = sum_d + d[i];
    }
    
    float mean_x = sum_x / BUFFER_SIZE;
    float mean_d = sum_d / BUFFER_SIZE;
    //printf("mean of input is %f\n",mean_x);
    //printf("mean of mask is %f\n", mean_d);

    float tmp_x[BUFFER_SIZE];
    float tmp_d[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        tmp_x[i] = x[i]-mean_x;
        tmp_d[i] = d[i]-mean_d;
    }
    
    float num = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        num = num + tmp_x[i] * tmp_d[i];
    }
    float denum;
    float sum1 = 0;
    float sum2 = 0;
    for (int i = 0; i < BUFFER_SIZE; i ++)
    {
        sum1 = sum1 + tmp_x[i]*tmp_x[i];
        sum2 = sum2 + tmp_d[i]*tmp_d[i];
    }
    denum = sqrt(sum1*sum2);
    return num/denum;
}

/** 
 * Function for adaptive filtering
 * @param x pastMask
 * @param d input from mic
 * @param e array to hold the result of adaptive filtering
 */
void nlms(float* x, float* d, float* e)
{

    int N = FILTER_LENGTH;
    // mu: adaptation factor
    // w: filter weight
    // dhat: filter output(predicted value)
    float mu = 0.5; //adaptation factor
    float w[FILTER_LENGTH]; 
    float dhat[INPUT_BUFFER_SIZE];

    //initialize array of zeros

    for (int i =0; i < INPUT_BUFFER_SIZE; i ++ )
    {
        dhat[i] = 0;
        e[i] = 0;
    }
    for (int i = 0; i < N; i ++ )
    {
        w[i] = 0;
    }

   float xx[N];
   
   for (int n = N; n <= BUFFER_SIZE; n ++)
   {
       int count = 0;
       for (int i = n; i >= n-(N-1); i--)
       {
           xx[count] = x[i-1];
           count ++;
       }

       float sum = 0;
       for (int i = 0; i < N; i++)
       {
           sum = sum + w[i] * xx[i];
       }
        dhat[n-1] = sum; 
        e[n-1] = d[n-1] - dhat[n-1];

        float sum1= 0;
        for (int i = 0; i < N ; i++)
            sum1 = sum1 + xx[i]*xx[i];

        for (int i = 0; i < N; i++)
            w[i] = w[i] + 2.0*mu*xx[i]*e[n-1]/sum1;
        
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
 
