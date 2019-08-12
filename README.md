#c++ based framework to do analysis with RDataFrame

source /cvmfs/sft-nightlies.cern.ch/lcg/views/dev3/latest/x86_64-centos7-gcc8-opt/setup.sh

To run:

cd inputdata

Update the sampleParser.py, selections.py, systematics.py, variables.py according to the need
 
python createInputjson.py

cd ../

sh compile.sh

./controlPlots

##Very basic version. Features needs to be added. See open issues.
