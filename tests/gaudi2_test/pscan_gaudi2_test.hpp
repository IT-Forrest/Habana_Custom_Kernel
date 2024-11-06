/**********************************************************************
Copyright (c) 2024 Habana Labs.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef PSCAN_GAUDI2_TEST_HPP
#define PSCAN_GAUDI2_TEST_HPP

#include "test_base.hpp"
#include "tensor.h"
#include "pscan_gaudi2.hpp"
#include "entry_points.hpp"

class PscanGaudi2Test : public TestBase
{
public:
    PscanGaudi2Test() {}
    ~PscanGaudi2Test() {}
    int runTest(Gaudi2_Kernel_Name_e NameofKernel);

    static void pscan_fp32_ref(
         const test::Tensor<float,4>& state_M,
         const test::Tensor<float,4>& x_M,
         const test::Tensor<float,4>& dt_M,
         const test::Tensor<float,4>& A_M,
         const test::Tensor<float,4>& B_M,
         test::Tensor<float,4>& state_out,
         const IndexSpace& indexSpace);

    static void pscan_bf16_ref(
         const test::Tensor<bfloat16,4>& state_M,
         const test::Tensor<bfloat16,4>& x_M,
         const test::Tensor<bfloat16,4>& dt_M,
         const test::Tensor<bfloat16,4>& A_M,
         const test::Tensor<bfloat16,4>& B_M,
         test::Tensor<bfloat16,4>& state_out,
         const IndexSpace& indexSpace);
private:
    PscanGaudi2Test(const PscanGaudi2Test& other) = delete;
    PscanGaudi2Test& operator=(const PscanGaudi2Test& other) = delete;

};

#endif /* PSCAN_GAUDI2_TEST_HPP */
