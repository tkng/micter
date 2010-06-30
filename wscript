APPNAME = 'micter'
VERSION = '0.1.0'

srcdir = '.'
blddir = 'build'

import sys

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.env['CXXFLAGS'] = ['-W', '-Wall', '-O2', '-g']

    print """
configure result:

Package:                 %s
build target:            %s
host endian:             %s
Compiler:                %s
Compiler version:        %s
CXXFLAGS:                %s
""" % (APPNAME+'-'+VERSION,
       conf.env.DEST_CPU+'-'+conf.env.DEST_OS,
       sys.byteorder,
       conf.env.COMPILER_CXX,
       '.'.join(conf.env.CC_VERSION),
       ' '.join(conf.env.CXXFLAGS))

def build(bld):
    bld(features = 'cxx cprogram',
        source = 'main.cpp micter.cpp util.cpp',
        target = 'micter',
        includes = '.')

    bld(features = 'cxx cprogram',
        source = 'micter_train.cpp micter.cpp util.cpp',
        target = 'micter-train',
        includes = '.')


def shutdown(ctx):
    pass

import Scripting
Scripting.dist_exts += ['.txt', '.model']
