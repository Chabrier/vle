/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Filesystem.hpp>
#include <thread>
#include <mutex>

namespace vle { namespace utils {

struct DownloadManager::Pimpl
{
    ContextPtr mContext;
    std::mutex mMutex;
    std::thread mThread;
    std::string mCommand;
    std::string mUrl;
    std::string mFilename;
    std::string mErrorMessage;
    bool mIsStarted;
    bool mIsFinish;
    bool mHasError;

    Pimpl(ContextPtr context)
        : mContext(context)
        , mIsStarted(false)
        , mIsFinish(false)
        , mHasError(false)
    {
        utils::Preferences prefs(true, mContext->getConfigurationFile());
        if (not prefs.get("vle.command.url.get", &mCommand))
            throw InternalError(
                _("Download: fail to get vle.command.url.get command "
                  "from vle.conf"));
    }

    ~Pimpl()
    {
        join();
    }

    void start(const std::string& url, const std::string& filepath)
    {
        if (mIsStarted) {
            mHasError = true;
            mErrorMessage.assign("Download manager: already started");
            return;
        }

        mUrl = url;
        mFilename = filepath;
        mErrorMessage.clear();
        mIsStarted = true;
        mIsFinish = false;
        mHasError = false;

        mThread = std::thread(&DownloadManager::Pimpl::run, this);
    }

    void join()
    {
        if (mIsStarted and not mIsFinish) {
            mThread.join();
            mIsFinish = true;
        }
    }

    void run()
    {
        std::string command;
        try {
            command = (vle::fmt(mCommand) % mUrl % mFilename).str();

            utils::Spawn spawn(mContext);
            std::vector<std::string> argv = spawn.splitCommandLine(command);
            std::string exe = std::move(argv.front());
            argv.erase(argv.begin());

            FSpath pwd = FSpath::current_path();
            if (not spawn.start(exe, pwd.string(), argv, 50000u)) {
                mErrorMessage = _("Download: fail to start download command");
                TraceAlways(mErrorMessage.c_str());
                mHasError = true;
                return;
            }

            std::string output, error;
            while (not spawn.isfinish()) {
                if (spawn.get(&output, &error)) {
                    TraceAlways(output.c_str());
                    TraceAlways(output.c_str());
                    output.clear();
                    error.clear();

                    std::this_thread::sleep_for(std::chrono::microseconds(200));
                } else
                    break;
            }

            spawn.wait();

            std::string message;
            bool success;
            spawn.status(&message, &success);

            if (not message.empty())
                TraceAlways(message.c_str());
        } catch (const std::exception& e) {
            mErrorMessage = (fmt(_("Download: unable to download '%s' "
                                   "in '%s' with the '%s' command"))
                             % mUrl % mFilename % command).str();
            TraceAlways(mErrorMessage.c_str());
            mHasError = true;
        }
    }
};

DownloadManager::DownloadManager(ContextPtr context)
    : mPimpl(std::make_unique<DownloadManager::Pimpl>(context))
{
}

DownloadManager::~DownloadManager() noexcept = default;

void DownloadManager::start(const std::string& url, const std::string& filepath)
{
    mPimpl->start(url, filepath);
}

void DownloadManager::join()
{
    mPimpl->join();
}

std::string DownloadManager::filename() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        std::lock_guard<std::mutex> lock(mPimpl->mMutex);
        return mPimpl->mFilename;
    }
    return mPimpl->mFilename;
}

bool DownloadManager::isFinish() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        std::lock_guard<std::mutex> lock(mPimpl->mMutex);
        return mPimpl->mIsFinish;
    }
    return mPimpl->mIsFinish;
}

bool DownloadManager::hasError() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        std::lock_guard<std::mutex> lock(mPimpl->mMutex);
        return mPimpl->mHasError;
    }
    return mPimpl->mHasError;
}

std::string DownloadManager::getErrorMessage() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        std::lock_guard<std::mutex> lock(mPimpl->mMutex);
        return mPimpl->mErrorMessage;
    }
    return mPimpl->mErrorMessage;
}

}} // namespace vle utils
