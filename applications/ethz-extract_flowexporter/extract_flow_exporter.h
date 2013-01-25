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
 * @file   extract_flow_exporter.h
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Februar, 2012
 * @brief  Identify the flow exports from a Netflow stream. 
 *
 * The class identifies the different Netflow exporters. 
 *
 */

#ifndef EXTRACT_FLOW_EXPORTER_H_
#define EXTRACT_FLOW_EXPORTER_H_

#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>

#include "packet.h"
#include "packet_source_csg_file.h"
#include "parser_netflow_5.h"
#include "parser_netflow_9.h"

/* Changelog ******************************************************************
V 00.00.01, 2012.02.29
	- first version by Dominik Schatzmann
*/
class ExtractFlowExporter {
  public:
  void process_files(
    std::string file_19991_dat,
    std::string file_19991_stat,
    std::string file_19993_dat,
    std::string file_19993_stat);
};

#endif // EXTRACT_FLOW_EXPORTER_H_
