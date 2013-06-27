#
# sconstruct for building textgen
#
# usage:
#       scons [-j 4] [-q] [debug=1|profile=1] [objdir=<path>]
#
# notes:
#       the three variants share the same output and object file names;
#       changing from one version to another will cause a full recompile.
#       this is intentional (instead of keeping, say, out/release/...)
#       for compatibility with existing test suite etc. and because normally
#       different machines are used only for debug/release/profile.
#
# windows usage:
#	run 'vcvars32.bat' or similar before using us, to give right
#	env.var. setups for visual c++ 2008 command line tools & headers.
#
# os x usage:
#   source has '#include <newbase/...>'. to be able to deal with these (without
#   altering the source), do such:
#
#       ln -s ../newbase/include newbase
#

import os.path

Help(""" 
    Usage: scons [-j 4] [-Q] [debug=1|profile=1] [objdir=<path>] smartmet_textgen.a|lib
    
    Or just use 'make release|debug|profile', which point right back to us.
""") 

Decider("MD5-timestamp") 

DEBUG=      int( ARGUMENTS.get("debug", 0) ) != 0
PROFILE=    int( ARGUMENTS.get("profile", 0) ) != 0
RELEASE=    (not DEBUG) and (not PROFILE)     # default

OBJDIR=     ARGUMENTS.get("objdir","obj")
PREFIX=     ARGUMENTS.get("prefix","/usr/")

env= Environment()

LINUX=  env["PLATFORM"]=="posix"
OSX=    env["PLATFORM"]=="darwin"
WINDOWS= env["PLATFORM"]=="win32"

#
# scons does not pass env.vars automatically through to executing commands.
# on windows, we want it to get them all (visual c++ 2008).
#
if WINDOWS:
    env.replace( ENV= os.environ )

env.Append( CPPPATH= [ "./include" ] )
env.Append( CPPPATH= [ "/usr/include/gdal" ] )

# Asko's temporary
#
# env.Append( CPPPATH= [ "asko/FrostProto", "asko/TempOverviewProto" ] )

if WINDOWS: 
    if env["cc"]=="cl":
        env.Append( CXXFLAGS= ["/ehsc"] )
else:
    env.Append( CPPDEFINES= ["UNIX"] )
    env.Append( CXXFLAGS= [
        # mainflags from orig. makefile ('-fpic' is automatically added by scons)
	"-fPIC",
        "-Wall", 
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
	    
	    # difficultflags from orig. makefile (flags that cause some choking,
	    # would be good but not used)
	    #
	    #"-Weffc++",
	    #"-Wredundant-decls",
	    #"-Wshadow",
	    #"-Woverloaded-virtual",
	    #"-Wctor-dtor-privacy",
	    #"-Wold-style-cast",
	    #"-pedantic",
    ] )

BOOST_POSTFIX=""
BOOST_PREFIX=""

if WINDOWS:
    # installed from 'boost_1_35_0_setup.exe' from boostpro internet page.
    #
    BOOST_INSTALL_PATH= "d:/boost/1_35_0"
    env.Append( CPPPATH= [ BOOST_INSTALL_PATH ] )
    env.Append( LIBPATH= [ BOOST_INSTALL_PATH + "/lib" ] )
    if debug:
        BOOST_POSTFIX= "-vc90-gd-1_35"
    else:
        BOOST_POSTFIX= "-vc90-1_35"
        BOOST_PREFIX= "lib"
    env.Append( libs= [ BOOST_PREFIX+"boost_iostreams"+BOOST_POSTFIX,
                        BOOST_PREFIX+"boost_date_time"+BOOST_POSTFIX
			 ] )

if WINDOWS:
    env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( LIBPATH= [ "../newbase" ] )

elif LINUX:
    # newbase from system install
    #
    env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( LIBPATH= [ "../newbase" ] )
    env.Append( CPPPATH= [ PREFIX+"/include/smartmet" ] )

elif OSX:
    # newbase from local cvs
    #
    #env.Append( CPPPATH= [ "../newbase/include" ] )
    env.Append( CPPPATH= [ "." ] )      # 'newbase' must be linked to '../newbase/include'
    env.Append( LIBPATH= [ "../newbase" ] )

    # fink
    #
    env.Append( CPPPATH= [ "/sw/include" ] )
    env.Append( LIBPATH= [ "/sw/lib" ] )

# mysql support

if LINUX:
    env.Append( CPPPATH = [ PREFIX+"/include/mysql" ] )
if OSX:
    env.Append( CPPPATH = [ "/sw/include/mysql" ] )

env.Append( LIBS = [ "mysql", "mysqlclient" ] )

# DEBUG settings
#
if DEBUG:
    if WINDOWS:
        if env["cc"]=="cl":
            env.Appendunique( CPPDEFINES=["_DEBUG","DEBUG"] )
            # DEBUG multithreaded dll runtime, no opt.
            env.Appendunique( CCFLAGS=["/mdd", "/od"] )
            # each obj gets own .pdb so parallel building (-jn) works
            env.Appendunique( CCFLAGS=["/zi", "/fd${target}.pdb"] )
    else:
        env.Append( CXXFLAGS=[ "-O0", "-g", "-Werror",
    
            # extraflags from orig. makefile (for 'DEBUG' target)
            #
            # "-Wconversion",
            "-ansi",
            "-Wcast-align",
            "-Wcast-qual",
            "-Winline",
            "-Wno-multichar",
            "-Wno-pmf-conversions",
            "-Woverloaded-virtual",
            "-Wpointer-arith",
            "-Wredundant-decls",
            "-Wsign-promo",
            "-Wwrite-strings",
        ] )

#
# release settings
#
if RELEASE or PROFILE:
    if WINDOWS:
        if env["CC"]=="cl":
            # multithreaded dll runtime, reasonable opt.
            env.Appendunique( CCFLAGS=["/md", "/ox"] )
    else:
        env.Append( CPPDEFINES="NDEBUG",
                    CXXFLAGS= ["-O2","-Wuninitialized", ]
	           )

#
# profile settings
#
if PROFILE:
    if WINDOWS:
        { }     # tbd
    else: 
        env.Append( CXXFLAGS="-g -pg" )




#---
# exceptions to the regular compilation rules (from orig. makefile)
#
# note: samples show 'env.replace( objdir=... )' to be able to use separate
#       object dir, but this did not work.    -- aka 15-sep-2008
#

objs= []

env.Append( LIBS= [ "smartmet_newbase" ] )

if not WINDOWS:
    env.Append( CPPDEFINES= "_REENTRANT" )

if WINDOWS:
    for fn in glob("source/*.cpp"): 
        s= os.path.basename( str(fn) )
	obj_s= OBJDIR+"/"+ s.replace(".cpp","")

        objs += env.object( obj_s, fn )
else:
    if DEBUG:
        e_o0= env.Clone()

        e_o0["CXXFLAGS"].append("-O0")

        e_noerror= env    # anyways no -Werror

    else:
        e_o0= env       # no change, anyways

        e_noerror= env.Clone()
        e_noerror["CXXFLAGS"].append( "-Wno-error" )
        e_noerror["CXXFLAGS"].append( "-Wuninitialized" )

    for fn in Glob("source/*.cpp"):
        s= os.path.basename( str(fn) )
	obj_s= OBJDIR+"/"+ s.replace(".cpp","")

	objs += env.Object( obj_s, fn )

# make just the static lib (at least it should be default for just 'scons')

env.Library( "smartmet_textgen", objs )


