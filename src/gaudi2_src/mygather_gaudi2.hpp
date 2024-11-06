/**********************************************************************
Copyright (c) 2024 Habana Labs. All rights reserved.

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

#ifndef _PSCAN_GAUDI2_HPP
#define _PSCAN_GAUDI2_HPP

#include "gc_interface.h"
#include "tpc_kernel_lib_interface.h"

class MygatherGaudi2
{
public:
    typedef enum _mygather_mode_t
    {
        mygather_f32,
        mygather_bf16,
    } mygather_mode_t;

    MygatherGaudi2(mygather_mode_t mode=mygather_f32) {m_mode = mode;}
    virtual ~MygatherGaudi2() {}

    virtual tpc_lib_api::GlueCodeReturn GetGcDefinitions(
            tpc_lib_api::HabanaKernelParams* params,
            tpc_lib_api::HabanaKernelInstantiation* kernel);

    virtual tpc_lib_api::GlueCodeReturn GetKernelName(
            char kernelName [tpc_lib_api::MAX_NODE_NAME], mygather_mode_t mode);
    
    struct MygatherParam
    {
        int max_ctx_len;
    };    
private:
    mygather_mode_t m_mode;
    MygatherGaudi2(const MygatherGaudi2& other) = delete;
    MygatherGaudi2& operator=(const MygatherGaudi2& other) = delete;
};


#endif //_MYGATHER_GAUDI2_HPP