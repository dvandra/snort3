//--------------------------------------------------------------------------
// Copyright (C) 2014-2019 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// sfdaq.h author Michael Altizer <mialtize@cisco.com>

#ifndef SFDAQ_INSTANCE_H
#define SFDAQ_INSTANCE_H

#include <daq_common.h>

#include <string>

#include "main/snort_types.h"
#include "protocols/protocol_ids.h"

struct SFDAQConfig;

namespace snort
{
struct Packet;
struct SfIp;

class SFDAQInstance
{
public:
    SFDAQInstance(const char* intf, const SFDAQConfig*);
    ~SFDAQInstance();

    bool init(DAQ_Config_h, const std::string& bpf_string);

    bool start();
    bool was_started();
    bool stop();
    void reload();
    void abort();

    DAQ_RecvStatus receive_messages(unsigned max_recv);
    DAQ_Msg_h next_message()
    {
        if (curr_batch_idx < curr_batch_size)
            return daq_msgs[curr_batch_idx++];
        return nullptr;
    }
    int finalize_message(DAQ_Msg_h msg, DAQ_Verdict verdict);
    const char* get_error();

    int get_base_protocol();
    uint32_t get_batch_size() { return batch_size; }
    const char* get_input_spec();
    const DAQ_Stats_t* get_stats();

    bool can_inject();
    bool can_inject_raw();
    bool can_replace();
    bool can_start_unprivileged();
    SO_PUBLIC bool can_whitelist();

    int inject(DAQ_Msg_h, int rev, const uint8_t* buf, uint32_t len);
    bool interrupt();

    SO_PUBLIC int ioctl(DAQ_IoctlCmd cmd, void *arg, size_t arglen);
    SO_PUBLIC int modify_flow_opaque(DAQ_Msg_h, uint32_t opaque);
    int modify_flow_pkt_trace(DAQ_Msg_h, uint8_t verdict_reason,
        uint8_t* buff, uint32_t buff_len);
    int add_expected(const Packet* ctrlPkt, const SfIp* cliIP, uint16_t cliPort,
            const SfIp* srvIP, uint16_t srvPort, IpProtocol, unsigned timeout_ms,
            unsigned /* flags */);
    bool get_tunnel_bypass(uint8_t proto);

private:
    void get_tunnel_capabilities();

    std::string input_spec;
    DAQ_Instance_h instance = nullptr;
    DAQ_Msg_h* daq_msgs;
    unsigned curr_batch_size = 0;
    unsigned curr_batch_idx = 0;
    uint32_t batch_size;
    uint32_t pool_size = 0;
    uint32_t pool_available = 0;
    int dlt = -1;
    DAQ_Stats_t daq_stats = { };
    uint8_t daq_tunnel_mask = 0;
};
}
#endif

