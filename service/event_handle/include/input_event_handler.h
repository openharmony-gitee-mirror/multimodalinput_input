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
#ifndef OHOS_INPUT_EVENT_HANDLER_H
#define OHOS_INPUT_EVENT_HANDLER_H
#include "msg_handler.h"
#include "event_dispatch.h"
#include "event_package.h"
#include "c_singleton.h"

namespace OHOS {
namespace MMI {
using EventFun = std::function<int32_t(libinput_event& event)>;
using NotifyDeviceChange = std::function<void(int32_t, int32_t, char *)>;
class InputEventHandler : public MsgHandler<EventFun>, public CSingleton<InputEventHandler> {
public:
    InputEventHandler();
    virtual ~InputEventHandler() override;
    bool Init(UDSServer& udsServer);
    void OnEvent(void *event);
    void OnCheckEventReport();
    void RegistnotifyDeviceChange(NotifyDeviceChange cb);

protected:
    int32_t OnEventDeviceAdded(libinput_event& event);
    int32_t OnEventDeviceRemoved(libinput_event& event);
    int32_t OnEventKeyboard(libinput_event& event);
    int32_t OnEventPointer(libinput_event& event);
    int32_t OnEventTouch(libinput_event& event);
    int32_t OnEventGesture(libinput_event& event);
    int32_t OnEventTabletTool(libinput_event& event);
    int32_t OnEventTabletPad(libinput_event& event);
    int32_t OnEventSwitchToggle(libinput_event& event);
    int32_t OnEventJoyStickKey(libinput_event& event, const uint64_t time);
    int32_t OnEventTabletPadKey(libinput_event& event);
    int32_t OnEventJoyStickAxis(libinput_event& event, const uint64_t time);
    bool SendMsg(const int32_t fd, NetPacket& pkt) const;
#ifndef OHOS_AUTO_TEST_FRAME
    bool OnSystemEvent(const KeyEventValueTransformations& temp, const enum KEY_STATE state) const;
#else
    bool OnSystemEvent(const KeyEventValueTransformations& temp, const enum KEY_STATE state,
        AutoTestKeyTypePkt& autoTestKeyTypePkt);
#endif  // OHOS_AUTO_TEST_FRAME

private:
    int32_t OnEventHandler(libinput_event& ev);
    std::mutex mu_;
    UDSServer *udsServer_ = nullptr;
    WindowSwitch winSwitch_;
    EventDispatch eventDispatch_;
    EventPackage eventPackage_;
    KeyEventValueTransformation xkbKeyboardHandlerKey_;
    NotifyDeviceChange notifyDeviceChange_;

    uint64_t idSeed_ = 0;
    int32_t eventType_ = 0;
    uint64_t initSysClock_ = 0;
    uint64_t lastSysClock_ = 0;
};
}
}
#define InputHandler OHOS::MMI::InputEventHandler::GetInstance()
#endif