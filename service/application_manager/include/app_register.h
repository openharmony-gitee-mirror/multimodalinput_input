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

#ifndef OHOS_APP_REGISTER_H
#define OHOS_APP_REGISTER_H
#include <set>
#include <vector>
#include "proto.h"
#include "uds_server.h"
#include "c_singleton.h"

namespace OHOS {
namespace MMI {
struct AppInfo {
    int32_t abilityId;
    int32_t windowId;
    int32_t fd;
    std::string bundlerName;
    std::string appName;
};
struct WaitQueueEvent {
    int32_t fd;
    int32_t event;
    uint64_t inputTime;
    uint64_t westonTime;
    uint64_t serverTime;
};

class AppRegister : public CSingleton<AppRegister> {
public:
    AppRegister();
    virtual ~AppRegister();

    bool Init(UDSServer& udsServer);

    const AppInfo& FindByWinId(int32_t windowId);

    const AppInfo& FindBySocketFd(int32_t fd);

    void RegisterAppInfoforServer(const AppInfo& appInfo);

    void UnregisterAppInfoBySocketFd(int32_t fd);

    void UnregisterConnectState(int32_t fd);

    void PrintfMap();
    void Dump(int32_t fd);
    void SurfacesDestroyed(const IdsList &desList);

    int32_t QueryMapSurfaceNum();

    bool IsMultimodeInputReady(MmiMessageId idMsg, const int32_t findFd, uint64_t inputTime, uint64_t westonTime = 0);

    WaitQueueEvent GetWaitQueueEvent(int32_t fd, int32_t idMsg);
    void DeleteEventFromWaitQueue(int32_t fd, int32_t idMsg);

    void RegisterConnectState(int32_t fd);

#ifdef OHOS_AUTO_TEST_FRAME
    void AutoTestSetAutoTestFd(int32_t fd);
    int32_t AutoTestGetAutoTestFd();
    void AutoTestGetAllAppInfo(std::vector<AutoTestClientListPkt>& clientListPkt);
#endif  // OHOS_AUTO_TEST_FRAME

private:
    bool OnAnrLocked(int32_t fd) const;
    bool CheckFindFdError(const int32_t findFd);
    bool CheckConnectionIsDead(const int32_t findFd);
    bool CheckWaitQueueBlock(ssize_t currentTime, ssize_t timeOut, const int32_t findFd);
    const AppInfo& FindAppInfoBySocketFd(int32_t fd);
    void UnregisterBySocketFd(int32_t fd);

    std::map<int32_t, AppInfo>::iterator EraseAppInfo(const std::map<int32_t, AppInfo>::iterator &it);
    std::map<int32_t, AppInfo>::iterator UnregisterAppInfo(int32_t winId);

private:
    IdsList fds_;
    std::map<int32_t, AppInfo> mapSurface_ = {}; // key=windowId:value=AppInfo
    std::vector<WaitQueueEvent> waitQueue_ = {};
    std::map<int32_t, int8_t> mapConnectState_ = {};
#ifdef OHOS_AUTO_TEST_FRAME
    int32_t autoTestFrameFd_ = 0;
#endif  // OHOS_AUTO_TEST_FRAME

    std::mutex mu_;
    UDSServer *udsServer_ = nullptr;
    const AppInfo AppInfoError_ = {-1, -1, -1, "", ""};
};
};
}
#define AppRegs OHOS::MMI::AppRegister::GetInstance()
#endif
