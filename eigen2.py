#! /usr/bin/env python
# encoding: utf-8
# JB Mouret - 2009

"""
Quick n dirty eigen2 detection
"""

import os, glob, types
from waflib.Configure import conf

def options(opt):
	opt.add_option('--eigen2', action='store', default='', dest='eigen2', help='path to eigen2')

@conf
def check_eigen2(conf):
	opt = conf.options
	conf.start_msg("checking Eigen2")
	conf.env['EIGEN2_FOUND'] = False
	if opt.eigen2:
		conf.env['INCLUDES_EIGEN2'] = [Options.options.eigen2]
		conf.env['LIBPATH_EIGEN2'] = [Options.options.eigen2]
	else:
		conf.env['INCLUDES_EIGEN2'] = ['/usr/include/eigen2', '/usr/local/include/eigen2', '/usr/include', '/usr/local/include']
		conf.env['LIBPATH_EIGEN2'] = ['/usr/lib', '/usr/local/lib']

	res = conf.find_file('Eigen/Core', conf.env['INCLUDES_EIGEN2'])
	if (res == '') :
                conf.end_msg('Eigen2 not found!')
		return 0
	conf.end_msg('oK')
	conf.env['EIGEN2_FOUND'] = True
	return 1



def configure(conf):
	conf.check_eigen2()
