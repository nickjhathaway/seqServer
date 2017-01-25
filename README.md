seqServer
================
Version 1.5.0

several content managers for serving up the results procuded by sequenceTools 

#Installing


requires clang-3.8 or gcc-5 and cmake (required by dependencies)

```bash
git clone https://github.com/nickjhathaway/seqServer.git
cd seqServer 
#determines compilier
./configure.py
#this will download most of the dependencies 
./setup.py --compfile compfile.mk --outMakefile makefile-common.mk
make 
```
