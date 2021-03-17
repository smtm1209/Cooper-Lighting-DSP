//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  MaskGenerate.cpp
//
//  Code generation for function 'MaskGenerate'
//


// Include files
#include "MaskGenerate.h"
#include "MaskGenerate_types.h"
#include "nlms.h"
#include "randn.h"
#include "rt_nonfinite.h"
#include "mwmathutil.h"
#include <string.h>

// Variable Definitions
static emlrtRSInfo n_emlrtRSI = { 25,  // lineNo
  "MaskGenerate",                      // fcnName
  "/Users/hsy/Library/Mobile Documents/com~apple~CloudDocs/DropboxCopy/GT/Fall2020/Senior Project1/Sound Masking/MaskGenerate.m"// pathName 
};

// Function Definitions
void MaskGenerate(MaskGenerateStackData *SD, const emlrtStack *sp, const real_T
                  d[524288], const real_T pastMask[524288], creal_T newMask
                  [524288])
{
  emlrtStack st;
  real_T w[10];
  int32_T k;
  boolean_T exitg1;
  boolean_T y;
  st.prev = sp;
  st.tls = sp->tls;

  // inputs:
  //    pastMask: Mask generated previously
  //    d: input from mic
  // output:
  //    newMask: new mask generated
  //    Assumes d and pastMask is of same length and powers of 2.
  // disp(d)
  //     %% check if pastMask is all zeros (which will be the case during initial setup) 
  y = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 524288)) {
    if (pastMask[k] == 0.0) {
      y = false;
      exitg1 = true;
    } else {
      k++;
    }
  }

  if (!y) {
    memcpy(&SD->f0.e[0], &d[0], 524288U * sizeof(real_T));

    // if paskMask was defined, do adaptive filtering.
  } else {
    // apatation factor
    // initial filter weight guess
    // adaptive filter
    memset(&w[0], 0, 10U * sizeof(real_T));
    st.site = &n_emlrtRSI;
    nlms(&st, pastMask, d, w, SD->f0.wnoise, SD->f0.e);
  }

  //     %% FFT of e (out estimated environment noise)
  emlrtFFTWSetNumThreads(4);
  emlrtFFTW_1D_R2C(&SD->f0.e[0], (real_T *)&SD->f0.E[0], 1, 524288, 524288, 1,
                   -1);
  for (k = 0; k < 524288; k++) {
    SD->f0.e[k] = muDoubleScalarHypot(SD->f0.E[k].re, SD->f0.E[k].im);
  }

  // Spectral magnitude
  for (k = 0; k < 524288; k++) {
    SD->f0.e[k] = muDoubleScalarLog10(SD->f0.e[k]);
  }

  // Spectral magnitude in dB
  //     %% Create a sound mask
  // [Mm, Ii] = samplePeak(Mmag);
  coder::randn(SD->f0.wnoise);
  emlrtFFTWSetNumThreads(4);
  emlrtFFTW_1D_R2C(&SD->f0.wnoise[0], (real_T *)&SD->f0.E[0], 1, 524288, 524288,
                   1, -1);
  for (k = 0; k < 524288; k++) {
    real_T E_im;
    real_T E_re;
    real_T ai;
    real_T b_d;
    real_T br;
    b_d = (20.0 * SD->f0.e[k] + 3.0) / 20.0;
    SD->f0.e[k] = b_d;
    E_im = SD->f0.E[k].re;
    ai = SD->f0.E[k].im;
    br = muDoubleScalarHypot(E_im, ai);
    if (ai == 0.0) {
      E_re = E_im / br;
      E_im = 0.0;
    } else if (E_im == 0.0) {
      E_re = 0.0;
      E_im = ai / br;
    } else {
      E_re = E_im / br;
      E_im = ai / br;
    }

    b_d = muDoubleScalarPower(10.0, b_d);
    SD->f0.E[k].re = b_d * E_re;
    SD->f0.E[k].im = b_d * E_im;
  }

  emlrtFFTWSetNumThreads(4);
  emlrtFFTW_1D_C2C((real_T *)&SD->f0.E[0], (real_T *)&newMask[0], 1, 524288,
                   524288, 1, 1);
}

// End of code generation (MaskGenerate.cpp)
