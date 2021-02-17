/**********************************************************************
Copyright (c) 2018 Habana Labs.

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

#ifndef FILTER_2D_F32_TEST_HPP
#define FILTER_2D_F32_TEST_HPP

#include "test_base.hpp"
#include "tensor.h"
#include "filter_2d_f32.hpp"

class Filter2DF32Test : public TestBase
{
public:
    Filter2DF32Test() {}
    ~Filter2DF32Test() {}
    int runTest();

    inline static void filter_2d_reference_implementation(
        const test::Tensor<float,4>& ifm,
        const test::Tensor<float,3>& filter,
        test::Tensor<float,4>& ofm,
        const SpatialReductionKernels::SpatialReduction2DDef& layer_def,
        const IndexSpace& indexSpace);
private:
    Filter2DF32Test(const Filter2DF32Test& other) = delete;
    Filter2DF32Test& operator=(const Filter2DF32Test& other) = delete;

};


inline void Filter2DF32Test::filter_2d_reference_implementation(
        const test::Tensor<float,4>& ifm,
        const test::Tensor<float,3>& filter,
        test::Tensor<float,4>& ofm,
        const SpatialReductionKernels::SpatialReduction2DDef& layer_def,
        const IndexSpace& indexSpace)
{
    int output_coords [4] = {0};
    for (int b = indexSpace.offset[3]; b < indexSpace.offset[3]+indexSpace.size[3]; b += 1)
    {
        output_coords[3] = b;
        for (int h = indexSpace.offset[2]; h < indexSpace.offset[2]+indexSpace.size[2]; h += 1)
        {
            output_coords[2] = h;
            for (int w = indexSpace.offset[1]; w < indexSpace.offset[1]+indexSpace.size[1]; w += 1)
            {
                output_coords[1] = w;
                for (int d = (int) indexSpace.offset[0];
                     d < (int)((indexSpace.offset[0]+indexSpace.size[0])*(256 /sizeof(float)));
                     d += 1)
                {
                   int filterCoords [] = {d,0,0};
                   output_coords[0] = d;
                   float accum = {0};
                   //this loop unroll purpose is to hide pipe latency
                   for (int kh = 0 ; kh <  layer_def.kernel_h; kh++)
                   {
                        filterCoords[2] = kh;
                        for (int kw = 0 ; kw <  layer_def.kernel_w; kw++)
                        {
                            filterCoords[1] = kw;
                            int ifmCoords []= { d,
                                (layer_def.stride_w*w) -layer_def.pad_w + (kw * layer_def.dilation_w),
                                (layer_def.stride_h*h) -layer_def.pad_h + (kh * layer_def.dilation_h),
                                 b};
                            float filterValue = filter.ElementAt(filterCoords);
                            float ifmVector = ifm.ElementAt(ifmCoords);
                            accum += filterValue*ifmVector;
                        }
                    }
                    ofm.SetElement(output_coords,accum);
                }
            }
        }
    }
}

 inline int Filter2DF32Test::runTest()
 {
    const int fm_height = 5;
    const int fm_width  = 5;
    const int fm_depth = 100;
    const int fm_batch = 1;

    SpatialReductionKernels::SpatialReduction2DDef layer_def;
    layer_def.pad_w = 1;
    layer_def.pad_h = 1;
    layer_def.kernel_h = 3;
    layer_def.kernel_w = 3;
    layer_def.stride_h = 1;
    layer_def.stride_w = 1;
    layer_def.dilation_w = 1;
    layer_def.dilation_h = 1;


    unsigned int fmInitializer[] = {fm_depth, fm_width, fm_height, fm_batch};
    float_4DTensor ifm(fmInitializer);
    ifm.FillWithData();
    unsigned int filterInitialize[] = {(unsigned)fm_depth,
                                       (unsigned)layer_def.kernel_w,
                                       (unsigned)layer_def.kernel_h};

    float_3DTensor filter (filterInitialize);
    filter.FillWithData();

    float_4DTensor ofm(fmInitializer);
    float_4DTensor ofm_ref(fmInitializer);

    IndexSpace indexSpace = {{0}};
    int depthIS = (fm_depth + 63) / 64 ;
    indexSpace.size[0] = depthIS;
    indexSpace.size[1] = fm_width;
    indexSpace.size[2] = fm_height;
    indexSpace.size[3] = fm_batch;

    // execute reference implementation of the kernel.
    filter_2d_reference_implementation(ifm,
                                    filter,
                                    ofm_ref,
                                    layer_def,
                                    indexSpace);
    // generate input for query call
    m_in_defs.NodeParams = &layer_def;
    m_in_defs.inputTensorNr = 2;
    LoadTensorToGcDescriptor(&(m_in_defs.inputTensors[0]),ifm );
    LoadTensorToGcDescriptor(&(m_in_defs.inputTensors[1]),filter );

    m_in_defs.outputTensorNr = 1;
    LoadTensorToGcDescriptor(&(m_in_defs.outputTensors[0]),ofm );

    Filter2dF32 kernelClass;
    // make the call into the glue code.
    gcapi::GlueCodeReturn_t result = kernelClass.GetGcDefinitions(&m_in_defs,&m_out_defs);
    if (result != gcapi::GLUE_SUCCESS)
    {
        std::cout << "glue test failed!! " << result << std::endl;
        return -1;
    }

    // generate and load tensor descriptors
    std::vector<TensorDescriptor> vec;
    vec.push_back(ifm.GetTensorDescriptor());
    vec.push_back(filter.GetTensorDescriptor());
    vec.push_back(ofm.GetTensorDescriptor());
    // execute a simulation of the kernel using TPC simulator,
    TestBase::RunSimulation(vec, m_in_defs, m_out_defs);
    std::cout << std::endl;
    std::cout << "ofm data shown below " << std::endl;
    ofm.Print(0);
    std::cout << std::endl;
    std::cout << "ofm_ref data shown below " << std::endl;
    ofm_ref.Print(0);
    for (int element = 0 ; element <  ofm_ref.ElementCount() ; element++)
    {
        if (ofm.Data()[element] != ofm_ref.Data()[element])
        {
            std::cout << "Filter2DF32Test failed!!" << std::endl;
            return -1;
        }
    }
    std::cout << "Filter2DF32Test pass!!" << std::endl;
    std::cout << std::endl;
    return 0;
 }

#endif /* FILTER_2D_F32_TEST_HPP */

