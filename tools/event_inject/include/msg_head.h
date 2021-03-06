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

#ifndef OHOS_MSG_HEAD_H
#define OHOS_MSG_HEAD_H

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <linux/input.h>
#include <linux/uinput.h>
#include "nlohmann/json.hpp"
#include "libmmi_util.h"

using Json = nlohmann::json;

/*
 * Structure for docking libinput
 */
struct InputEvent {
    uint16_t code;
    int32_t target;
    int32_t type;
    int32_t value;
    int32_t x;
    int32_t y;
    int32_t fp;
    int32_t devType;
    int32_t track;
    int32_t blockTime;
    int32_t multiReprot;
};

/*
 * Click events on devices
 * such as mouse and keyboard
 */
enum PressEvent {
    EVENT_RELEASE = 0,
    EVENT_PRESS = 1,
    LONG_PRESS = 2,
    TOUCH_PAD_PRESS = 15
};

enum HdiInfoType :uint16_t {
    GET_STATUS_INFO = 1001,
    SET_HOT_PLUGS = 1002,
    SET_EVENT_INJECT = 1003,
    GET_DEVICE_INFO = 1004,
    SHOW_DEVICE_INFO = 1005,
    REPLY_STATUS_INFO = 2001,
};

struct DeviceInformation {
    bool status;
    int32_t devIndex;
    int32_t devType;
    int16_t fd;
    char chipName[32];
};

struct InjectEvent {
    input_event event;
    int32_t blockTime;
};

struct InputEventArray {
    std::string deviceName;
    std::string target;
    std::vector<InjectEvent> events;
};

typedef std::function<void(void)> HandleInjectCommandItemsFuncType;
typedef std::function<int32_t (const InputEvent& inputEvent)> WriteDeviceFun;
#endif