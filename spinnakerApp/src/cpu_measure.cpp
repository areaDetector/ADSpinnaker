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

#include <stdio.h>
#include "cpu_measure.h"

CpuMeasure::CpuMeasure()
{
    m_process_id = getpid();
    m_num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    m_user_cpu_time_process_start = m_user_cpu_time_process_stop = m_kernel_cpu_time_process_start =
        m_kernel_cpu_time_process_stop = 0;

    printf("processID = %d, numCPU = %d\n", m_process_id, m_num_cpus);
}

CpuMeasure::~CpuMeasure()
{
}

bool CpuMeasure::Start()
{
    char path[64];

    sprintf(path, "/proc/%d/stat", m_process_id);

    FILE* fd = fopen(path, "r");

    if (!fd)
    {
        printf("Failed to open %s for reading!\n", path);
        return false;
    }

    fscanf(
        fd,
        "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",
        &m_user_cpu_time_process_start,
        &m_kernel_cpu_time_process_start);
    fclose(fd);

    fd = fopen("/proc/stat", "r");

    if (!fd)
    {
        printf("Failed to open /proc/stat for reading!\n");
        return false;
    }

    fscanf(
        fd,
        "%*s %llu %llu %llu %llu",
        &m_user_cpu_time_system_start,
        &m_nice_cpu_time_system_start,
        &m_kernel_cpu_time_system_start,
        &m_idle_cpu_time_system_start);
    fclose(fd);

    return true;
}

bool CpuMeasure::Stop()
{
    char path[64];

    sprintf(path, "/proc/%d/stat", m_process_id);

    FILE* fd = fopen(path, "r");

    if (!fd)
    {
        printf("Failed to open %s for reading!\n", path);
        return false;
    }

    fscanf(
        fd,
        "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",
        &m_user_cpu_time_process_stop,
        &m_kernel_cpu_time_process_stop);
    fclose(fd);

    fd = fopen("/proc/stat", "r");

    if (!fd)
    {
        printf("Failed to open /proc/stat for reading!\n");
        return false;
    }

    fscanf(
        fd,
        "%*s %llu %llu %llu %llu",
        &m_user_cpu_time_system_stop,
        &m_nice_cpu_time_system_stop,
        &m_kernel_cpu_time_system_stop,
        &m_idle_cpu_time_system_stop);
    fclose(fd);

    return true;
}

float CpuMeasure::GetCPUTime()
{
    unsigned long cpu_start = m_user_cpu_time_process_start + m_kernel_cpu_time_process_start;
    unsigned long cpu_stop = m_user_cpu_time_process_stop + m_kernel_cpu_time_process_stop;

    unsigned long long cpu_global_start = m_user_cpu_time_system_start + m_kernel_cpu_time_system_start +
                                          m_nice_cpu_time_system_start + m_idle_cpu_time_system_start;
    unsigned long long cpu_global_stop = m_user_cpu_time_system_stop + m_kernel_cpu_time_system_stop +
                                         m_nice_cpu_time_system_stop + m_idle_cpu_time_system_stop;

    float cpu_usage =
        ((cpu_stop - cpu_start) / (float)(cpu_global_stop - cpu_global_start)) * 100.0; // / (float)m_num_cpus;

    return cpu_usage;
}

float CpuMeasure::GetUserTime()
{
    unsigned long long cpu_global_start = m_user_cpu_time_system_start + m_kernel_cpu_time_system_start +
                                          m_nice_cpu_time_system_start + m_idle_cpu_time_system_start;
    unsigned long long cpu_global_stop = m_user_cpu_time_system_stop + m_kernel_cpu_time_system_stop +
                                         m_nice_cpu_time_system_stop + m_idle_cpu_time_system_stop;

    float cpu_usage =
        ((m_user_cpu_time_process_stop - m_user_cpu_time_process_start) / (float)(cpu_global_stop - cpu_global_start)) *
        100.0; // / (float)m_num_cpus;

    return cpu_usage;
}

float CpuMeasure::GetKernelTime()
{
    unsigned long long cpu_global_start = m_user_cpu_time_system_start + m_kernel_cpu_time_system_start +
                                          m_nice_cpu_time_system_start + m_idle_cpu_time_system_start;
    unsigned long long cpu_global_stop = m_user_cpu_time_system_stop + m_kernel_cpu_time_system_stop +
                                         m_nice_cpu_time_system_stop + m_idle_cpu_time_system_stop;

    float cpu_usage = ((m_kernel_cpu_time_process_stop - m_kernel_cpu_time_process_start) /
                       (float)(cpu_global_stop - cpu_global_start)) *
                      100.0; // / (float)m_num_cpus;

    return cpu_usage;
}

float CpuMeasure::GetGlobalUserTime()
{
    unsigned long long cpu_global_start = m_user_cpu_time_system_start + m_kernel_cpu_time_system_start +
                                          m_nice_cpu_time_system_start + m_idle_cpu_time_system_start;
    unsigned long long cpu_global_stop = m_user_cpu_time_system_stop + m_kernel_cpu_time_system_stop +
                                         m_nice_cpu_time_system_stop + m_idle_cpu_time_system_stop;

    return ((m_user_cpu_time_system_stop - m_user_cpu_time_system_start) /
            (float)(cpu_global_stop - cpu_global_start)) *
           100.0; // / (float)m_num_cpus;
}

float CpuMeasure::GetGlobalKernelTime()
{
    unsigned long long cpu_global_start = m_user_cpu_time_system_start + m_kernel_cpu_time_system_start +
                                          m_nice_cpu_time_system_start + m_idle_cpu_time_system_start;
    unsigned long long cpu_global_stop = m_user_cpu_time_system_stop + m_kernel_cpu_time_system_stop +
                                         m_nice_cpu_time_system_stop + m_idle_cpu_time_system_stop;

    return ((m_kernel_cpu_time_system_stop - m_kernel_cpu_time_system_start) /
            (float)(cpu_global_stop - cpu_global_start)) *
           100.0; // / (float)m_num_cpus;
}