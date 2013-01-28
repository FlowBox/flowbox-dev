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
 * @file   flowtotxt_module.cc
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Januar, 2013
 * @brief  A module that prints out the flows on STDOUT.
 *
 * The FlowToTxt Module can be used to print out the flow information on the 
 * standart out. 
 *
 */

#include "flowtotxt_module.h"
//------------------------------------------------------------------------------
// class FlowToTxtModule::Const
//------------------------------------------------------------------------------
const std::string FlowToTxtModule::kVersion("00.00.01");
const int FlowToTxtModule::kVersionNbr = 1;

//------------------------------------------------------------------------------
// class FlowToTxtModule::Configuration
//------------------------------------------------------------------------------
const int FlowToTxtModule::Configuration::kStatisticDefaultInterval = 300;

FlowToTxtModule::Configuration::Configuration(){
  reset();
};
void FlowToTxtModule::Configuration::reset(void){
  input_ = NULL;
  output_ = NULL;
  stat_interval_= kStatisticDefaultInterval;
};

void FlowToTxtModule::Configuration::set_input(FlowContainerBuffer* input){
  input_ = input;
};
void FlowToTxtModule::Configuration::set_output(FlowContainerBuffer* output){
  output_ = output;
};
void FlowToTxtModule::Configuration::set_stat_interval(uint64_t interval){
  stat_interval_ = interval;
};

FlowContainerBuffer* FlowToTxtModule::Configuration::get_input(void) const {
  return(input_);
};
FlowContainerBuffer* FlowToTxtModule::Configuration::get_output(void) const {
  return(output_);
}
uint64_t FlowToTxtModule::Configuration::get_stat_interval(void) const {
  return(stat_interval_);
}
//------------------------------------------------------------------------------
// class FlowToTxtModule::Observation
//------------------------------------------------------------------------------
FlowToTxtModule::Observation::Observation(){
  reset();
};
void FlowToTxtModule::Observation::reset(void){
  valid_ = false;
};
bool FlowToTxtModule::Observation::get_valid(void){
  return(valid_);
};
void FlowToTxtModule::Observation::set_valid(bool valid){
  valid_ = valid;
};
//------------------------------------------------------------------------------
// class FlowToTxtModule::Statistics
//------------------------------------------------------------------------------
FlowToTxtModule::Statistics::Statistics(){
  reset();
};
void FlowToTxtModule::Statistics::reset(void){
  valid_ = false;
  time_s_ = 0;
  duration_s_ = 0;
  flows_ = 0;
};
bool FlowToTxtModule::Statistics::get_valid(void){
  return(valid_);
};
uint64_t FlowToTxtModule::Statistics::get_time_s(void){
  return(time_s_);
};
uint64_t FlowToTxtModule::Statistics::get_duration_s(void){
  return(duration_s_);
};
uint64_t FlowToTxtModule::Statistics::get_flows(void){
  return(flows_);
};
void FlowToTxtModule::Statistics::set_valid(bool valid){
  valid_ = valid;
};
void FlowToTxtModule::Statistics::set_time_s(uint64_t time_s){
  time_s_ = time_s;
};
void FlowToTxtModule::Statistics::set_duration_s(uint64_t duration_s){
  duration_s_ = duration_s;
};
void FlowToTxtModule::Statistics::set_flows(uint64_t flows){
  flows_ = flows;
};
void FlowToTxtModule::Statistics::add_flows(uint64_t flows_inc){
  flows_ += flows_inc;
};
std::string FlowToTxtModule::Statistics::to_s(void){
  std::stringstream tmp;
  tmp << time_s_ << ", ";
  tmp << duration_s_ << ", ";
  tmp << flows_;
  return(std::string(tmp.str()));
};
//------------------------------------------------------------------------------
// class FlowToTxtModule
//------------------------------------------------------------------------------
FlowToTxtModule::FlowToTxtModule() {

  // -- CONFIGURATION ----------------------------------------------------------
  sem_init(&conf_critical_section_sem_, 0, 1);
  conf_available_ = false;

  // -- OBSERVATION ------------------------------------------------------------
  sem_init(&obs_critical_section_sem_, 0, 1);
  sem_init(&obs_available_sem_ , 0, 0);

  // -- STATISTICS -------------------------------------------------------------
  sem_init(&stat_critical_section_sem_, 0, 1);
  sem_init(&stat_available_sem_ , 0, 0);

  stat_current_.reset();
  stat_interval_s_ = Configuration::kStatisticDefaultInterval;
  stat_export_next_s_ = 0;

  // -- DATA STREAM  -----------------------------------------------------------
  data_input_ = NULL;
  data_output_ = NULL;
  data_container_pool_ = FlowContainerPool::instance();

 };
FlowToTxtModule::~FlowToTxtModule() {

};

