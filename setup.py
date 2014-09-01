#!/usr/bin/python

# by purcaro@gmail.com

import subprocess, sys, os, argparse
from setUpScripts.utils import Utils
from collections import namedtuple
from setUpScripts.color_text import ColorText as CT

BuildPaths = namedtuple("BuildPaths", 'url build_dir build_sub_dir local_dir')

def shellquote(s):
    # from http://stackoverflow.com/a/35857
    return "'" + s.replace("'", "'\\''") + "'"

def isMac():
    return sys.platform == "darwin"

class Paths():
    def __init__(self, externalLoc):
        self.base_dir = externalLoc;
        self.ext_tars = os.path.join(self.base_dir, "tarballs")
        self.ext_build = os.path.join(self.base_dir, "build")
        self.install_dir = os.path.join(self.base_dir, "local")
        Utils.mkdir(self.ext_tars)
        Utils.mkdir(self.ext_build)
        self.paths = {}
        self.paths["zi_lib"] = self.__zi_lib()
        self.paths["cppitertools"] = self.__cppitertools()
        self.paths["cppprogutils"] = self.__cppprogutils()
        self.paths["boost"] = self.__boost()
        self.paths["R-devel"] = self.__Rdevel()
        self.paths["cppcms"] = self.__cppcms()
        self.paths["bamtools"] = self.__bamtools()
        self.paths["pear"] = self.__pear()
        self.paths["mathgl"] = self.__mathgl()
        self.paths["armadillo"] = self.__armadillo()
        self.paths["mlpack"] = self.__mlpack()
        self.paths["liblinear"] = self.__liblinear()
        self.paths["bibcpp"] = self.__bibcpp()

    def path(self, name):
        if name in self.paths:
            return self.paths[name]
        raise Exception(name + " not found in paths")

    def __zi_lib(self):
        url = 'https://github.com/weng-lab/zi_lib.git'
        local_dir = os.path.join(self.install_dir, "zi_lib")
        return BuildPaths(url, '', '', local_dir)

    def __bamtools(self):
        url = 'https://github.com/pezmaster31/bamtools.git'
        name = "bamtools"
        build_dir = os.path.join(self.ext_build, name)
        fn = os.path.basename(url)
        fn_noex = fn.replace(".git", "")
        build_sub_dir = os.path.join(build_dir, fn_noex)
        local_dir = os.path.join(self.install_dir, name)
        return BuildPaths(url, build_dir, build_sub_dir, local_dir)

    def __cppitertools(self):
        url = 'https://github.com/ryanhaining/cppitertools.git'
        local_dir = os.path.join(self.install_dir, "cppitertools")
        return BuildPaths(url, '', '', local_dir)

    def __cppprogutils(self):
        url = 'https://github.com/bailey-lab/cppprogutils.git'
        local_dir = os.path.join(self.install_dir, "cppprogutils")
        return BuildPaths(url, '', '', local_dir)

    def __pear(self):
        url = "http://sco.h-its.org/exelixis/web/software/pear/files/pear-0.9.4-src.tar.gz"
        return self.__package_dirs(url, "pear")

    def __Rdevel(self):
        #url = "ftp://ftp.stat.math.ethz.ch/Software/R/R-devel.tar.gz"
        url = "http://cran.r-project.org/src/base/R-3/R-3.1.1.tar.gz"
        return self.__package_dirs(url, "R-devel")

    def __boost(self):
        url = "http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz"
        return self.__package_dirs(url, "boost")

    def __armadillo(self):
        url = "http://freefr.dl.sourceforge.net/project/arma/armadillo-4.000.2.tar.gz"
        return self.__package_dirs(url, "armadillo")

    def __mlpack(self):
        url = "http://www.mlpack.org/files/mlpack-1.0.8.tar.gz"
        return self.__package_dirs(url, "mlpack")

    def __liblinear(self):
        url = "http://www.csie.ntu.edu.tw/~cjlin/liblinear/liblinear-1.94.tar.gz"
        return self.__package_dirs(url, "liblinear")

    def __cppcms(self):
        url = "http://freefr.dl.sourceforge.net/project/cppcms/cppcms/1.0.4/cppcms-1.0.4.tar.bz2"
        return self.__package_dirs(url, "cppcms")

    def __mathgl(self):
        url = "http://freefr.dl.sourceforge.net/project/mathgl/mathgl/mathgl%202.2.1/mathgl-2.2.1.tar.gz"
        return self.__package_dirs(url, "mathgl")
    
    def __bibcpp(self):
        url = "https://github.com/bailey-lab/bibcpp.git"
        name = "bibcpp"
        build_dir = os.path.join(self.ext_build, name)
        fn = os.path.basename(url)
        fn_noex = fn.replace(".git", "")
        build_sub_dir = os.path.join(build_dir, fn_noex)
        local_dir = os.path.join(self.install_dir, name)
        return BuildPaths(url, build_dir, build_sub_dir, local_dir)

    def __package_dirs(self, url, name):
        build_dir = os.path.join(self.ext_build, name)
        fn = os.path.basename(url)
        fn_noex = fn.replace(".tar.gz", "").replace(".tar.bz2", "").replace(".git", "")
        build_sub_dir = os.path.join(build_dir, fn_noex)
        local_dir = os.path.join(self.install_dir, name)
        return BuildPaths(url, build_dir, build_sub_dir, local_dir)

