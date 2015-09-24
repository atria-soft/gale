#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools

def get_desc():
	return "Simple windows with gale"

def create(target):
	my_module = module.Module(__file__, 'gale-sample-basic', 'BINARY')
	# add the file to compile:
	my_module.add_src_file([
		'basic.cpp'
		])
	# add dependency of gale
	my_module.add_module_depend(['gale'])
	my_module.copy_file('basic.frag')
	my_module.copy_file('basic.vert')
	return my_module




