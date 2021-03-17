//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  MaskGenerate_types.h
//
//  Code generation for function 'MaskGenerate'
//


#pragma once

// Include files
#include "rtwtypes.h"
#include "emlrt.h"

// Type Definitions
struct MaskGenerateStackData
{
  struct {
    creal_T E[524288];
    real_T e[524288];
    real_T wnoise[524288];
  } f0;

  struct {
    creal_T newMask[524288];
  } f1;
};

// End of code generation (MaskGenerate_types.h)
