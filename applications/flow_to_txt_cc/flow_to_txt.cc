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
 * @file   bi_flow.h
 * @author Dominik Schatzmann <schadomi@gmail.com>
 * @date   Januar, 2013
 * @brief  A module that prints out the flows on STDOUT.
 *
 */
#include "flow_to_txt.h"
//------------------------------------------------------------------------------
// class FlowToTxt::Const
//------------------------------------------------------------------------------
const std::string FlowToTxt::kVersion("00.00.01");
const int FlowToTxt::kVersionNbr = 1;

//------------------------------------------------------------------------------
// class FlowToTxt::Configuration
//------------------------------------------------------------------------------
const int FlowToTxt::Configuration::kStatisticDefaultInterval = 300;

FlowToTxt::Configuration::Configuration(){
  reset();
};
void FlowToTxt::Configuration::reset(void){
  input_ = NULL;
  output_ = NULL;
  stat_interval_= kStatisticDefaultInterval;
};

void FlowToTxt::Configuration::set_input(FlowContainerBuffer* input){
  input_ = input;
};
void FlowToTxt::Configuration::set_output(FlowContainerBuffer* output){
  output_ = output;
};
void FlowToTxt::Configuration::set_stat_interval(uint64_t interval){
  stat_interval_ = interval;
};

