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

#ifndef OHOS_TIME_COST_CHK_H
#define OHOS_TIME_COST_CHK_H
#include <inttypes.h>

#define MAX_INPUT_EVENT_TIME (1000)
#define MAX_OVER_TIME (300)

namespace OHOS::MMI {
template<class T>
class TimeCostChk {
    static inline constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TimeCostChk" };
public:
    TimeCostChk(const std::string& strReason, const std::string& strOutputStr, uint32_t tmChk, T llParam1,
                int64_t llParam2 = 0)
        : beginTime_(std::chrono::high_resolution_clock::now()),
          strOutput_(strOutputStr),
          strReason_(strReason),
          uiTime_(tmChk),
          llParam1_(static_cast<int64_t>(llParam1)),
          llParam2_(llParam2) {
    }

    ~TimeCostChk(void)
    {
        auto ullCost = GetElapsed_micro(); // microsecond
        if ((ullCost > uiTime_) && strReason_.size() > 0 && strOutput_.size() > 0) {
            if (llParam1_ != 0 || llParam2_ != 0) {
                MMI_LOGW("Time cost overtime (%{public}" PRId64 "(us)>%{public}u(us)) when Reason:%{public}s chk:%{public}s "
                         "param1:%{public}" PRId64 " param2:%{public}" PRId64 "",
                         ullCost, uiTime_, strReason_.c_str(), strOutput_.c_str(), llParam1_, llParam2_);
            } else {
                MMI_LOGW("TimeCostChk Overtime(%{public}" PRId64 "(us)>%{public}u(us)) when Reason:%{public}s chk:%{public}s",
                         ullCost, uiTime_, strReason_.c_str(), strOutput_.c_str());
            }
        }
    }

    // microseconds
    uint64_t GetElapsed_micro() const
    {
        auto tm64Cost = std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::high_resolution_clock::now() - beginTime_
                        ).count();
        return static_cast<uint64_t>(tm64Cost);
    }

private:
    const std::chrono::time_point<std::chrono::high_resolution_clock> beginTime_;
    const std::string strOutput_ = ""; // output string
    const std::string strReason_ = ""; // reason string
    const uint32_t uiTime_ = 0;
    const int64_t llParam1_ = 0;
    const int64_t llParam2_ = 0;
};
}
#endif
