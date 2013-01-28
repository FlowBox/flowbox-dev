/*******************************************************************************
 * File: rb_flowtotxt_module.cc 
 *
 * Copyright (c) 2011, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 *
 * This class performs a sanity check on the flows
 *
 */
#include "rb_flowtotxt_module.h"
void rb_flowtotxt_module_free(void *p) {
  delete (FlowToTxtModule*) p;
  p = NULL;
};
VALUE rb_flowtotxt_module_alloc(VALUE klass) {
  FlowToTxtModule* p = NULL;
  VALUE obj;
  p = new FlowToTxtModule();
  if (p == NULL)
      rb_raise(rb_path2class("FlowToTxt::Core::FlowToTxtModuleError"), "Out of Memory");

  obj = Data_Wrap_Struct(klass, 0, rb_flowtotxt_module_free, p);
  return(obj);
};

// configuration
VALUE rb_flowtotxt_module_conf_push(
  VALUE self, 
  VALUE configuration_h
){
  RB_FLOWTOTXT_MODULE_UNWRAP
  FlowToTxtModule::Configuration conf_c;

  // input folder
  VALUE input = rb_hash_aref(configuration_h, ID2SYM(rb_intern("input")));
  if (TYPE(input) == T_DATA) {
    FlowContainerBuffer* input_buf_p;
    Data_Get_Struct(input, FlowContainerBuffer, input_buf_p);
    conf_c.set_input(input_buf_p);
  };

  // output
  VALUE output = rb_hash_aref(configuration_h, ID2SYM(rb_intern("output")));
  if (TYPE(output) == T_DATA) {
    FlowContainerBuffer* output_buf_p;
    Data_Get_Struct(output, FlowContainerBuffer, output_buf_p);
    conf_c.set_output(output_buf_p);
  }

  // stat_interval
  VALUE export_stat_interval_s = rb_hash_aref(configuration_h, ID2SYM(rb_intern("stat_interval_s")));
  Check_Type(export_stat_interval_s, T_FIXNUM);
  conf_c.set_stat_interval(NUM2INT(export_stat_interval_s));

  dm->conf_push(conf_c);
  return(self);
};

// observation
VALUE rb_flowtotxt_module_obs_get_block(
  void* data
){

  FlowToTxtModule* dm = (FlowToTxtModule*) data;
  FlowToTxtModule::Observation* obs = new FlowToTxtModule::Observation();
  (*obs) = dm->obs_get(); // blocking
 
  // rb_bug("object allocation during garbage collection phase");
  // -- hot fix -- don't allocate ruby memory in non-blocking threads
  VALUE klass = rb_path2class("FlowToTxt::Core::FlowToTxtModuleObs");
  VALUE observation = Data_Wrap_Struct(klass, 0, NULL, obs);

  return(observation);
}
VALUE rb_flowtotxt_module_obs_get(
  VALUE self
){
  RB_FLOWTOTXT_MODULE_UNWRAP
  VALUE observation = rb_thread_blocking_region(
    rb_flowtotxt_module_obs_get_block,
    dm, 
    RUBY_UBF_IO, 
    0
  );

  FlowToTxtModule::Observation* obs;
  Data_Get_Struct(observation, FlowToTxtModule::Observation, obs);

  VALUE answer;
  if(obs->get_valid() == false) {
    // an invalid observation, return just nil
    answer = Qnil;
  } else {
    // convert c++ data to ruby data
    answer = rb_hash_new();
  }

  delete obs;
  return(answer);
};

// statistics
VALUE rb_flowtotxt_module_stat_get_block(
  void* data
){

  FlowToTxtModule* dm = (FlowToTxtModule*) data;
  FlowToTxtModule::Statistics* stat = new FlowToTxtModule::Statistics();
  (*stat) = dm->stat_get();

  // rb_bug("object allocation during garbage collection phase");
  // -- hot fix -- don't allocate ruby memory in non-blocking threads
  VALUE klass = rb_path2class("FlowToTxt::Core::FlowToTxtModuleStat");
  VALUE statistics = Data_Wrap_Struct(klass, 0, NULL, stat);

  return(statistics);
};
VALUE rb_flowtotxt_module_stat_get(
  VALUE self
){
  RB_FLOWTOTXT_MODULE_UNWRAP
  VALUE statistic = rb_thread_blocking_region(
    rb_flowtotxt_module_stat_get_block,
    dm, 
    RUBY_UBF_IO, 
    0
  );

  FlowToTxtModule::Statistics* stat;
  Data_Get_Struct(statistic, FlowToTxtModule::Statistics, stat);

  VALUE answer;
  if(stat->get_valid() == false) {
    // an invalid observation, return just nil
    answer = Qnil;
  } else {

    // convert c++ data to ruby data
    answer = rb_hash_new();
    rb_hash_aset(answer, rb_str_new2("time_s"), ULL2NUM(stat->get_time_s()));
    rb_hash_aset(answer, rb_str_new2("duration_s"), ULL2NUM(stat->get_duration_s()));
    rb_hash_aset(answer, rb_str_new2("flows"), ULL2NUM(stat->get_flows()));
  }

  delete stat;
  return(answer);
};

// data
VALUE rb_flowtotxt_module_data_process_block(
  void* data
){
  FlowToTxtModule* dm = (FlowToTxtModule*) data;
  dm->data_process();
  return(Qnil);
};
VALUE rb_flowtotxt_module_data_process(
  VALUE self
){
  RB_FLOWTOTXT_MODULE_UNWRAP
    rb_thread_blocking_region(
    rb_flowtotxt_module_data_process_block,
    dm,
    RUBY_UBF_IO, 
    0
  );
  return(self);
};
