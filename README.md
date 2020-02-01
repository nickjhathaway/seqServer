seqServer
================
Version 3.0.1-dev

Code to help serve sequence analysis results

# Installing


requires clang-4.0 or gcc-7 and cmake (required by dependencies)

```bash
git clone https://github.com/nickjhathaway/seqServer.git
cd seqServer
#determines compilier
./configure.py
#this will download most of the dependencies
./setup.py --compfile compfile.mk --outMakefile makefile-common.mk
make
```
