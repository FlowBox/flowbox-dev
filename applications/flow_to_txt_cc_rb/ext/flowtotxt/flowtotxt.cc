#include <ruby.h>
static VALUE rb_flowtotxt_o;
static VALUE rb_flowtotxt;

#include "rb_flowtotxt_module.h"
static VALUE rb_eFlowToTxtModule;
static VALUE rb_cFlowToTxtModule;
static VALUE rb_cFlowToTxtModuleObs;
static VALUE rb_cFlowToTxtModuleStat;

extern "C" void Init_flowtotxt() {

  rb_flowtotxt_o = rb_define_module("FlowToTxt");
  rb_flowtotxt = rb_define_module_under(rb_flowtotxt_o, "Core");

//------------------------------------------------------------------------------
// FlowToTxtModule
//------------------------------------------------------------------------------
	rb_eFlowToTxtModule = rb_define_class_under(rb_flowtotxt, "FlowToTxtModuleError", rb_eStandardError);
	rb_cFlowToTxtModule = rb_define_class_under(rb_flowtotxt, "FlowToTxtModule", rb_cObject);
	rb_cFlowToTxtModuleObs = rb_define_class_under(rb_flowtotxt, "FlowToTxtModuleObs", rb_cObject);
	rb_cFlowToTxtModuleStat = rb_define_class_under(rb_flowtotxt, "FlowToTxtModuleStat", rb_cObject);

	rb_define_alloc_func(rb_cFlowToTxtModule, rb_flowtotxt_module_alloc);

  // configuration
 	rb_define_method(rb_cFlowToTxtModule, "conf_push", RUBY_METHOD_FUNC(rb_flowtotxt_module_conf_push), 1);

  // observation
	rb_define_method(rb_cFlowToTxtModule, "obs_get", RUBY_METHOD_FUNC(rb_flowtotxt_module_obs_get), 0);

  // statistics
	rb_define_method(rb_cFlowToTxtModule, "stat_get", RUBY_METHOD_FUNC(rb_flowtotxt_module_stat_get), 0);

  // data
	rb_define_method(rb_cFlowToTxtModule, "data_process", RUBY_METHOD_FUNC(rb_flowtotxt_module_data_process), 0);

};
