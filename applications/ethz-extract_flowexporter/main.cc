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
 * @file   main.cc
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Februar, 2012
 * @brief  Identify the flow exports from a Netflow stream. 
 *
 * This program identifies the different Netflow exporters. 
 *
 */
#include "extract_flow_exporter.h"
int main(int argc, char *argv[]) {

  ExtractFlowExporter efe;

  if (argc != 5) {
    std::cerr << "usage ./main ./../19991_XX.dat.bz2 ./../19991_XX.stat.bz2 ./../19993_XX.dat.bz2 ./../19993_XX.stat.bz2" <<  std::endl;
    exit(-1);
  }

  std::cout << "We will analyze the following files for you:" << std::endl;
  std::cout << argv[1] << std::endl;
  std::cout << argv[2] << std::endl;
  std::cout << argv[3] << std::endl;
  std::cout << argv[4] << std::endl;

  efe.process_files(argv[1],argv[2],argv[3],argv[4]);

  return(0);
};
