/*
* Copyright (c) 2019, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file     vphal_g12_tgllp.h
//! \brief    vphal interface clarification for GEN12 TGL LP
//! \details  vphal interface clarification for GEN12 TGL LP inlcuding:
//!           some marcro, enum, structure, function
//!
#ifndef __VPHAL_G12TGLLP_H__
#define __VPHAL_G12TGLLP_H__

#include "vphal.h"

//!
//! Class VphalStateG12Tgllp
//! \brief VPHAL class definition for GEN12 TGL LP
//!
class VphalStateG12Tgllp : public VphalState
{
public:
    //!
    //! \brief    VphalState Constructor
    //! \details  Creates instance of VphalState
    //!           - Caller must call Allocate to allocate all VPHAL states and objects.
    //! \param    [in] pOsInterface
    //!           OS interface, if provided externally - may be NULL
    //! \param    [in] pOsDriverContext
    //!           OS driver context (UMD context, pShared, ...)
    //! \param    [in,out] pStatus
    //!           Pointer to the MOS_STATUS flag.
    //!           Will assign this flag to MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    VphalStateG12Tgllp(
        PMOS_INTERFACE          pOsInterface,
        PMOS_CONTEXT            pOsDriverContext,
        MOS_STATUS              *peStatus) :
        VphalState(pOsInterface, pOsDriverContext, peStatus)
    {
        // check the peStatus returned from VphalState
        MOS_STATUS eStatus = peStatus ? (*peStatus) : MOS_STATUS_SUCCESS;

        if (MOS_FAILED(eStatus))
        {
            VPHAL_PUBLIC_ASSERTMESSAGE("VphalStateG12Tgllp construct failed due to VphalState() returned failure: eStatus = %d", eStatus);
            return;
        }

        bool                        bComputeContextEnabled;

        // bComputeContextEnabled is true only if Gen12+ for compute context(CCS). Gen12+, compute context(MOS_GPU_CONTEXT_COMPUTE)
        // can be used for media kernel. Before Gen12, we only use MOS_GPU_CONTEXT_RENDER which we know as RCS for media kernel.
        // On TGLLP, CCS has a HW limition for Luma-key/Chroma key feature since 3DSTATE_CHROMA_KEY support for ComputeCS.
        // Therefore, RCS is by Default on TGLLP.
        bComputeContextEnabled      = false;

#if (_DEBUG || _RELEASE_INTERNAL)
        bool       computeContextEnabled = false;
        MOS_STATUS                  eRegKeyReadStatus = MOS_STATUS_SUCCESS;
        eRegKeyReadStatus = ReadUserSettingForDebug(
            m_userSettingPtr,
            computeContextEnabled,
            __VPHAL_ENABLE_COMPUTE_CONTEXT,
            MediaUserSetting::Group::Sequence);
        if (eRegKeyReadStatus == MOS_STATUS_SUCCESS)
        {
            bComputeContextEnabled = computeContextEnabled ? true : false;
        }
#endif

        if (!MEDIA_IS_SKU(m_skuTable, FtrCCSNode))
        {
            bComputeContextEnabled = false;
        }

        if (bComputeContextEnabled)
        {
            m_renderGpuContext    = MOS_GPU_CONTEXT_COMPUTE;
            m_renderGpuNode       = MOS_GPU_NODE_COMPUTE;
        }
    }

    //!
    //! \brief    VphalState Destuctor
    //! \details  Destroys VPHAL and all internal states and objects
    //! \return   VOID
    //!
    ~VphalStateG12Tgllp()
    {
    }

    //!
    //! \brief    Allocate VPHAL Resources
    //! \details  Allocate VPHAL Resources
    //!           - Allocate and initialize HW states
    //!           - Allocate and initialize renderer states
    //! \param    [in] pVpHalSettings
    //!           Pointer to VPHAL Settings
    //! \return   MOS_STATUS
    //!           Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS Allocate(
        const VphalSettings *pVpHalSettings);

protected:
    //!
    //! \brief    Create instance of VphalRenderer
    //! \details  Create instance of VphalRenderer
    //! \return   MOS_STATUS
    //!           Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    MOS_STATUS CreateRenderer();
};

#endif  // __VPHAL_G12TGLLP_H__
