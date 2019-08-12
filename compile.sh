g++ `root-config --cflags` -std=c++17 -c VariableBlock.cc -o VariableBlock.o
g++ `root-config --cflags` -std=c++17 -c Module.cc -o Module.o 
g++ `root-config --cflags` -g -std=c++17 -c RDFProcessor.cc -o RDFProcessor.o
g++ `root-config --cflags` -g -std=c++17 controlPlots.cc -o controlPlots VariableBlock.o Module.o RDFProcessor.o `root-config --libs`
