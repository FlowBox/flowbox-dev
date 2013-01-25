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
 * @file   main_csg_reader.cc
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Januar, 2013
 * @brief  A program to print CSG flow files to SDTOUT. 
 *
 * This programm uses the csg_file_reader to read the flow files from 
 * disk and prints the information as text to STDOUT.
 *
 */

#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>

// FlowBox 
#include <parser_unit_csg_file_netflow.h>

// Own Stuff
#include "flow_to_txt.h"

bool run;

void* parser_data_thread(void *ptr);
void* parser_obs_thread(void *ptr);
void* parser_stat_thread(void *ptr);
void* flow_to_txt_data_thread(void *ptr);
void* flow_to_txt_obs_thread(void *ptr);
void* flow_to_txt_stat_thread(void *ptr);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

int main(int argc, char *argv[]) {

  if(argc != 3) {
    std::cerr << "usage ./dummy path/to/folder/with/flows 'string-with-flowexporters'" << std::endl;
    std::cerr << "String with flow exporters: " << std::endl;
    std::cerr << " 'ip-domain-netflowversion,ip-domain-netflowversion, ...'" << std::endl;
    exit(-1);
  }

  std::cout << "Scan the following folder:" << std::endl;
  std::string flowpath(argv[1]);
  std::cout << flowpath << std::endl;

  std::cout << "Use the follwoing flow exporters" << std::endl;
  std::string flowexporters_s(argv[2]);
  std::cout << flowexporters_s << std::endl;

  run = true;

  // BUFFERS
  FlowContainerBuffer* buffer = new FlowContainerBuffer();

  FlowContainerPool* flow_container_pool = FlowContainerPool::instance();
  flow_container_pool->set_capacity(100); // this will reduce the number of parallel threads to one

  // Parser --------------------------------------------------------------------
  ParserUnitCSGFileNetflow::Configuration* parser_config;
  parser_config = new ParserUnitCSGFileNetflow::Configuration();

  parser_config->set_path(argv[1]);
  parser_config->set_scan_interval_s(-1); // single scan
  parser_config->set_output(buffer);
  parser_config->set_stat_interval_s(30);
  parser_config->set_remove_files_after(false);

  std::vector<std::string> exporter_addr;
  std::vector<int> exporter_domain;
  std::vector<int> exporter_version;

  std::vector<std::string> flowexporters_a = split(flowexporters_s, ',');
  std::vector<std::string>::iterator iter = flowexporters_a.begin();
   while (iter != flowexporters_a.end()) {
    std::cout << *iter << std::endl;
    std::vector<std::string> current_exporter_a = split(*iter, '-');
    if (current_exporter_a.size() != 3) {
      throw FlowBoxE("Invalid flow exporter",__FILE__, __LINE__);
    }
    exporter_addr.push_back(current_exporter_a[0]);
    exporter_domain.push_back(atoi((current_exporter_a[1]).c_str()));
    exporter_version.push_back(atoi((current_exporter_a[2]).c_str()));

    iter++;
  }
  int exporter_id = 1;
  for(unsigned int i = 0; i < exporter_addr.size(); i++){
    std::cout << "ADD EXPORTER: " << std::endl;
    std::cout << "  ADDR = " << exporter_addr[i] << std::endl;
    std::cout << "  DOMAIN = " << exporter_domain[i] << std::endl;
    std::cout << "  Version = " << exporter_version[i] << std::endl;

    struct sockaddr addr;
    addr.sa_family = AF_INET;
    inet_pton(
      AF_INET,
      exporter_addr[i].c_str(), 
      &((struct sockaddr_in*) (&addr))->sin_addr
    );

    // add a the exporter
    ParserUnitCSGFileNetflow::ExportDevice exporter(
      addr, 
      exporter_domain[i],
      exporter_version[i],
      exporter_id
    );
    exporter_id++;

    parser_config->add_export_device(exporter);
  };

  ParserUnitCSGFileNetflow* parser;
  parser = new ParserUnitCSGFileNetflow;
  parser->conf_push(*parser_config);

  pthread_t parser_data, parser_observation, parser_statistics;
  pthread_create(&parser_data, NULL, parser_data_thread, (void*) parser);
  pthread_create(&parser_observation, NULL, parser_obs_thread, (void*) parser);
  pthread_create(&parser_statistics, NULL, parser_stat_thread, (void*) parser);
  
  // FlowToTxt -----------------------------------------------------------------
  FlowToTxt::Configuration* flow_to_txt_config = new FlowToTxt::Configuration();
  flow_to_txt_config->set_input(buffer);
  flow_to_txt_config->set_output(NULL);
  flow_to_txt_config->set_stat_interval(60);

  FlowToTxt* dummy;
  dummy = new FlowToTxt;
  dummy->conf_push(*flow_to_txt_config);

  pthread_t flow_to_txt_data, flow_to_txt_observation, flow_to_txt_statistics;
  pthread_create(&flow_to_txt_data, NULL, flow_to_txt_data_thread, (void*) dummy);
  pthread_create(&flow_to_txt_observation, NULL, flow_to_txt_obs_thread, (void*) dummy);
  pthread_create(&flow_to_txt_statistics, NULL, flow_to_txt_stat_thread, (void*) dummy);

  // wait until data processing is over ----------------------------------------
  pthread_join(parser_data, NULL);
  pthread_join(flow_to_txt_data, NULL);
  run = false;

  sleep(2);

  std::cout << "Finished" << std::endl;
  return 0;
};
void* parser_data_thread(void *ptr ){
  ((ParserUnitCSGFileNetflow* )(ptr))->data_process();
  return NULL;
};
void* parser_obs_thread(void *ptr){
  ParserUnitCSGFileNetflow::Observation obs;
  while(run){
    obs = ((ParserUnitCSGFileNetflow* )(ptr))->obs_get();
    std::cout << "parser_obs_thread obs found" << std::endl;
    std::cout << obs.get_message()<< std::endl;

  };
  return NULL;
};
void* parser_stat_thread(void *ptr){
  ParserUnitCSGFileNetflow::Statistics stat;
  while(run){
    stat = ((ParserUnitCSGFileNetflow* )(ptr))->stat_get();
    std::cout << "parser_stat_thread stat found" << std::endl;
    std::cout << stat.head_to_s() << std::endl;
    std::cout << stat.to_s() << std::endl;
  };
  return NULL;
};
void* flow_to_txt_data_thread(void *ptr ){
  ((FlowToTxt* )(ptr))->data_process();
  return NULL;
};
void* flow_to_txt_obs_thread(void *ptr){
  FlowToTxt::Observation obs;
  while(run){
    obs = ((FlowToTxt* )(ptr))->obs_get();
    std::cout << "flow_to_txt_obs_thread obs found" << std::endl;
    std::cout << obs.get_message()<< std::endl;
  };
  return NULL;
};
void* flow_to_txt_stat_thread(void *ptr){
  FlowToTxt::Statistics stat;
  while(run){
    stat = ((FlowToTxt* )(ptr))->stat_get();
    std::cout << "flow_to_txt_stat_thread stat found" << std::endl;
    std::cout << stat.to_s() << std::endl;
  };
  return NULL;
};
