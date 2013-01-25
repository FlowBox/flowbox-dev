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
 * @file   flow_to_txt.h
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Januar, 2013
 * @brief  A module that prints out the flows on STDOUT.
 *
 * The FlowToTxt Unit can be used to print out the flow information on the 
 * Standart out.
 *
 */

#ifndef FLOW_TO_TXT_H_
#define FLOW_TO_TXT_H_

#include <semaphore.h>

#include <cassert>
#include <cstring>
#include <queue>
#include <sstream>

#include "flow_container_pool.h"
#include "flow_container_buffer.h"

/* Changelog ******************************************************************
V 00.00.01, 2013.02.25
	- Init by Dominik Schatzmann
*/

class FlowToTxt {

  public:
    static const std::string kVersion;
    static const int kVersionNbr;

    class Configuration {
      public:
        static const int kStatisticDefaultInterval;
      private:
        FlowContainerBuffer* input_;
        FlowContainerBuffer* output_;
        uint64_t stat_interval_;
      public:
        Configuration();
        void reset(void);

        void set_input(FlowContainerBuffer* input);
        void set_output(FlowContainerBuffer* output);
        void set_stat_interval(uint64_t interval);

        FlowContainerBuffer* get_input(void) const;
        FlowContainerBuffer* get_output(void) const;
        uint64_t get_stat_interval() const;
    };
    class Observation {
      private:
        bool valid_;
        uint64_t time_s_;
        std::string message_;

      public:
        Observation();
        void reset();

        bool get_valid(void) const;
        uint64_t get_time_s(void) const;
        std::string get_message(void) const;

        void set_valid(bool valid);
        void set_time_s(uint64_t time_s);
        void set_message(const std::string& message);
    };
    class Statistics {
      private:
        bool valid_;
        uint64_t time_s_;
        uint64_t duration_s_;
        uint64_t flows_;

      public:
        Statistics();
        void reset(void);

        bool get_valid(void);
        uint64_t get_time_s(void);
        uint64_t get_duration_s(void);
        uint64_t get_flows(void);

        void set_valid(bool valid);
        void set_time_s(uint64_t time_s);
        void set_duration_s(uint64_t duration_s);
        void set_flows(uint64_t flows);
        void add_flows(uint64_t flows_inc);

        std::string to_s(void);
    };

  // ## CLASS VARIABLES ####################################################### 
  private:
    // -- CONFIGURATION: INCOMING ----------------------------------------------
    sem_t conf_critical_section_sem_;
    std::queue<Configuration> conf_in_;
    bool conf_available_;

    // -- OBSERVATION: OUTGOING ------------------------------------------------
    sem_t obs_critical_section_sem_;
    sem_t obs_available_sem_;
    std::queue<Observation> obs_out_;

    // -- STATISTICS: OUTGOING--------------------------------------------------
    sem_t stat_critical_section_sem_;
    sem_t stat_available_sem_;
    std::queue<Statistics> stat_out_;

    Statistics stat_current_;
    uint64_t stat_interval_s_;
    uint64_t stat_export_next_s_;

    // -- DATA STREAM  ---------------------------------------------------------
    FlowContainerBuffer* data_input_;
    FlowContainerBuffer* data_output_;
    FlowContainerPool* data_container_pool_;

  // ## CLASS FUNCTIONS ########################################################
  private:
   // -- CONFIGURATION: INCOMING ----------------------------------------------
    void conf_pop(void);
    void conf_migrate(const Configuration& migrate);
    // -- OBSERVATION: OUTGOING ------------------------------------------------
    void obs_push(const std::string&  message);
    // -- STATISTICS: OUTGOING--------------------------------------------------
    void stat_export(uint64_t now_s);

    // -- DATA STREAM  ---------------------------------------------------------
    void data_process(
      FlowContainer::iterator flow,
      FlowContainer::iterator end
    );
    void data_process(FlowContainer::iterator& flow);

  // ## CLASS API ##############################################################
  public:
    FlowToTxt();
    ~FlowToTxt();

    // -- CONFIGURATION: INCOMING ----------------------------------------------
    void conf_push(const Configuration& config);

    // -- OBSERVATION: OUTGOING ------------------------------------------------
    Observation obs_get(void);

    // -- STATISTICS: OUTGOING--------------------------------------------------
    Statistics stat_get(void);

    // -- DATA STREAM  ---------------------------------------------------------
    void data_process(void);

};
#endif // FLOW_TO_TXT_H_
