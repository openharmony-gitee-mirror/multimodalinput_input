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
#ifndef OHOS_DEFINE_MULTIMODAL_H
#define OHOS_DEFINE_MULTIMODAL_H

#include "log.h"

#ifndef RET_OK
    #define RET_OK (0)
#endif

#ifndef RET_ERR
    #define RET_ERR (-1)
#endif

#ifndef LINEINFO
#define LINEINFO __FILE__, __LINE__
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
    #define WL_EXPORT __attribute__ ((visibility("default")))
#else
    #define WL_EXPORT
#endif

#define USE_CLMAP
#define USE_CLSET

#ifdef USE_CLMAP
    #define MAKEPAIR    std::make_pair
    #define PAIR        std::pair
    #define CLMAP       std::map
    #define CLMULTIMAP  std::multimap
#else
    #define CLMAP       std::unordered_map
    #define CLMULTIMAP  std::unordered_multimap
#endif

#ifdef USE_CLSET
    #define CLSET       std::set
    #define CLMULTISET  std::multiset
#else
    #define CLSET       std::unordered_set
    #define CLMULTISET  std::unordered_multiset
#endif


#ifndef IdsList
#define IdsList std::vector<int32_t>
#endif

#ifndef StringList
#define StringList  std::vector<std::string>
#endif

#ifndef StringSet
#define StringSet   CLSET<std::string>
#endif

#ifndef StringMap
#define StringMap   CLMAP<std::string, std::string>
#endif

#ifdef DEBUG_CODE_TEST
#define CK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CK(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
    }

#define CHK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHK(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return; \
    }

#define CHKF(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKF(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return 0; \
    }

#define CHKC(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKC(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        continue; \
    }

#define CHKR(msg, ec, r) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKR(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return r; \
    }
#else
#define CK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CK(%{public}s) errCode:%{public}d", #msg, ec); \
    }

#define CHK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHK(%{public}s) errCode:%{public}d", #msg, ec); \
        return; \
    }

#define CHKF(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHKF(%{public}s) errCode:%{public}d", #msg, ec); \
        return 0; \
    }

#define CHKC(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHKC(%{public}s) errCode:%{public}d", #msg, ec); \
        continue; \
    }

#define CHKR(msg, ec, r) \
    if (!(msg)) { \
        MMI_LOGE("CHKR(%{public}s) errCode:%{public}d", #msg, ec); \
        return r; \
    }
#endif


#endif
