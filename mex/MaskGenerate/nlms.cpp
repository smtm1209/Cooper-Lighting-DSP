//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  nlms.cpp
//
//  Code generation for function 'nlms'
//


// Include files
#include "nlms.h"
#include "MaskGenerate_data.h"
#include "rt_nonfinite.h"
#include <string.h>

// Function Definitions
void nlms(const emlrtStack *sp, const real_T x[524288], const real_T d[524288],
          real_T w[10], real_T dhat[524288], real_T e[524288])
{
  real_T b_xx[10];
  real_T b_w;
  real_T xx;

  // x: speakr signal
  // d: target signal
  // mu: beta
  // w: initial weight guess
  // dhat: filter output(predicted value)
  memset(&dhat[0], 0, 524288U * sizeof(real_T));
  memset(&e[0], 0, 524288U * sizeof(real_T));

  //  now implement the adaptive filter
  for (int32_T n = 0; n < 524279; n++) {
    int32_T i;

    //  produce filtered output sample
    b_w = 0.0;

    //  update the filter coefficients
    xx = 0.0;
    for (i = 0; i < 10; i++) {
      real_T b_d;
      b_d = x[(n - i) + 9];
      b_xx[i] = b_d;
      b_w += w[i] * b_d;
      xx += b_d * b_d;
    }

    dhat[n + 9] = b_w;
    b_w = d[n + 9] - b_w;
    e[n + 9] = b_w;
    for (i = 0; i < 10; i++) {
      w[i] += 0.0004 * b_xx[i] * b_w / xx;
    }

    if (*emlrtBreakCheckR2012bFlagVar != 0) {
      emlrtBreakCheckR2012b(sp);
    }
  }
}

// End of code generation (nlms.cpp)
