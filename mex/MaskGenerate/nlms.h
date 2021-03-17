//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  nlms.h
//
//  Code generation for function 'nlms'
//


#pragma once

// Include files
#include "rtwtypes.h"
#include "emlrt.h"
#include "mex.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Function Declarations
void nlms(const emlrtStack *sp, const real_T x[524288], const real_T d[524288],
          real_T w[10], real_T dhat[524288], real_T e[524288]);

// End of code generation (nlms.h)
