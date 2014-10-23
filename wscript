

from waflib import Configure, Options
Configure.autoconfig = True

def options(opt):
    opt.load("compiler_c gnu_dirs")

def configure(conf):
    conf.load("compiler_c gnu_dirs")

    conf.check_cc(fragment="int main() { return 0; }\n")

    conf.env.CXXFLAGS = ["-O2", "-Wall", "-Werror", "-g"]
    conf.env.CFLAGS = conf.env.CXXFLAGS + ["-std=gnu99", "-D_GNU_SOURCE"]

def build(bld):
    bld.program(source="mmap_test.c",
                target="mmap_test")
