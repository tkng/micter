APPNAME = 'micter'
VERSION = '0.1.0'

srcdir = '.'
blddir = 'build'

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.env['CXXFLAGS'] = ['-W', '-Wall', '-O2', '-g']

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
