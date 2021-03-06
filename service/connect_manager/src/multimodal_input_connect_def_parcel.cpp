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

#include "multimodal_input_connect_def_parcel.h"

namespace OHOS {
namespace MMI {
bool ConnectDefReqParcel::Marshalling(Parcel& out) const
{
    if (!out.WriteInt32(data.moduleId)) {
        return false;
    }
    if (!out.WriteString(data.clientName)) {
        return false;
    }
    return true;
}

ConnectDefReqParcel *ConnectDefReqParcel::Unmarshalling(Parcel& in)
{
    auto* request = new (std::nothrow) ConnectDefReqParcel();
    if (request == nullptr) {
        return nullptr;
    }

    if (!in.ReadInt32(request->data.moduleId)) {
        delete request;
        request = nullptr;
        return nullptr;
    }

    request->data.clientName = in.ReadString();
    return request;
}

bool ConnectDefRespParcel::Marshalling(Parcel &out) const
{
    if (!out.WriteInt32(data.returnCode)) {
        return false;
    }
    if (!out.WriteInt32(data.allocedSocketId)) {
        return false;
    }
    return true;
}

ConnectDefRespParcel *ConnectDefRespParcel::Unmarshalling(Parcel &in)
{
    auto *response = new (std::nothrow) ConnectDefRespParcel();
    if (response == nullptr) {
        return nullptr;
    }

    if (!in.ReadInt32(response->data.returnCode)) {
        delete response;
        response = nullptr;
        return nullptr;
    }

    if (!in.ReadInt32(response->data.allocedSocketId)) {
        delete response;
        response = nullptr;
        return nullptr;
    }

    return response;
}
} // namespace MMI
} // namespace OHOS