class Setup:
    def __init__(self, args):
        self.extDirLoc = ""
        if not args.compfile:
            self.extDirLoc = os.path.abspath(os.path.join(os.path.dirname(__file__), "external"))
        else:
            self.extDirLoc = os.path.abspath(self.parseForExtPath(args.compfile[0]))
        self.paths = Paths(self.extDirLoc)
        self.args = args
        self.setUps = {}
        self.setUpNeeded = []
        self.installed = []
        self.failedInstall = []
        self.CC = ""
        self.CXX = ""
        self.externalLoc = ""
        self.bibCppSetUps = ["zi_lib", "cppitertools", "cppprogutils",  "boost", "R-devel", "bamtools", "pear"]
        self.allSetUps = self.bibCppSetUps + ["cppcms", "mathgl", "armadillo", "mlpack", "liblinear", "bibcpp"]
        self.__initSetUps()
        self.__processArgs()

    def __initSetUps(self):
        self.setUps = {"zi_lib": self.zi_lib,
                       "boost": self.boost,
                       "cppitertools": self.cppitertools,
                       "cppprogutils": self.cppprogutils,
                       "R-devel": self.Rdevel,
                       "bamtools": self.bamtools,
                       "cppcms": self.cppcms,
                       "mathgl": self.mathgl,
                       "armadillo": self.armadillo,
                       "mlpack": self.mlpack,
                       "liblinear": self.liblinear,
                       "pear": self.pear,
                       "bibcpp": self.bibcpp
                       
                       }
    def __processArgs(self):
        dirs = self.args.dirsToDelete
        if dirs:
            if "all" == dirs[0]:
                dirs = []
                for k, _ in self.paths.paths.iteritems():
                    dirs.append(k)
            for e in dirs:
                if e in self.paths.paths.keys():
                    p = self.__path(e)
                    if p.build_dir:
                        Utils.rm_rf(p.build_dir)
                    if p.local_dir:
                        Utils.rm_rf(p.local_dir)

        if self.args.compfile:
            self.parseSetUpNeeded(self.args.compfile[0])
            self.parserForCompilers(self.args.compfile[0])
    
    def parseForExtPath(self, compfile):
        compfile = open(compfile)
        for line in compfile:
            values = line.split("=")
            firstArg = values[0].strip()
            if (firstArg == "EXT_PATH"):
               pathPreProcess = values[1].strip()
               pathPreProcess = pathPreProcess.replace("$(realpath", "")
               pathPreProcess = pathPreProcess.replace(")", "")
               pathPreProcess = pathPreProcess.strip()
        return pathPreProcess
            
               
    
    def parseSetUpNeeded(self, compfile):
        compfile = open(compfile)
        for line in compfile:
            values = line.split("=")
            firstArg = values[0].strip()
            if (firstArg.find("USE_") != -1 and line.find("1") != -1):
               self.setUpNeeded.append(firstArg)

    def parserForCompilers(self, compfile):
        compfile = open(compfile)
        for line in compfile:
            values = line.split("=")
            if(values[0].strip()=="CC"):
                self.CC = values[1].strip()
            elif(values[0].strip() == "CXX"):
                self.CXX = values[1].strip()
                if "clang" in self.CXX:
                    self.args.clang = True
    def __path(self, name):
        return self.paths.path(name)

    def __setup(self, name, builder_f):
        if os.path.exists(self.__path(name).local_dir):
            print name, "\033[1;32mfound\033[0m"
        else:
            print name, "\033[1;31mNOT found; building...\033[0m"
            try:
                builder_f()
                self.installed = self.installed + [name]
            except:
                print "failed to install " + name
                self.failedInstall = self.failedInstall + [name]

    def setup(self):
        dirs = self.args.dirsToDelete
        libsToInstall = []
        if not dirs:
            if not self.args.compfile:
                print "Need to supply compfile to parse for needed libraries and compilers"
                print "by giving -compfile"
                exit(1)

            for libNeeded in self.setUpNeeded:
                if(libNeeded == "USE_R"):
                    libsToInstall.append( "R-devel")
                else:
                    # "USE_SOMETHING" becomes "something"
                    libsToInstall.append(libNeeded[4:].lower())
        else:
            libsToInstall = dirs
            if not self.args.CC:
                print "Need to supply C compiler by giving -CC"
                exit(1)
            if not self.args.CXX:
                print "Need to supply C++ compiler by giving -CXX"
                exit(1)
            self.CC = self.args.CC[0]
            self.CXX = self.args.CXX[0]

        for lib in libsToInstall:
            if not lib in self.setUps.keys():
                print "Unrecognized option " + lib
            else:
                self.__setup(lib, self.setUps[lib])
        for inst in self.installed:
            print "\033[1;30m" + inst + " \033[0m", "\033[1;32minstalled\033[0m"
            #"\033[1;" + colorCode + "m" + title + "\033[0m"
        for fail in self.failedInstall:
            print "\033[1;30m" + fail + " \033[0m", "\033[1;31mfailed install\033[0m"



    def num_cores(self):
        c = Utils.num_cores()
        if c > 8:
            return c/2
        if 1 == c:
            return 1
        return c - 1

    def __build(self, i, cmd):
        print "\t getting file..."
        fnp = Utils.get_file_if_size_diff(i.url, self.paths.ext_tars)
        Utils.clear_dir(i.build_dir)

        Utils.untar(fnp, i.build_dir)
        try:
            Utils.run_in_dir(cmd, i.build_sub_dir)
        except:
            Utils.rm_rf(i.local_dir)
            sys.exit(1)

    def boost(self):
        i = self.__path("boost")
        if self.args.clang:
             if isMac():
                cmd = """
                wget https://github.com/boostorg/atomic/commit/6bb71fdd.diff && wget https://github.com/boostorg/atomic/commit/e4bde20f.diff&&  wget https://gist.githubusercontent.com/philacs/375303205d5f8918e700/raw/d6ded52c3a927b6558984d22efe0a5cf9e59cd8c/0005-Boost.S11n-include-missing-algorithm.patch&&  patch -p2 -i 6bb71fdd.diff&&  patch -p2 -i e4bde20f.diff&&  patch -p1 -i 0005-Boost.S11n-include-missing-algorithm.patch&&  echo "using clang;  " >> tools/build/v2/user-config.jam&&  ./bootstrap.sh --with-toolset=clang --prefix={local_dir}&&  ./b2  -d 2 toolset=clang cxxflags=\"-stdlib=libc++\" linkflags=\"-stdlib=libc++\"  -j {num_cores} install &&  install_name_tool -change libboost_system.dylib {local_dir}/lib/libboost_system.dylib {local_dir}/lib/libboost_thread.dylib&&  install_name_tool -change libboost_system.dylib {local_dir}/lib/libboost_system.dylib {local_dir}/lib/libboost_filesystem.dylib""".format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores())
             else:
                cmd = """wget https://github.com/boostorg/atomic/commit/6bb71fdd.diff && wget https://github.com/boostorg/atomic/commit/e4bde20f.diff&&  wget https://gist.githubusercontent.com/philacs/375303205d5f8918e700/raw/d6ded52c3a927b6558984d22efe0a5cf9e59cd8c/0005-Boost.S11n-include-missing-algorithm.patch&&  patch -p2 -i 6bb71fdd.diff&&  patch -p2 -i e4bde20f.diff&&  patch -p1 -i 0005-Boost.S11n-include-missing-algorithm.patch&&  echo "using clang;  " >> tools/build/v2/user-config.jam&&  ./bootstrap.sh --with-toolset=clang --prefix={local_dir}&&  ./b2  -d 2 toolset=clang -j {num_cores} install""".format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores())
        else:
            if isMac():
                cmd = """echo "using gcc : 4.8 : g++-4.8 ; " >> tools/build/v2/user-config.jam &&./bootstrap.sh --prefix={local_dir} && ./b2 -d 2 toolset=darwin-4.8 -j {num_cores} install && install_name_tool -change libboost_system.dylib {local_dir}/lib/libboost_system.dylib {local_dir}/lib/libboost_thread.dylib&&  install_name_tool -change libboost_system.dylib {local_dir}/lib/libboost_system.dylib {local_dir}/lib/libboost_filesystem.dylib""".format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores())
            else:
                cmd = """echo "using gcc : 4.8 : g++-4.8 ; " >> tools/build/v2/user-config.jam &&./bootstrap.sh --prefix={local_dir} && ./b2 -d 2 toolset=gcc-4.8 -j {num_cores} install""".format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores())
        print cmd
        self.__build(i, cmd)

    def pear(self):
        i = self.__path("pear")
        cmd = """
            echo $(pwd) && ./configure --prefix={local_dir}/../../../ && make -j {num_cores} && make install""".format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores())
        self.__build(i, cmd)

    def Rdevel(self):
        i = self.__path("R-devel")
        if isMac():
            cmd = """
                ./configure --prefix={local_dir} --enable-R-shlib --with-x=no CC={CC} CXX={CXX} OBJC={CC}
                && make -j {num_cores}
                && make install
                && echo 'install.packages(c(\"gridExtra\", \"ape\", \"ggplot2\", \"seqinr\",\"Rcpp\", \"RInside\", \"devtools\"), repos=\"http://cran.us.r-project.org\")' | $({local_dir}/R.framework/Resources/bin/R RHOME)/bin/R --slave --vanilla
                """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        else:
            cmd = """
                ./configure --prefix={local_dir} --enable-R-shlib --with-x=no CC={CC} CXX={CXX} OBJC={CC}
                && make -j {num_cores}
                && make install
                && echo 'install.packages(c(\"gridExtra\", \"ape\", \"ggplot2\", \"seqinr\",\"Rcpp\", \"RInside\",\"devtools\"), repos=\"http://cran.us.r-project.org\")' | $({local_dir}/lib/R/bin/R RHOME)/bin/R --slave --vanilla
            """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        cmd = " ".join(cmd.split())

        self.__build(i, cmd)
    
    def installRPackageSource(self, sourceFile):
        i = self.__path("R-devel")
        for pack in sourceFile.split(","):
            if isMac():
                cmd = """echo 'install.packages(\"{SOURCEFILE}\", repos = NULL, type="source")' | $({local_dir}/R.framework/Resources/bin/R RHOME)/bin/R --slave --vanilla
                """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '),SOURCEFILE = pack )
            else:
                cmd = """echo 'install.packages(\"{SOURCEFILE}\", repos = NULL, type="source")' | $({local_dir}/lib/R/bin/R RHOME)/bin/R --slave --vanilla
                """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '),SOURCEFILE = pack )
            print CT.boldBlack(cmd)
            Utils.run(cmd)
    
    def installRPackageName(self, packageName):
        i = self.__path("R-devel")
        for pack in packageName.split(","):
            if isMac():
                cmd = """echo 'install.packages(c(\"{PACKAGENAME}\"), repos=\"http://cran.us.r-project.org\")' | $({local_dir}/R.framework/Resources/bin/R RHOME)/bin/R --slave --vanilla
                """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '), PACKAGENAME = pack )
            else:
                cmd = """echo 'install.packages(\"{PACKAGENAME}\", repos=\"http://cran.us.r-project.org\")'  | $({local_dir}/lib/R/bin/R RHOME)/bin/R --slave --vanilla
                """.format(local_dir=shellquote(i.local_dir).replace(' ', '\ '),PACKAGENAME = pack )
            print CT.boldBlack(cmd)
            Utils.run(cmd)
    
    def bamtools(self):
        i = self.__path('bamtools')
        cmd = "git clone {url} {d}".format(url=i.url, d=i.build_dir)
        Utils.run(cmd)
        i = self.__path('bamtools')
        cmd = "mkdir -p build && cd build && CC={CC} CXX={CXX} cmake -DCMAKE_INSTALL_PREFIX:PATH={local_dir} .. && make -j {num_cores} install".format(
            local_dir=shellquote(i.local_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        Utils.run_in_dir(cmd, i.build_dir)
    
    
    def bibcpp(self):
        i = self.__path('bibcpp')
        cmd = "git clone {url} {d}".format(url=i.url, d=i.build_dir)
        Utils.run(cmd)
        secondCmd = """./setUpScripts/generateCompFile.py -outFilename compfile.mk -externalLoc {external} -CC {CC} -CXX {CXX} -outname seqTools -installName bibcpp -prefix {localTop} -neededLibs zi_lib,cppitertools,cppprogutils,boost,R,bamtools,pear,curl""".format(localTop=shellquote(self.paths.install_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX, external=self.extDirLoc) 
        #print secondCmd
        Utils.run_in_dir(secondCmd, i.build_dir)
        secondCmd = """./setup.py -compfile compfile.mk""".format(localTop=shellquote(self.paths.install_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX, external=self.extDirLoc) 
        #print secondCmd
        Utils.run_in_dir(secondCmd, i.build_dir)
        secondCmd = """make COMPFILE=compfile.mk -j {num_cores}""".format(localTop=shellquote(self.paths.install_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX, external=self.extDirLoc) 
        #print secondCmd
        Utils.run_in_dir(secondCmd, i.build_dir)
        secondCmd = """make COMPFILE=compfile.mk install""".format(localTop=shellquote(self.paths.install_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX, external=self.extDirLoc) 
        #print secondCmd
        Utils.run_in_dir(secondCmd, i.build_dir)
        i = self.__path('bibcpp')
        
        Utils.run_in_dir(secondCmd, i.build_dir)

    def cppcms(self):
        i = self.__path('cppcms')
        cmd = "mkdir -p build && cd build && CC={CC} CXX={CXX} cmake -DCMAKE_INSTALL_PREFIX:PATH={local_dir} .. && make -j {num_cores} install".format(local_dir=shellquote(i.local_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        if(sys.platform == "darwin"):
            cmd += " && install_name_tool -change libbooster.0.dylib {local_dir}/lib/libbooster.0.dylib {local_dir}/lib/libcppcms.1.dylib".format(local_dir=shellquote(i.local_dir), num_cores=self.num_cores())
        self.__build(i, cmd)

    def armadillo(self):
        i = self.__path('armadillo')
        cmd = "mkdir -p build && cd build && CC={CC} CXX={CXX} cmake -DCMAKE_INSTALL_PREFIX:PATH={local_dir} .. && make -j {num_cores} install".format(
            local_dir=shellquote(i.local_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        self.__build(i, cmd)

    def liblinear(self):
        i = self.__path('liblinear')
        cmd = "make && mkdir -p {local_dir} && cp predict train {local_dir}".format(
            local_dir=shellquote(i.local_dir))
        self.__build(i, cmd)

    def mlpack(self):
        i = self.__path('mlpack')
        armadillo_dir = shellquote(i.local_dir).replace("mlpack", "armadillo")
        boost_dir = shellquote(i.local_dir).replace("mlpack", "boost")
        cmd = """
mkdir -p build
&& cd build
&& CC={CC} CXX={CXX} cmake -D DEBUG=OFF -D PROFILE=OFF
         -D ARMADILLO_LIBRARY={armadillo_dir}/lib/libarmadillo.so.4.0.2
         -D ARMADILLO_INCLUDE_DIR={armadillo_dir}/include/
         -D CMAKE_INSTALL_PREFIX:PATH={local_dir} ..
         -DBoost_NO_SYSTEM_PATHS=TRUE -DBOOST_INCLUDEDIR={boost}/include/ -DBOOST_LIBRARYDIR={boost}/lib/
&& make -j {num_cores} install
""".format(local_dir=shellquote(i.local_dir),
           armadillo_dir=armadillo_dir,
           num_cores=self.num_cores(),
           boost=boost_dir, CC=self.CC, CXX=self.CXX)
        cmd = " ".join(cmd.split('\n'))
        self.__build(i, cmd)

    def mathgl(self):
        i = self.__path('mathgl')
        cmd = "mkdir -p build && cd build && CC={CC} CXX={CXX} cmake -DCMAKE_INSTALL_PREFIX:PATH={local_dir} .. && make -j {num_cores} install".format(
            local_dir=shellquote(i.local_dir), num_cores=self.num_cores(), CC=self.CC, CXX=self.CXX)
        self.__build(i, cmd)

    def __git(self, i):
        cmd = "git clone {url} {d}".format(url=i.url, d=shellquote(i.local_dir))
        Utils.run(cmd)

    def zi_lib(self):
        self.__git(self.__path('zi_lib'))

    def cppitertools(self):
        self.__git(self.__path('cppitertools'))
        i = self.__path('cppitertools')
        cmd = "cd {d} && git checkout d4f79321842dd584f799a7d51d3e066a2cdb7cac".format(d=shellquote(i.local_dir))
        Utils.run(cmd)

    def cppprogutils(self):
        self.__git(self.__path('cppprogutils'))

    def ubuntu(self):
        pkgs = """libbz2-dev python2.7-dev cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev
python doxygen doxygen-gui auctex xindy graphviz libcurl4-openssl-dev""".split()



def startSrc():
    if not os.path.isdir("src/"):
        os.mkdir("src/")
    if not os.path.isfile("src/main.cpp"):
        cmd = "./scripts/genHelloWorldCpp.sh src/main.cpp"
        Utils.run(cmd)

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-instRPackageName',type=str, nargs=1)
    parser.add_argument('-instRPackageSource',type=str, nargs=1)
    parser.add_argument('-compfile', type=str, nargs=1)
    parser.add_argument('dirsToDelete', type=str, nargs='*')
    parser.add_argument('-CC', type=str, nargs=1)
    parser.add_argument('-CXX', type=str, nargs=1)
    parser.add_argument('-libs', dest = 'print_libs', action = 'store_true' )
    parser.add_argument('-addBashCompletion', dest = 'addBashCompletion', action = 'store_true' )
    parser.add_argument('-clang', dest = 'clang', action = 'store_true' )
    parser.add_argument('-generateSrc', dest = 'generateSrc', action = 'store_true' )
    return parser.parse_args()


def main():
    args = parse_args()
    if(args.instRPackageName):
        s = Setup(args)
        s.installRPackageName(args.instRPackageName[0])
        return (0)
    if(args.instRPackageSource):
        s = Setup(args)
        s.installRPackageSource(args.instRPackageSource[0])
        return (0)
    if(args.generateSrc):
        startSrc()
        return (0)

    s = Setup(args)
    if args.print_libs:
        print "Available installs:"
        count = 1
        installs = s.allSetUps
        installs.sort()
        for set in installs:
            print count , ")" , set
            count = count + 1
    elif args.addBashCompletion:
        if(os.path.isdir("./bashCompletes")):
            cmd = "cat bashCompletes/* >> ~/.bash_completion"
            Utils.run(cmd)
    else:
        s.setup()
main()