//------------------------------------------------------------------------------
// class FlowToTxtModule CONFIGURATION
//------------------------------------------------------------------------------
void FlowToTxtModule::conf_push(const Configuration& config){
  //std::cout << "FlowToTxtModule::conf_push" << std::endl;
  // CRITICAL SECTION:: START
  while(sem_wait(&conf_critical_section_sem_) != 0) ;
  conf_in_.push(config);
  conf_available_ = true;
  // CRITICAL SECTION:: STOP
  sem_post(&conf_critical_section_sem_);
  return;
};
void FlowToTxtModule::conf_pop(void) {
  // CRITICAL SECTION:: START
  while(sem_wait(&conf_critical_section_sem_) != 0) ;
  while(conf_in_.size() > 0) {

    // get config from queue
    Configuration migrate(conf_in_.front());
    conf_in_.pop();

    // apply config
    conf_migrate(migrate);
  }
  conf_available_ = false;
  // CRITICAL SECTION:: STOP
  sem_post(&conf_critical_section_sem_);
  return;
};
void FlowToTxtModule::conf_migrate(const Configuration& migrate) { 

  data_input_ = migrate.get_input();
  data_output_ = migrate.get_output();
  stat_interval_s_ = migrate.get_stat_interval();
  //std::cout << "data_input_ " <<  data_input_ << std::endl;
  //std::cout << "data_output_ " <<  data_output_ << std::endl;
  //std::cout << "stat_interval_s_ " <<  stat_interval_s_ << std::endl;
  return;
};
//------------------------------------------------------------------------------
// class FlowToTxtModule OBSERVATION
//------------------------------------------------------------------------------
FlowToTxtModule::Observation FlowToTxtModule::obs_get(void){

  FlowToTxtModule::Observation obs;

  // block until we have data
  int err = sem_wait(&obs_available_sem_);
  if(err != 0) {
    // waiting was not successful (interrupt?)
    obs.set_valid(false);
  } else {
    // critical Section -- pop data
    while(sem_wait(&obs_critical_section_sem_) != 0) ; // enforce success
    assert(obs_out_.size() >= 1);
    obs =  obs_out_.front();
    obs_out_.pop();
    sem_post(&obs_critical_section_sem_);
    obs.set_valid(true);
  }

  return(FlowToTxtModule::Observation(obs));
}
//------------------------------------------------------------------------------
// class FlowToTxtModule STATISTICS
//------------------------------------------------------------------------------
FlowToTxtModule::Statistics FlowToTxtModule::stat_get(void){

  FlowToTxtModule::Statistics stat;

  // block until we have data
  int err = sem_wait(&stat_available_sem_);
  if(err != 0) {
    // waiting was not successful (interrupt?)
    stat.set_valid(false);
  } else {
    // critical Section -- pop data
    while(sem_wait(&stat_critical_section_sem_) != 0) ;
    assert(stat_out_.size() >= 1);
    stat = stat_out_.front();
    stat_out_.pop();
    sem_post(&stat_critical_section_sem_);
    stat.set_valid(true);
  }

  return(FlowToTxtModule::Statistics(stat));
}
void FlowToTxtModule::stat_export(uint64_t now_s){

  if(stat_current_.get_time_s() != 0) {

    stat_current_.set_duration_s(now_s - stat_current_.get_time_s());
  
    // critical Section -- push data
    while(sem_wait(&stat_critical_section_sem_) != 0) ;
    stat_out_.push(stat_current_);
    sem_post(&stat_critical_section_sem_);

    // inform statistic thread
    sem_post(&stat_available_sem_);
  };
  stat_current_.reset();
  stat_current_.set_time_s(now_s);
  stat_export_next_s_ = ((now_s/stat_interval_s_)+1)*stat_interval_s_;
}

//------------------------------------------------------------------------------
// class FlowToTxtModule DATA
//------------------------------------------------------------------------------
void FlowToTxtModule::data_process(
  FlowContainer::iterator flow,
  FlowContainer::iterator end
){
  uint64_t flow_c = 0;

  // walk over flows
  while(flow != end) {
    if(flow->valid_ == true) { // process only valid data

     // packet time: stat switch?
     if(flow->start_s_ > stat_export_next_s_) {
        stat_export(flow->start_s_);
     };
     flow_c++;
    };
    flow++;
  }; // while
  stat_current_.add_flows(flow_c);
};
void FlowToTxtModule::data_process(void){
  FlowContainer* fc;

  std::cout << "FlowToTxtModule::data_process -- started" << std::endl;
  std::cout.flush();

  while(true) {

    // do we have some a new config?
    if(conf_available_)
      conf_pop();

    // ups no input buffer ...
    if(data_input_ == NULL){
      std::cout << "FlowToTxtModule::data_thread_main -- empty data_input_ retry in (5)" << std::endl;
      sleep(5);
      continue;
    }

    // blocking wait on some input
    //std::cout << "FlowToTxtModule::data_thread_main --   in: fc = data_input_->pop();" << std::endl;
    fc = data_input_->pop();
    //std::cout << "FlowToTxtModule::data_thread_main --   back: fc = data_input_->pop();" << std::endl;
    //std::cout.flush();

    // since the locking wait can be interrupted 
    // by system interrupts, timeouts or signals
    // we have to check if there is the pointer 
    // is valid.
    if(fc == NULL) {
      // invalid data but WHY?

      // FIN: is the processing over?
      if(data_input_->is_state_fin()) {

        // push/flush our internal results
        // NOP

        // forward the signal to the next buffer (if possible)
        if(data_output_ != NULL) {
          data_output_->signal_fin();
        }

        // break the while(true) ...
        // ... and say goodbye
        break;

      // unknown reason
      } else {
          // WTF ??
          std::cout << "I've got an empty container pointer without good reason !!" << std::endl;
          continue;
      }
    } else {
      // WORK:
      data_process(fc->begin(), fc->end_used());

      // forward the buffer to the next element
      if(data_output_ != NULL) {
        data_output_->push(fc);
      } else {
        data_container_pool_->push(fc);
      }
    }
  } // while(true)

  // ... say goodby
  // clean up
  // NOP
  //std::cout << "FlowToTxtModule::data_thread_main -- finished" << std::endl;
};
