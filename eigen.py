#! /usr/bin/env python
# encoding: utf-8
# JB Mouret - 2009

"""
Quick n dirty eigen3 detection
"""

import os, glob, types
from waflib.Configure import conf


def options(opt):
	opt.add_option('--eigen', type='string', help='path to eigen', dest='eigen')


@conf
def check_eigen(conf):
	if conf.options.eigen:
		conf.env.INCLUDES_EIGEN = [conf.options.eigen]
		conf.env.LIBPATH_EIGEN = [conf.options.eigen]
	else:
		conf.env.INCLUDES_EIGEN = ['/usr/include/eigen2', 
                                           '/usr/local/include/eigen3', 
                                           '/usr/include', '/usr/local/include']
	res = conf.find_file('Eigen/Core', conf.env.INCLUDES_EIGEN)

	return 1


