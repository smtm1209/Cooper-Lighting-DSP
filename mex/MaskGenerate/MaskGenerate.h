//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  MaskGenerate.h
//
//  Code generation for function 'MaskGenerate'
//


#pragma once

// Include files
#include "MaskGenerate_types.h"
#include "rtwtypes.h"
#include "emlrt.h"
#include "mex.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Function Declarations
void MaskGenerate(MaskGenerateStackData *SD, const emlrtStack *sp, const real_T
                  d[524288], const real_T pastMask[524288], creal_T newMask
                  [524288]);

// End of code generation (MaskGenerate.h)
