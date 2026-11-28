seqServer
================
Version 3.0.2-dev

Code to help serve sequence analysis results

# Installing


requires a C++17 compiler (defaults to gcc/g++ on Linux, gcc 13 on Ubuntu 24.04, and clang on macOS) and cmake (required by dependencies)

```bash
#--recursive also gets the cppSetupTools submodule (configure.py and setup.py fetch it too if it's missing)
git clone --recursive -b develop https://github.com/nickjhathaway/seqServer.git
cd seqServer
#determines compilier
./configure.py
#this will download most of the dependencies
./setup.py --compfile compfile.mk --outMakefile makefile-common.mk
make
```
