#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "SAMPLE"

def get_desc():
	return "Simple windows with gale"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def configure(target, my_module):
	my_module.add_src_file([
	    'sample/basic.cpp'
	    ])
	my_module.add_depend([
	    'gale',
	    'test-debug'
	    ])
	my_module.copy_file('sample/basic.frag')
	my_module.copy_file('sample/basic.vert')
	return True




