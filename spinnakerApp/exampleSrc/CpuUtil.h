//=============================================================================
// Copyright (c) 2001-2023 FLIR Systems, Inc. All Rights Reserved.
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
#include "stdafx.h"
#include <Windows.h>
#include <string>
#include "CpuUtil.h"
#include <stdio.h>
#include <ostream>
#include "Spinnaker.h" //searches include path //local project file
#include "SpinGenApi/SpinnakerGenApi.h"
#include <math.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <regex>
#include <random>
#include <stdio.h>
#include <Windows.h>
#include <Process.h>

using namespace std;
//=============================================================================
// CPU Usage Helpers
//=============================================================================
namespace CpuUtil
{
#ifdef _WIN32
    struct CpuUsageInfo
    {
        CpuUsageInfo()
        {
            kernelStartTime = FILETIME();
            kernelStopTime = FILETIME();
            userStartTime = FILETIME();
            userStopTime = FILETIME();

            trackingStartTime = FILETIME();
            trackingStopTime = FILETIME();

            kernelSystemTime = SYSTEMTIME();
            userSystemTime = SYSTEMTIME();
            cpuPercentage = 0;
            kernelTimeMilliseconds = 0;
            userTimeMilliseconds = 0;
            elapsedTime = 0;
        }

        FILETIME kernelStartTime;
        FILETIME kernelStopTime;
        FILETIME userStartTime;
        FILETIME userStopTime;

        FILETIME trackingStartTime;
        FILETIME trackingStopTime;

        double cpuPercentage;
        SYSTEMTIME kernelSystemTime;
        SYSTEMTIME userSystemTime;
        double kernelTimeMilliseconds;
        double userTimeMilliseconds;
        double elapsedTime;
    };
#else
    struct CpuUsageInfo
    {
        bool dummy;
    };
#endif

    bool StartCpuTracing(CpuUsageInfo* cpuUsage);
    bool StopCpuTracing(CpuUsageInfo* cpuUsage);
    std::string GetCpuStats(CpuUsageInfo* cpuUsage);
} // namespace CpuUtil
//=============================================================================
// Performance counter
//=============================================================================
namespace PerformanceCounter
{

    void StartPerformanceCounter(); // Starts or resets counter
    double GetPerformanceCounter();
} // namespace PerformanceCounter
//=============================================================================
// Seconds counter
//=============================================================================
namespace SecondsCounter
{
    void StartSecondsCounter();
    int GetSecondsCounter();
} // namespace SecondsCounter
//=============================================================================
// Type conversion
//=============================================================================
namespace Conversion
{
    string NumToCString(int number);
    string NumToCString(double number);
} // namespace Conversion