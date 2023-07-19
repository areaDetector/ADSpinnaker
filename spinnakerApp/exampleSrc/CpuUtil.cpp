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
//#include "stdafx.h"
#include "CpuUtil.h"
using namespace std;

//=============================================================================
// CPU Usage Services
//=============================================================================
namespace CpuUtil
{

#ifdef _WIN32
    bool StartCpuTracing(CpuUsageInfo* cpuUsage)
    {
        FILETIME creationTimeDummy, exitTimeDummy;

        GetSystemTimeAsFileTime(&cpuUsage->trackingStartTime);

        BOOL retVal = GetProcessTimes(
            GetCurrentProcess(),
            &creationTimeDummy,
            &exitTimeDummy,
            &cpuUsage->kernelStartTime,
            &cpuUsage->userStartTime);

        return (retVal == TRUE);
    }

    bool StopCpuTracing(CpuUsageInfo* cpuUsage)
    {
        FILETIME creationTimeDummy, exitTimeDummy;

        BOOL retVal = GetProcessTimes(
            GetCurrentProcess(),
            &creationTimeDummy,
            &exitTimeDummy,
            &(cpuUsage->kernelStopTime),
            &(cpuUsage->userStopTime));

        GetSystemTimeAsFileTime(&cpuUsage->trackingStopTime);

        return (retVal == TRUE);
    }

    std::string GetCpuStats(CpuUsageInfo* cpuUsage)
    {
        ULARGE_INTEGER kernelStartTimeInt, kernelStopTimeInt;
        ULARGE_INTEGER userStartTimeInt, userStopTimeInt;
        ULARGE_INTEGER trackingStartTimeInt, trackingStopTimeInt;

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        memcpy(&kernelStartTimeInt, &cpuUsage->kernelStartTime, sizeof(FILETIME));
        memcpy(&kernelStopTimeInt, &cpuUsage->kernelStopTime, sizeof(FILETIME));

        memcpy(&userStartTimeInt, &cpuUsage->userStartTime, sizeof(FILETIME));
        memcpy(&userStopTimeInt, &cpuUsage->userStopTime, sizeof(FILETIME));

        memcpy(&trackingStartTimeInt, &cpuUsage->trackingStartTime, sizeof(FILETIME));
        memcpy(&trackingStopTimeInt, &cpuUsage->trackingStopTime, sizeof(FILETIME));

        SYSTEMTIME kernelSystemTime, userSystemTime;
        FILETIME kernelDiffTime, userDiffTime;
        ULARGE_INTEGER kernelDiffTimeInt, userDiffTimeInt;

        kernelDiffTimeInt.QuadPart = kernelStopTimeInt.QuadPart - kernelStartTimeInt.QuadPart;
        userDiffTimeInt.QuadPart = userStopTimeInt.QuadPart - userStartTimeInt.QuadPart;

        memcpy(&kernelDiffTime, &kernelDiffTimeInt.QuadPart, sizeof(FILETIME));
        memcpy(&userDiffTime, &userDiffTimeInt.QuadPart, sizeof(FILETIME));

        FileTimeToSystemTime(&kernelDiffTime, &kernelSystemTime);
        FileTimeToSystemTime(&userDiffTime, &userSystemTime);

        double cpuPercentage = static_cast<double>(
                                   (kernelStopTimeInt.QuadPart - kernelStartTimeInt.QuadPart +
                                    userStopTimeInt.QuadPart - userStartTimeInt.QuadPart)) /
                               static_cast<double>(trackingStopTimeInt.QuadPart - trackingStartTimeInt.QuadPart) /
                               static_cast<double>(sysInfo.dwNumberOfProcessors) * 100.0;

        cpuUsage->cpuPercentage = cpuPercentage;
        cpuUsage->kernelSystemTime = kernelSystemTime;
        cpuUsage->userSystemTime = userSystemTime;

        // Total Elapsed Time in milliseconds:
        cpuUsage->elapsedTime =
            static_cast<double>(trackingStopTimeInt.QuadPart - trackingStartTimeInt.QuadPart) / 10000000;

        // Total Kernel Time in milliseconds:
        cpuUsage->kernelTimeMilliseconds = ((double)kernelSystemTime.wHour) * 60 * 60 * 1000 +
                                           ((double)kernelSystemTime.wMinute) * 60 * 1000 +
                                           ((double)kernelSystemTime.wSecond) * 1000;

        // Total User Time in milliseconds:
        cpuUsage->userTimeMilliseconds = ((double)kernelSystemTime.wHour) * 60 * 60 * 1000 +
                                         ((double)kernelSystemTime.wMinute) * 60 * 1000 +
                                         ((double)kernelSystemTime.wSecond) * 1000;

        std::ostringstream ss;
        ss << "Kernel Time: " << kernelSystemTime.wHour << ":" << kernelSystemTime.wMinute << ":"
           << kernelSystemTime.wSecond << "." << kernelSystemTime.wMilliseconds << "\n";
        ss << "User Time: " << userSystemTime.wHour << ":" << userSystemTime.wMinute << ":" << userSystemTime.wSecond
           << "." << userSystemTime.wMilliseconds << "\n";
        ss << "CPU Usage: " << cpuPercentage << "%\n";
        ss << "Elapsed Time: " << cpuUsage->elapsedTime << endl;
        std::string result(ss.str());
        return result;
    }
#else
    bool StartCpuTracing(CpuUsageInfo* cpuUsage)
    {
        return false;
    }

    bool StopCpuTracing(CpuUsageInfo* cpuUsage)
    {
        return false;
    }

    std::string GetCpuStats(CpuUsageInfo* cpuUsage)
    {
        return std::string("CPU Stat collection under Linux not implemented.\n");
    }
#endif

} // namespace CpuUtil

//=============================================================================
// Performance counter
//=============================================================================
namespace PerformanceCounter
{
    double PCFreq;
    __int64 CounterStart;

    void StartPerformanceCounter()
    {
        PCFreq = 0.0;
        CounterStart = 0;
        LARGE_INTEGER li;
        if (!QueryPerformanceFrequency(&li))
            cout << "QueryPerformanceFrequency failed!\n";
        PCFreq = double(li.QuadPart) / 1000.0; // milliseconds
        QueryPerformanceCounter(&li);
        CounterStart = li.QuadPart;
    }
    double GetPerformanceCounter()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return double(li.QuadPart - CounterStart) / PCFreq;
    }

} // namespace PerformanceCounter
//=============================================================================
// Seconds counter
//=============================================================================
namespace SecondsCounter
{
    time_t startTime, endTime; // tracking stream duration (accurate to one second)
    double timeDiff;           // This is double but diffTime() seems to only return seconds anyway

    void StartSecondsCounter()
    {

        time(&startTime);
    }
    int GetSecondsCounter()
    {
        time(&endTime);
        timeDiff = (difftime(endTime, startTime));
        return (int)timeDiff;
    }
} // namespace SecondsCounter

//=============================================================================
// Type conversion
//=============================================================================
namespace Conversion
{
    string NumToCString(int number)
    {
        stringstream ss;
        ss << number;
        return ss.str().c_str();
    }
    string NumToCString(double number)
    {
        stringstream ss;
        ss << number;
        return ss.str().c_str();
    }
    string NumToCString(float number)
    {
        stringstream ss;
        ss << number;
        return ss.str().c_str();
    }
} // namespace Conversion
