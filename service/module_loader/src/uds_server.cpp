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
#include "uds_server.h"
#include <sys/socket.h>
#include <list>
#include <inttypes.h>
#include "safe_keeper.h"
#include "util.h"
#include "util_ex.h"
#include "log.h"
#include "i_multimodal_input_connect.h"
#include "uds_command_queue.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "UDSServer"};
    }
}

OHOS::MMI::UDSServer::UDSServer()
{
}

OHOS::MMI::UDSServer::~UDSServer()
{
    MMI_LOGD("enter");
    Stop();
    MMI_LOGD("leave");
}

void OHOS::MMI::UDSServer::Stop()
{
    std::lock_guard<std::mutex> lock(mux_);
    isRun_ = false;
    if (epollFd_ != -1) {
        close(epollFd_);
        epollFd_ = -1;
    }

    for (auto it : sessionsMap_) {
        it.second->Close();
    }
    sessionsMap_.clear();
    if (t_.joinable()) {
        t_.join();
    }
}

bool OHOS::MMI::UDSServer::SendMsg(int32_t fd, NetPacket& pkt)
{
    std::lock_guard<std::mutex> lock(mux_);
    CHKF(fd >= 0, PARAM_INPUT_INVALID);
    auto ses = GetSession(fd);
    if (!ses) {
        MMI_LOGE("SendMsg fd:%{public}d not found, The message was discarded! errCode:%{public}d",
                 fd, SESSION_NOT_FOUND);
        return false;
    }
    return ses->SendMsg(pkt);
}

void OHOS::MMI::UDSServer::Broadcast(NetPacket& pkt)
{
    std::lock_guard<std::mutex> lock(mux_);
    for (auto it : sessionsMap_) {
        it.second->SendMsg(pkt);
    }
}

void OHOS::MMI::UDSServer::Multicast(const IdsList& fdList, NetPacket& pkt)
{
    for (auto it : fdList) {
        SendMsg(it, pkt);
    }
}

bool  OHOS::MMI::UDSServer::ClearDeadSessionInMap(const int serverFd, const int clientFd)
{
    auto it = sessionsMap_.find(serverFd);
    if (it != sessionsMap_.end()) {
        MMI_LOGE("The session(fd1: %{public}d) on the server side will be closed because it had in map.", serverFd);
        DelSession(serverFd);
    }

    it = sessionsMap_.find(clientFd);
    if (it != sessionsMap_.end()) {
        MMI_LOGE("The session(fd2:%{public}d) on the server side will be closed because it had in map.", clientFd);
        DelSession(clientFd);
    }

    return true;
}

