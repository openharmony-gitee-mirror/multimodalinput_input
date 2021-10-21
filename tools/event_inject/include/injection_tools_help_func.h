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
#ifndef OHOS_INJECTION_TOOLS_HELP_FUNC_H
#define OHOS_INJECTION_TOOLS_HELP_FUNC_H

#include <string.h>
#include "util.h"

namespace OHOS {
    namespace MMI {
        class InjectionToolsHelpFunc {
        public:
            std::string GetHelpText();
        };
    }
}
#endif // OHOS_INJECTION_TOOLS_HELP_FUNC_H