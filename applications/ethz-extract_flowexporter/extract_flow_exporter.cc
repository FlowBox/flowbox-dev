// Copyright (C) 2011-13 Dominik Schatzmann <schadomi@gmail.com>
// This file is part of FlowBox. FlowBox is free software: you can redistribute
// it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3
// of the License, or (at your option) any later version.
//
// FlowBox is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with FlowBox. If not, see <http://www.gnu.org/licenses/>.

/**
 * @file   extract_flow_exporter.cc
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Februar, 2012
 * @brief  Identify the flow exports from a Netflow stream. 
 *
 * The class identifies the different Netflow exporters. 
 *
 */
#include "extract_flow_exporter.h"

void to_console(std::pair<const std::string, uint64_t>& pair) {
  std::cout << pair.first << ", " << pair.second << std::endl;
}
void ExtractFlowExporter::process_files(
    std::string file_19991_dat,
    std::string file_19991_stat,
    std::string file_19993_dat,
    std::string file_19993_stat
){
  PacketSourceCSGFile data_source;

  // 1. open the stream
  // 2. process the packets
  // -- return stats/obs when requested
  int err;
  Packet* packet_p;
  char tmp_ip[INET6_ADDRSTRLEN];
  uint32_t version = 0;
  uint32_t engine_id = 0;
  std::map<std::string, uint64_t> exporters;
  int packets_counter = 0;

  // 1. open the stream
  std::cout << "1. open the stream" << std::endl;
  std::cout.flush();
  err = data_source.set_file_set(
    file_19991_dat,
    file_19991_stat,
    file_19993_dat,
    file_19993_stat
  );
  if(err != PacketSourceCSGFile::kFileSetOK) {
    throw FlowBoxE("FileSet not OK",__FILE__, __LINE__);
  }
  
  std::cout << "2. process the packets" << std::endl;
  std::cout.flush();
  // 2. process the packets
  while(true) {

    // get a packet
    err = data_source.get(&packet_p);
    packets_counter++;

    if(err == PacketSourceCSGFile::kFileSetOK) {

      std::stringstream tmp;

      // packet.addr_
      version = ParserNetflow9::get_version(*packet_p);
      if(version == 5) {
        engine_id = ParserNetflow5::get_engine_id(*packet_p);
      } else if (version == 9) {
        engine_id = ParserNetflow9::get_engine_id(*packet_p);
      } else  {
        throw FlowBoxE("Unknown Version Netflow Version",__FILE__, __LINE__);
      }

      //std::cerr << "Version " << version << std::endl;

      if(packet_p->addr_.sa_family == AF_INET) {
        inet_ntop(AF_INET, &((struct sockaddr_in*) (&packet_p->addr_))->sin_addr, tmp_ip, INET6_ADDRSTRLEN);
        tmp << tmp_ip << ", ";
      } else if(packet_p->addr_.sa_family == AF_INET6) {
        inet_ntop(AF_INET6, &((struct sockaddr_in6*) (&packet_p->addr_))->sin6_addr, tmp_ip, INET6_ADDRSTRLEN);
        tmp << tmp_ip << ", ";
      } else {
        throw FlowBoxE("IP: unknown addr family",__FILE__, __LINE__);
      }
      tmp << engine_id << ", ";
      tmp << version;

      std::string exporter_s = tmp.str();
      exporters[exporter_s]++;

    } else if (err == PacketSourceCSGFile::kFileSetRetry) {
      std::cout << "kFileSetRetry" << std::endl;
      continue;
    } else if (err == PacketSourceCSGFile::kFileSetEOF) {
      std::cout << "kFileSetEOF" << std::endl;
      break;
    } else {
      throw FlowBoxE("Generic Error",__FILE__, __LINE__);
    };

    //if(packets_counter > 100000)
    //  break;

  }; // while:  PROCESS FILE SET

  std::cout << "We analyzed " << packets_counter << " packets" << std::endl;
  std::cout << " and observed " << exporters.size() << " exporters" << std::endl;
  std::cout << "IP, Engine ID, NetFlow Version, Packet Count " << std::endl;
  std::for_each(exporters.begin(), exporters.end(), to_console);
};

