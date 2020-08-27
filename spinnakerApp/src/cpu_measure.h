//=============================================================================
// Copyright (c) 2001-2019 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

#pragma once

#include <sys/types.h>
#include <unistd.h>

class CpuMeasure
{
  public:
    CpuMeasure();
    ~CpuMeasure();

    bool Start();
    bool Stop();

    float GetCPUTime();
    float GetUserTime();
    float GetKernelTime();

    float GetGlobalUserTime();
    float GetGlobalKernelTime();

  private:
    pid_t m_process_id;
    unsigned int m_num_cpus;

    unsigned long m_user_cpu_time_process_start;
    unsigned long m_kernel_cpu_time_process_start;
    unsigned long m_user_cpu_time_process_stop;
    unsigned long m_kernel_cpu_time_process_stop;

    unsigned long long m_user_cpu_time_system_start;
    unsigned long long m_kernel_cpu_time_system_start;
    unsigned long long m_user_cpu_time_system_stop;
    unsigned long long m_kernel_cpu_time_system_stop;

    unsigned long long m_nice_cpu_time_system_start;
    unsigned long long m_idle_cpu_time_system_start;
    unsigned long long m_nice_cpu_time_system_stop;
    unsigned long long m_idle_cpu_time_system_stop;

    unsigned long long m_system_wide_user_start;
    unsigned long long m_system_wide_user_stop;
};