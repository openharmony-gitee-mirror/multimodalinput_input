/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_UTIL_H
#define OHOS_UTIL_H

#include <map>
#include <vector>
#include <string>
#include <time.h>
#include "struct_multimodal.h"
#include "define_multimodal.h"

#ifndef IdsList
#define IdsList std::vector<int32_t>
#endif

namespace OHOS::MMI {
    const char *GetMmiErrorTypeDesc(int32_t errorCodeEnum);
    std::string GetEnv(const std::string& name);
    std::string UuIdGenerate();
    int64_t GetMicrotime();
    uint64_t GetSysClockTime();
    int64_t GetMillisTime();
    std::string GetUUid();
    std::string GetThisThreadIdOfString();
    uint64_t GetThisThreadIdOfLL();
    size_t StringToken(std::string& str, const std::string& sep, std::string& token);
    size_t StringSplit(const std::string& str, const std::string& sep, std::vector<std::string>& vecList);
    std::string IdsListToString(const std::vector<int32_t>& list, const std::string& sep);
    void LocalTime(struct tm& t, time_t curTime = 0);
    std::string Strftime(const std::string& format = "%F %T", time_t curTime = 0);
    void PrintEventJoyStickAxisInfo(const EventJoyStickAxis& r, const int32_t fd,
        const int32_t abilityId, const int32_t focusId, const uint64_t preHandlerTime);
    void PrintWMSInfo(const std::string& str, const int32_t fd, const int32_t abilityId, const int32_t focusId);
    int GetPid();
    std::string GetFileName(const std::string& strPath);
    const char* GetProgramName();
    char* mmi_basename(char* path);
    std::string GetStackInfo();
    void SetThreadName(const std::string& name);
    const std::string& GetThreadName();
    void AddId(IdsList &list, int32_t id);
    size_t CalculateDifference(const IdsList &list1, IdsList &list2, IdsList &difList);
}

#endif
