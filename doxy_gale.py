#!/usr/bin/python
import os
import doxy.module as module
import doxy.debug as debug
import doxy.tools as tools

def create(target, module_name):
	my_module = module.Module(__file__, module_name)
	my_module.set_version("version.txt")
	my_module.set_title("Gale: Graphic Abstraction Layer for Ewol")
	my_module.set_website("http://atria-soft.github.io/" + module_name)
	my_module.set_website_sources("http://github.com/atria-soft/" + module_name)
	my_module.add_path([
	    module_name,
	    "doc"
	    ])
	my_module.add_sample_path([
	    "sample"
	    ])
	my_module.add_depend([
	    'elog',
	    'etk'
	    ])
	my_module.add_exclude_symbols([
	    '*operator<<*',
	    ])
	my_module.add_exclude_file([
	    'debug.h',
	    ])
	my_module.add_file_patterns([
	    '*.h',
	    '*.md',
	    ])
	
	return my_module