// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2017-2018 The Puppycoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_INIT_H
#define BITCOIN_INIT_H

#include <string>

class CScheduler;
class CWallet;
class CzPUPWallet;

namespace boost
{
class thread_group;
} // namespace boost

extern CWallet* pwalletMain;
extern CzPUPWallet* zwalletMain;

void StartShutdown();
bool ShutdownRequested();
/** Interrupt threads */
void Interrupt();
void Shutdown();
void PrepareShutdown();
bool AppInit2();

/** The help message mode determines what help message to show */
enum HelpMessageMode {
    HMM_BITCOIND,
    HMM_BITCOIN_QT
};

/** Help for options shared between UI and daemon (for -help) */
std::string HelpMessage(HelpMessageMode mode);
/** Returns licensing information (for -version) */
std::string LicenseInfo();

#endif // BITCOIN_INIT_H
