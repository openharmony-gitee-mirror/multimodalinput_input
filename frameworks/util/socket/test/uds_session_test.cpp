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

#include <gtest/gtest.h>
#include "proto.h"
#include "uds_session.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class UDSSessionTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    int fd_ = -1;
    static constexpr char programName_[] = "uds_sesion_test";
    const int moduleType_ = 3; // 3 CONNECT_MODULE_TYPE_ST_TEST

    void SetUp() override;
    void TearDown()  override;
};

void UDSSessionTest::SetUp()
{
    fd_ = STDOUT_FILENO;
}

void UDSSessionTest::TearDown()
{
    // do nothing
}

HWTEST_F(UDSSessionTest, Construct, TestSize.Level1)
{
    UDSSession udsSession(programName_, moduleType_, fd_);
    udsSession.Close();
}

HWTEST_F(UDSSessionTest, SendMsg_type1_001, TestSize.Level1)
{
    const char *buf = "1234";
    size_t size = 4;
    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_002, TestSize.Level1)
{
    const char *buf = nullptr;
    size_t size = 4;

    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_003, TestSize.Level1)
{
    const char *buf = nullptr;
    size_t size = 0;
    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_004, TestSize.Level1)
{
    const char *buf = "this unit data";
    size_t size = 14;

    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_005, TestSize.Level1)
{
    const char *buf = "this unit data";
    size_t size = -1001;

    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_001, TestSize.Level1)
{
    int32_t fd = -1;
    NetPacket newPacket(MmiMessageId::INVALID);

    UDSSession sesObj(programName_, moduleType_, fd);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_002, TestSize.Level1)
{
    NetPacket newPacket(MmiMessageId::BEGIN);

    UDSSession sesObj(programName_, moduleType_, fd_);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_003, TestSize.Level1)
{
    int32_t fd = -65535;
    NetPacket newPacket(MmiMessageId::BEGIN);

    UDSSession sesObj(programName_, moduleType_, fd);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}
} // namespace