int32_t OHOS::MMI::UDSServer::AddSocketPairInfo(const std::string& programName, const int moduleType, int& serverFd,
                                                int& toReturnClientFd)
{
    std::lock_guard<std::mutex> lock(mux_);
    MMI_LOGT("enter.");
    const int NUMBER_TWO = 2;
    int sockFds[NUMBER_TWO] = {};

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockFds) != 0) {
        const int savedErrNo = errno;
        MMI_LOGT("call socketpair fail, errno: %{public}d, msg: %{public}s!", savedErrNo, strerror(savedErrNo));
        return RET_ERR;
    }

    serverFd = sockFds[0];
    toReturnClientFd = sockFds[1]; // fcntl(sockFds[1], F_DUPFD_CLOEXEC, 0);
    if (toReturnClientFd < 0) {
        const int savedErrNo = errno;
        MMI_LOGT("call fcntl fail, errno: %{public}d, msg: %{public}s!", savedErrNo, strerror(savedErrNo));
        return RET_ERR;
    }

    const size_t bufferSize = 32 * 1024;

    setsockopt(sockFds[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockFds[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockFds[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockFds[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

    MMI_LOGE("alloc socketpair, serverFd = %{public}d, clientFd = %{public}d(%{public}d).",
             serverFd, toReturnClientFd, sockFds[1]);
    auto closeSocketFdWhenError = [&serverFd, &toReturnClientFd] {
        close(serverFd);
        close(toReturnClientFd);
        serverFd = IMultimodalInputConnect::INVALID_SOCKET_FD;
        toReturnClientFd = IMultimodalInputConnect::INVALID_SOCKET_FD;
    };

    std::list<std::function<void()> > cleanTaskList;
    auto cleanTaskWhenError = [cleanTaskList] {
        for (auto i : cleanTaskList) {
            i();
        }
    };

    cleanTaskList.push_back(closeSocketFdWhenError);

    if (!ClearDeadSessionInMap(serverFd, toReturnClientFd)) {
        cleanTaskWhenError();
        MMI_LOGE("IsSocketFdNotUsed error!");
        return RET_ERR;
    }

    epoll_event nev = {};
    nev.events = EPOLLIN;
    nev.data.fd = serverFd;
    int32_t ret = EpollCtl(serverFd, EPOLL_CTL_ADD, nev);
    if (ret != RET_OK) {
        cleanTaskWhenError();
        MMI_LOGE("epoll_ctl EPOLL_CTL_ADD return %{public}d errCode:%{public}d", ret, EPOLL_MODIFY_FAIL);
        return ret;
    }

    SessionPtr sess = std::make_shared<UDSSession>(programName, moduleType, serverFd);
    if (sess == nullptr) {
        cleanTaskWhenError();
        MMI_LOGE("insufficient memory.");
        return RET_ERR;
    }
#ifdef OHOS_BUILD_MMI_DEBUG
    sess->SetClientFd(toReturnClientFd);
#endif // OHOS__BUILD_MMI_DEBUG

    OnConnected(sess);
    if (!AddSession(sess)) {
        cleanTaskWhenError();
        MMI_LOGE("AddSession fail.");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t OHOS::MMI::UDSServer::EpollCreat(int32_t size)
{
    epollFd_ = epoll_create(size);
    if (epollFd_ == -1) {
        MMI_LOGE("UDSSocket::EpollCreat epoll_create retrun %{public}d", epollFd_);
    }
    return epollFd_;
}

int32_t OHOS::MMI::UDSServer::EpollCtl(const int fd, const int32_t op, epoll_event &event) const
{
    CHKR(epollFd_ >= 0, PARAM_INPUT_INVALID, RET_ERR);
    CHKR(fd >= 0, PARAM_INPUT_INVALID, RET_ERR);
    auto ret = epoll_ctl(epollFd_, op, fd, &event);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::EpollCtl epoll_ctl retrun %{public}d op:%{public}d fd:%{public}d", ret, op, fd);
    }
    return ret;
}

int32_t OHOS::MMI::UDSServer::EpollWait(epoll_event &events, const int32_t maxevents, const int32_t timeout) const
{
    CHKR(epollFd_ >= 0, PARAM_INPUT_INVALID, RET_ERR);
    auto ret = epoll_wait(epollFd_, &events, maxevents, timeout);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::EpollWait epoll_wait retrun %{public}d", ret);
    }
    return ret;
}

void OHOS::MMI::UDSServer::Dump(int32_t fd)
{
    std::lock_guard<std::mutex> lock(mux_);
    mprintf(fd, "Sessions: count=%d", sessionsMap_.size());
    std::string strTmp;
    if (sessionsMap_.size() > 0) {
        strTmp = "fds:[";
    }
    for (auto& it : sessionsMap_) {
        strTmp += std::to_string(it.second->GetFd()) + ",";
    }
    if (sessionsMap_.size() > 0) {
        strTmp.resize(strTmp.size() - 1);
        strTmp += "]";
    }
    mprintf(fd, "\t%s", strTmp.c_str());
}

void OHOS::MMI::UDSServer::OnConnected(SessionPtr s)
{
    MMI_LOGI("UDSServer::OnConnected session desc:%{public}s", s->GetDescript().c_str());
}

void OHOS::MMI::UDSServer::OnDisconnected(SessionPtr s)
{
    MMI_LOGI("UDSServer::OnDisconnected session desc:%{public}s", s->GetDescript().c_str());
}

void OHOS::MMI::UDSServer::SetRecvFun(MsgServerFunCallback fun)
{
    recvFun_ = fun;
}

bool OHOS::MMI::UDSServer::StartServer()
{
    isRun_ = true;
    t_ = std::thread(std::bind(&UDSServer::OnThread, this));
    t_.detach();
    return true;
}

void OHOS::MMI::UDSServer::OnRecv(int32_t fd, const char *buf, size_t size)
{
    CHK(fd >= 0 && buf, PARAM_INPUT_INVALID);
    auto sess = GetSession(fd);
    CHK(sess, NULL_POINTER);
    int32_t readIdx = 0;
    int32_t packSize = 0;
    const auto headSize = static_cast<int32_t>(sizeof(PackHead));
    CHK(size >= headSize, VAL_NOT_EXP);
    while (size > 0 && recvFun_) {
        CHK(size >= headSize, VAL_NOT_EXP);
        auto head = (PackHead*)&buf[readIdx];
        CHK(head->size[0] < static_cast<int32_t>(size), VAL_NOT_EXP);
        packSize = headSize + head->size[0];

        NetPacket pkt(head->idMsg);
        if (head->size[0] > 0) {
            CHK(pkt.Write(&buf[readIdx + headSize], head->size[0]), STREAM_BUF_WRITE_FAIL);
        }
        recvFun_(sess, pkt);
        size -= packSize;
        readIdx += packSize;
    }
}

void OHOS::MMI::UDSServer::OnEvent(const epoll_event& ev, CLMAP<int32_t, StreamBufData>& bufMap)
{
    const int32_t maxCount = static_cast<int32_t>(MAX_STREAM_BUF_SIZE / MAX_PACKET_BUF_SIZE) + 1;
    CHK(maxCount > 0, VAL_NOT_EXP);
    char szBuf[MAX_PACKET_BUF_SIZE] = {};
    auto fd = ev.data.fd;
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        MMI_LOGI("UDSServer::OnEvent fd:%{public}d, ev.events = 0x%{public}x", fd, ev.events);
        auto secPtr = GetSession(fd);
        if (secPtr) {
            OnDisconnected(secPtr);
            DelSession(fd);
        }
        close(fd);
        return;
    }

    if (fd != IMultimodalInputConnect::INVALID_SOCKET_FD && (ev.events & EPOLLIN)) {
        auto bufData = &bufMap[fd];
        if (bufData->isOverflow) {
            return;
        }
        for (auto j = 0; j < maxCount; j++) {
            auto size = read(fd, (void*)szBuf, MAX_PACKET_BUF_SIZE);
#ifdef OHOS_BUILD_HAVE_DUMP_DATA
            DumpData(szBuf, size, LINEINFO, "in %s, read message from fd: %d.", __func__, fd);
#endif
            if (size > 0 && bufData->sBuf.Write(szBuf, size) == false) {
                bufData->isOverflow = true;
                break;
            }
            if (size < MAX_PACKET_BUF_SIZE) {
                break;
            }
        }
    }
}

void OHOS::MMI::UDSServer::DumpSession(const std::string &title)
{
    MMI_LOGI("in %s: %s\n", __func__, title.c_str());
    int i = 0;
    for (auto& r : sessionsMap_) {
        MMI_LOGI("%d, %s\n", i, r.second->GetDescript().c_str());
        i++;
    }
}

bool OHOS::MMI::UDSServer::AddSession(SessionPtr ses)
{
    CHKF(ses, OHOS::NULL_POINTER);
    auto fd = ses->GetFd();
    CHKF(fd >= 0, VAL_NOT_EXP);
    sessionsMap_[fd] = ses;
    DumpSession("AddSession");
    if (sessionsMap_.size() > MAX_SESSON_ALARM) {
        MMI_LOGW("Too many clients... Warning Value:%{public}d Current Value:%{public}zd",
                 MAX_SESSON_ALARM, sessionsMap_.size());
    }
    return true;
}

void OHOS::MMI::UDSServer::DelSession(int32_t fd)
{
    sessionsMap_.erase(fd);
    DumpSession("DelSession");
}

void OHOS::MMI::UDSServer::OnThread()
{
    OHOS::MMI::SetThreadName(std::string("uds_server"));
    uint64_t tid = GetThisThreadIdOfLL();
    CHK(tid > 0, VAL_NOT_EXP);
    MMI_LOGI("begin... tid:%{public}" PRId64 "", tid);
    SafeKpr->RegisterEvent(tid, "UDSServer::_OnThread");

    CLMAP<int32_t, StreamBufData> bufMap;
    epoll_event ev[MAX_EVENT_SIZE] = {};
    while (isRun_) {
        auto count = EpollWait(*ev, MAX_EVENT_SIZE, DEFINE_EPOLL_TIMEOUT);
        if (count > 0) {
            bufMap.clear();
            for (auto i = 0; i < count; i++) {
                OnEvent(ev[i], bufMap);
            }
            for (auto& it : bufMap) {
                if (it.second.isOverflow) {
                    continue;
                }
                OnRecv(it.first, it.second.sBuf.Data(), it.second.sBuf.Size());
            }
        }
#ifdef OHOS_BUILD_MMI_DEBUG
        // HandleCommandQueue();
#endif // OHOS_BUILD_MMI_DEBUG
        SafeKpr->ReportHealthStatus(tid);
    }
    MMI_LOGI("end");
}

#ifdef OHOS_BUILD_MMI_DEBUG
void OHOS::MMI::UDSServer::HandleCommandQueue()
{
    UdsCommandQueue &commandQueue = Singleton<UdsCommandQueue>::GetInstance();
    if (commandQueue.GetSize() == 0) {
        return;
    }
    const int maxProcessTenCommandPerOneTime = 10;
    for (int i = 0; i < maxProcessTenCommandPerOneTime; ++i) {
        std::string command = commandQueue.PopCommand();
        if (command.empty()) {
            continue;
        }
        if (command == "dump session") {
            DumpSession("** dump sesion**\n");
        }
    }
}
#endif // OHOS_BUILD_MMI_DEBUG

