// Copyright 2016-present, Facebook, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the
// LICENSE file in the root directory of this source tree.

#include "UnPooling.h"

template <typename T, Int Dimension>
void cpu_UnPooling_updateOutput(
    /*long*/ at::Tensor inputSize, /*long*/ at::Tensor outputSize,
    /*long*/ at::Tensor poolSize,
    /*long*/ at::Tensor poolStride, Metadata<Dimension> &m,
    /*float*/ at::Tensor input_features,
    /*float*/ at::Tensor output_features, long nFeaturesToDrop) {

  Int nPlanes = input_features.size(1) - nFeaturesToDrop;
  auto _rules =
      m.getRuleBook(outputSize, inputSize, poolSize, poolStride, true);
  Int nActive = m.getNActive(outputSize);
  output_features.resize_({nActive, input_features.size(1) - nFeaturesToDrop});
  output_features.zero_();

  auto iF = input_features.data<T>() + nFeaturesToDrop;
  auto oF = output_features.data<T>();

  for (auto &r : _rules) {
    Int nHot = r.size() / 2;
    UnPooling_ForwardPass<T>(iF, oF, nPlanes, input_features.size(1),
                             output_features.size(1), &r[0], nHot);
  }
}
template <typename T, Int Dimension>
void cpu_UnPooling_updateGradInput(
    /*long*/ at::Tensor inputSize, /*long*/ at::Tensor outputSize,
    /*long*/ at::Tensor poolSize,
    /*long*/ at::Tensor poolStride, Metadata<Dimension> &m,
    /*float*/ at::Tensor input_features,
    /*float*/ at::Tensor d_input_features,
    /*float*/ at::Tensor d_output_features, long nFeaturesToDrop) {

  Int nPlanes = input_features.size(1) - nFeaturesToDrop;
  auto _rules =
      m.getRuleBook(outputSize, inputSize, poolSize, poolStride, true);
  d_input_features.resize_as_(input_features);
  d_input_features.zero_();

  auto diF = d_input_features.data<T>() + nFeaturesToDrop;
  auto doF = d_output_features.data<T>();

  for (auto &r : _rules) {
    Int nHot = r.size() / 2;
    UnPooling_BackwardPass<T>(diF, doF, nPlanes, input_features.size(1),
                              d_output_features.size(1), &r[0], nHot);
  }
}
