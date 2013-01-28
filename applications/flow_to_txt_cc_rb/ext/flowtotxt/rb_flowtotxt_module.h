/*******************************************************************************
 * File: rb_flowtotxt_module.h 
 *
 * Copyright (c) 2011-13, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (schadomi@gmail.com) 
 *
 * Print flows to STDOUT
 *
 */

#ifndef RB_FLOWTOTXT_MODULE_H_
#define RB_FLOWTOTXT_MODULE_H_

#include "ruby.h"
#include <flowtotxt_module.h>

/* Changelog *******************************************************************
V 00.00.01, 2011.07.30
	- first version by Dominik Schatzmann
*/

#define RB_FLOWTOTXT_MODULE_UNWRAP FlowToTxtModule* dm; Data_Get_Struct(self, FlowToTxtModule, dm);
void rb_flowtotxt_module_free(void *p);
VALUE rb_flowtotxt_module_alloc(VALUE klass);

// configuration
VALUE rb_flowtotxt_module_conf_push(
  VALUE self, 
  VALUE configuration_h
);

// observation
VALUE rb_flowtotxt_module_obs_get_block(
  void* data
);
VALUE rb_flowtotxt_module_obs_get(
  VALUE self
);

// statistics
VALUE rb_flowtotxt_module_stat_get_block(
  void* data
);
VALUE rb_flowtotxt_module_stat_get(
  VALUE self
);

// data
VALUE rb_flowtotxt_module_data_process_block(
  void* data
);
VALUE rb_flowtotxt_module_data_process(
  VALUE self
);

#endif // RB_FLOWTOTXT_MODULE_H_
