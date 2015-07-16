#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools

def get_desc():
	return "Simple windows with gale"

def create(target):
	myModule = module.Module(__file__, 'gale-sample-basic', 'BINARY')
	# add the file to compile:
	myModule.add_src_file([
		'basic.cpp'
		])
	# add dependency of gale
	myModule.add_module_depend(['gale'])
	return myModule