FlowContainerBuffer* FlowToTxt::Configuration::get_input(void) const {
  return(input_);
};
FlowContainerBuffer* FlowToTxt::Configuration::get_output(void) const {
  return(output_);
}
uint64_t FlowToTxt::Configuration::get_stat_interval(void) const {
  return(stat_interval_);
}
//------------------------------------------------------------------------------
// class FlowToTxt::Observation
//------------------------------------------------------------------------------
FlowToTxt::Observation::Observation(){
  reset();
};
void FlowToTxt::Observation::reset(void){
  valid_ = false;
  time_s_ = 0;
  message_ = "";
};
//------------------------------------------------------------------------------
bool FlowToTxt::Observation::get_valid(void) const {
  return(valid_);
};
uint64_t FlowToTxt::Observation::get_time_s(void) const {
  return(time_s_);
};
std::string FlowToTxt::Observation::get_message(void) const {
  return(std::string(message_));
};
//------------------------------------------------------------------------------
void FlowToTxt::Observation::set_valid(bool valid){
  valid_ = valid;
};
void FlowToTxt::Observation::set_time_s(uint64_t time_s){
  time_s_ = time_s;
};
void FlowToTxt::Observation::set_message(const std::string& message){
  message_ = message;
};
//------------------------------------------------------------------------------
// class FlowToTxt::Statistics
//------------------------------------------------------------------------------
FlowToTxt::Statistics::Statistics(){
  reset();
};
void FlowToTxt::Statistics::reset(void){
  valid_ = false;
  time_s_ = 0;
  duration_s_ = 0;
  flows_ = 0;
};
bool FlowToTxt::Statistics::get_valid(void){
  return(valid_);
};
uint64_t FlowToTxt::Statistics::get_time_s(void){
  return(time_s_);
};
uint64_t FlowToTxt::Statistics::get_duration_s(void){
  return(duration_s_);
};
uint64_t FlowToTxt::Statistics::get_flows(void){
  return(flows_);
};
void FlowToTxt::Statistics::set_valid(bool valid){
  valid_ = valid;
};
void FlowToTxt::Statistics::set_time_s(uint64_t time_s){
  time_s_ = time_s;
};
void FlowToTxt::Statistics::set_duration_s(uint64_t duration_s){
  duration_s_ = duration_s;
};
void FlowToTxt::Statistics::set_flows(uint64_t flows){
  flows_ = flows;
};
void FlowToTxt::Statistics::add_flows(uint64_t flows_inc){
  flows_ += flows_inc;
};
std::string FlowToTxt::Statistics::to_s(void){
  std::stringstream tmp;
  tmp << time_s_ << ", ";
  tmp << duration_s_ << ", ";
  tmp << flows_;
  return(std::string(tmp.str()));
};
//------------------------------------------------------------------------------
// class FlowToTxt
//------------------------------------------------------------------------------
FlowToTxt::FlowToTxt() {

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
FlowToTxt::~FlowToTxt() {

};

//------------------------------------------------------------------------------
// class FlowToTxt CONFIGURATION
//------------------------------------------------------------------------------
void FlowToTxt::conf_push(const Configuration& config){
  fb_sem_wait(&conf_critical_section_sem_); // CRITICAL SECTION::START
  conf_in_.push(config);
  conf_available_ = true;
  fb_sem_post(&conf_critical_section_sem_); // CRITICAL SECTION::STOP
  return;
};
void FlowToTxt::conf_pop(void) {
  fb_sem_wait(&conf_critical_section_sem_); // CRITICAL SECTION::START
  while(conf_in_.size() > 0) {
    // get config from queue
    Configuration migrate(conf_in_.front());
    conf_in_.pop();
    // apply config
    conf_migrate(migrate);
  }
  conf_available_ = false;
  fb_sem_post(&conf_critical_section_sem_); // CRITICAL SECTION::STOP
  return;
};
void FlowToTxt::conf_migrate(const Configuration& migrate) {
  data_input_ = migrate.get_input();
  data_output_ = migrate.get_output();
  stat_interval_s_ = migrate.get_stat_interval();
  return;
};
//------------------------------------------------------------------------------
// class FlowToTxt OBSERVATION
//------------------------------------------------------------------------------
FlowToTxt::Observation FlowToTxt::obs_get(void){
  FlowToTxt::Observation obs;
  fb_sem_wait(&obs_available_sem_); // WAIT ON DATA
  fb_sem_wait(&obs_critical_section_sem_); // CRITICAL SECTION::START
  assert(obs_out_.size() >= 1);
  obs =  obs_out_.front();
  obs_out_.pop();
  fb_sem_post(&obs_critical_section_sem_); // CRITICAL SECTION::STOP
  obs.set_valid(true);
  return(FlowToTxt::Observation(obs));
};
void FlowToTxt::obs_push(const std::string&  message){
  FlowToTxt::Observation obs;
  obs.set_time_s(stat_export_next_s_);
  obs.set_message(message);
  fb_sem_wait(&obs_critical_section_sem_); // CRITICAL SECTION::START
  obs_out_.push(obs); // push data
  fb_sem_post(&obs_critical_section_sem_); // CRITICAL SECTION::STOP
  fb_sem_post(&obs_available_sem_); // SIGNAL DATA
};
//------------------------------------------------------------------------------
// class FlowToTxt STATISTICS
//------------------------------------------------------------------------------
FlowToTxt::Statistics FlowToTxt::stat_get(void){

  FlowToTxt::Statistics stat;

  // block until we have data
  int err = sem_wait(&stat_available_sem_);
  if(err != 0) {
    // waiting was not successful (interrupt?)
    stat.set_valid(false);
  } else {
    // critical section -- pop data
    if(sem_wait(&stat_critical_section_sem_) != 0){
      // semaphore failed, abort
      throw FlowBoxE("semaphore: stat_critical_section_sem_ failed",__FILE__, __LINE__);
    };
    assert(stat_out_.size() >= 1);
    stat = stat_out_.front();
    stat_out_.pop();
    sem_post(&stat_critical_section_sem_);
    stat.set_valid(true);
  }

  return(FlowToTxt::Statistics(stat));
}
void FlowToTxt::stat_export(uint64_t now_s){

  if(stat_current_.get_time_s() != 0) {

    stat_current_.set_duration_s(now_s - stat_current_.get_time_s());
  
    // critical Section -- push data
    if(sem_wait(&stat_critical_section_sem_) != 0){
      // semaphore failed, abort
      throw FlowBoxE("semaphore: stat_critical_section_sem_ failed",__FILE__, __LINE__);
    };
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
// class FlowToTxt DATA
//------------------------------------------------------------------------------
void FlowToTxt::data_process(
  FlowContainer::iterator flow,
  FlowContainer::iterator end
){
  uint64_t flow_c = 0;
  while(flow != end) {
    if(flow->valid_ == true) { // process only valid flow data
	    std::cout << flow->to_s() << std::endl;
      flow_c++;
    };
    flow++;
  }; // while
  stat_current_.add_flows(flow_c);
};
void FlowToTxt::data_process(void){
  obs_push("data_process -- started");
  FlowContainer* fc;
  while (true) {
    // do we have some a new config?
    if (conf_available_)
      conf_pop();

    // ups no input buffer ...
    if (data_input_ == NULL) {
      obs_push("data_thread_main -- empty data_input_");
      sleep(5);
      continue;
    }

    // blocking wait on some input
    fc = data_input_->pop();

    // semaphore can be interrupted by system interrupts, timeouts or signals
    // we have to check if this pointer is valid.
    if (fc == NULL) {
      // invalid data but WHY?

      // FIN: is the processing over?
      if (data_input_->is_state_fin()) {
        // push/flush our internal results
        // NOP

        // forward the signal to the next buffer (if possible)
        if (data_output_ != NULL) {
          data_output_->signal_fin();
        }

        // break the while(true) ...
        // ... and say goodbye
        break;

        // unknown reason
      } else {
        // WTF ??
        obs_push("data_thread_main -- empty container?!?");
        continue;
      }
    } else {
      // WORK:
      data_process(fc->begin(), fc->end_used());

      // forward the buffer to the next element
      if (data_output_ != NULL) {
        data_output_->push(fc);
      } else {
        data_container_pool_->push(fc);
      }
    }
  }// while(true)

  // ... say goodby
  // clean up
  // NOP
  obs_push("data_thread_main -- finished");
};
