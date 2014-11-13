#!/usr/bin/env python
# encoding: utf-8
VERSION='0.0.1'
APPNAME='network_toolbox'

srcdir = '.'
blddir = 'build'
import sys

def options(opt):
    opt.load('compiler_cxx boost')
    opt.load('boost')
    opt.load('eigen')
    opt.load('xcode')

def configure(conf):
    conf.load('compiler_cxx')
    conf.check_cfg(path='gsl-config', package='gsl', uselib_store='GSL', args='--cflags --libs')
    conf.check_boost(lib='graph regex program_options')
    conf.load('xcode')

#    conf.env.SHLIB_MARKER="-Wl,-Bstatic"
    conf.env.append_value('LINKFLAGS', ['-pthread'])
    conf.env.append_value('CXXFLAGS', ['-O3', '-std=gnu++0x'])
    conf.load('eigen')
    conf.check_eigen()
    conf.env['LIB_BLISS']='bliss'
    conf.env['LIB_GMP']='gmp'

    print conf.env['CPPPATH_EIGEN']

def build(bld):
    bld.recurse('src/')
