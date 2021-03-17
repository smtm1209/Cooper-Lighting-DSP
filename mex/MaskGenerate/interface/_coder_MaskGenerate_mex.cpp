//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  _coder_MaskGenerate_mex.cpp
//
//  Code generation for function '_coder_MaskGenerate_mex'
//


// Include files
#include "_coder_MaskGenerate_mex.h"
#include "MaskGenerate_data.h"
#include "MaskGenerate_initialize.h"
#include "MaskGenerate_terminate.h"
#include "MaskGenerate_types.h"
#include "_coder_MaskGenerate_api.h"
#include "rt_nonfinite.h"

// Function Definitions
void MaskGenerate_mexFunction(MaskGenerateStackData *SD, int32_T nlhs, mxArray
  *plhs[1], int32_T nrhs, const mxArray *prhs[2])
{
  emlrtStack st = { NULL,              // site
    NULL,                              // tls
    NULL                               // prev
  };

  const mxArray *outputs[1];
  st.tls = emlrtRootTLSGlobal;

  // Check for proper number of arguments.
  if (nrhs != 2) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 2, 4,
                        12, "MaskGenerate");
  }

  if (nlhs > 1) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 12,
                        "MaskGenerate");
  }

  // Call the function.
  MaskGenerate_api(SD, prhs, outputs);

  // Copy over outputs to the caller.
  emlrtReturnArrays(1, plhs, &outputs[0]);
}

void mexFunction(int32_T nlhs, mxArray *plhs[], int32_T nrhs, const mxArray
                 *prhs[])
{
  MaskGenerateStackData *MaskGenerateStackDataGlobal = NULL;
  MaskGenerateStackDataGlobal = new MaskGenerateStackData;
  mexAtExit(&MaskGenerate_atexit);

  // Module initialization.
  MaskGenerate_initialize();
  try {
    emlrtShouldCleanupOnError(emlrtRootTLSGlobal, false);

    // Dispatch the entry-point.
    MaskGenerate_mexFunction(MaskGenerateStackDataGlobal, nlhs, plhs, nrhs, prhs);

    // Module termination.
    MaskGenerate_terminate();
  } catch (...) {
    emlrtCleanupOnException(emlrtRootTLSGlobal);
    throw;
  }

  delete MaskGenerateStackDataGlobal;
}

emlrtCTX mexFunctionCreateRootTLS()
{
  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  return emlrtRootTLSGlobal;
}

// End of code generation (_coder_MaskGenerate_mex.cpp)
