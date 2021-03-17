//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  _coder_MaskGenerate_api.cpp
//
//  Code generation for function '_coder_MaskGenerate_api'
//


// Include files
#include "_coder_MaskGenerate_api.h"
#include "MaskGenerate.h"
#include "MaskGenerate_data.h"
#include "MaskGenerate_types.h"
#include "rt_nonfinite.h"

// Function Declarations
static real_T (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[524288];
static real_T (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *d, const
  char_T *identifier))[524288];
static real_T (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[524288];
static const mxArray *emlrt_marshallOut(const emlrtStack *sp, const creal_T u
  [524288]);

// Function Definitions
static real_T (*b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId))[524288]
{
  static const int32_T dims[2] = { 1, 524288 };

  real_T (*ret)[524288];
  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 2U, dims);
  ret = (real_T (*)[524288])emlrtMxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}
  static real_T (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *d, const
  char_T *identifier))[524288]
{
  emlrtMsgIdentifier thisId;
  real_T (*y)[524288];
  thisId.fIdentifier = const_cast<const char_T *>(identifier);
  thisId.fParent = NULL;
  thisId.bParentIsCell = false;
  y = emlrt_marshallIn(sp, emlrtAlias(d), &thisId);
  emlrtDestroyArray(&d);
  return y;
}

static real_T (*emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId))[524288]
{
  real_T (*y)[524288];
  y = b_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}
  static const mxArray *emlrt_marshallOut(const emlrtStack *sp, const creal_T u
  [524288])
{
  static const int32_T iv[2] = { 1, 524288 };

  const mxArray *m;
  const mxArray *y;
  y = NULL;
  m = emlrtCreateNumericArray(2, &iv[0], mxDOUBLE_CLASS, mxCOMPLEX);
  emlrtExportNumericArrayR2013b(sp, m, (void *)&u[0], 8);
  emlrtAssign(&y, m);
  return y;
}

void MaskGenerate_api(MaskGenerateStackData *SD, const mxArray * const prhs[2],
                      const mxArray *plhs[1])
{
  emlrtStack st = { NULL,              // site
    NULL,                              // tls
    NULL                               // prev
  };

  real_T (*d)[524288];
  real_T (*pastMask)[524288];
  st.tls = emlrtRootTLSGlobal;

  // Marshall function inputs
  d = emlrt_marshallIn(&st, emlrtAlias(prhs[0]), "d");
  pastMask = emlrt_marshallIn(&st, emlrtAlias(prhs[1]), "pastMask");

  // Invoke the target function
  MaskGenerate(SD, &st, *d, *pastMask, SD->f1.newMask);

  // Marshall function outputs
  plhs[0] = emlrt_marshallOut(&st, SD->f1.newMask);
}

// End of code generation (_coder_MaskGenerate_api.cpp)
