seqServer
================
Version 1.2

several content managers for serving up the results procuded by sequenceTools 

#Installing


requires clang-3.6 or gcc-4.9  

```bash
git clone https://github.com/nickjhathaway/seqServer.git
cd seqServer 
#determines compilier
./configure.py
#this will download most of the dependencies 
./setup.py --compfile compfile.mk
make 
```
