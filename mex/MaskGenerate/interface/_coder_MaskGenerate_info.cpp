//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  _coder_MaskGenerate_info.cpp
//
//  Code generation for function 'MaskGenerate'
//


// Include files
#include "_coder_MaskGenerate_info.h"
#include "emlrt.h"
#include "tmwtypes.h"

// Function Declarations
static const mxArray *emlrtMexFcnResolvedFunctionsInfo();

// Function Definitions
static const mxArray *emlrtMexFcnResolvedFunctionsInfo()
{
  const mxArray *nameCaptureInfo;
  const char_T *data[5] = {
    "789cc593cf4ec24010c61783c40bcac98b17ae5e5c2431f10c146222062d5c3426967694c2fea9bbad520f90f8021e8c4fe14bf8181e7d1469a1652134901071"
    "9266faebd7ee3733bb45a9b37a0a21b43bba3e87087d655018d909e7c688b6d06ccceba90539d0a3d846e990b373efbd4db2c9990b7d770ccca0107f69716a33",
    "83b94ddf01244072f20456a8dcdb049a36055d858b806855916208a4e0bedc01b3a77b14898e9c56485488e7f193d06f7ac5790c1556e7919b70f4e846bbc52d"
    "0942e28ef4f1b9dd1686f0719db747f5e62bdcf42830576293d381e138040665c23d6b24485c11dc69f37e993b3eae3571d520a4582816b00ecce622df10bc0b",
    "a67b8c75ee312b5f3764cf660f38c83560200c178e68dcefdd9afda6159eed77bcff8c50a9ce775dbf4ca25f26648b7b6d0253bfef35fd9e13fdc61ce91bdfcf"
    "60b02bece3de8a7d26fdc759b413f2fb4735b4d8945ffff5e070937e51fc975f3f61bd55cfe97e825f744e23dd6b9d3e960ae5aee6ebc6c975a972a55f6a2fda",
    "b48ec6129f6575208517f5f957ebff0295b2dccc", "" };

  nameCaptureInfo = NULL;
  emlrtNameCaptureMxArrayR2016a(&data[0], 1752U, &nameCaptureInfo);
  return nameCaptureInfo;
}

mxArray *emlrtMexFcnProperties()
{
  mxArray *xEntryPoints;
  mxArray *xInputs;
  mxArray *xResult;
  const char_T *epFieldName[6] = { "Name", "NumberOfInputs", "NumberOfOutputs",
    "ConstantInputs", "FullPath", "TimeStamp" };

  const char_T *propFieldName[4] = { "Version", "ResolvedFunctions",
    "EntryPoints", "CoverageInfo" };

  xEntryPoints = emlrtCreateStructMatrix(1, 1, 6, epFieldName);
  xInputs = emlrtCreateLogicalMatrix(1, 2);
  emlrtSetField(xEntryPoints, 0, "Name", emlrtMxCreateString("MaskGenerate"));
  emlrtSetField(xEntryPoints, 0, "NumberOfInputs", emlrtMxCreateDoubleScalar(2.0));
  emlrtSetField(xEntryPoints, 0, "NumberOfOutputs", emlrtMxCreateDoubleScalar
                (1.0));
  emlrtSetField(xEntryPoints, 0, "ConstantInputs", xInputs);
  emlrtSetField(xEntryPoints, 0, "FullPath", emlrtMxCreateString(
    "/Users/hsy/Library/Mobile Documents/com~apple~CloudDocs/DropboxCopy/GT/Fall2020/Senior Project1/Sound Masking/MaskGenerate.m"));
  emlrtSetField(xEntryPoints, 0, "TimeStamp", emlrtMxCreateDoubleScalar
                (738223.88039351848));
  xResult = emlrtCreateStructMatrix(1, 1, 4, propFieldName);
  emlrtSetField(xResult, 0, "Version", emlrtMxCreateString(
    "9.9.0.1570001 (R2020b) Update 4"));
  emlrtSetField(xResult, 0, "ResolvedFunctions", (mxArray *)
                emlrtMexFcnResolvedFunctionsInfo());
  emlrtSetField(xResult, 0, "EntryPoints", xEntryPoints);
  return xResult;
}

// End of code generation (_coder_MaskGenerate_info.cpp)
