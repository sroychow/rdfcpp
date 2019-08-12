import os

class sampleParser:
    
    def __init__(self, tag='V1', dataYear = '2016', restrict=[], exclude =[] ):

        self.tag = tag
        self.dataYear = dataYear
        self.inputDir = ('/eos/user/r/roy4/data/NanoAOD%s-%s/' % (str(self.dataYear), self.tag))
        self.restrict = restrict
        self.exclude = exclude
        self.samples_dict = {}

    def parse(self):

        production_file = open('/eos/user/r/roy4/data/mcsamples_2016-V1.txt', 'r')
        print "Input Directory=", self.inputDir
        print "Name of production file=", production_file
        
        production_content = [x.strip() for x in production_file.readlines()]

        # available files
        samples = os.listdir(self.inputDir)

        for sample in samples:    
            
            if '_ext' in sample: sample_stripped = sample[:-5]
            else: sample_stripped = sample        
            xsec = -1
    
            accept = False

            # check if sample is in restrict list
            for r in self.restrict: 
                if r in sample_stripped: accept = True
            accept |= (len(self.restrict)==0)

            # check if sample is in exclude list
            for e in self.exclude:
                if e in sample_stripped: accept = False
            
            if not accept: continue


            multiprocess = False
            files = os.listdir(self.inputDir + '/' + sample)

            # add each Run period separately
            if 'Run' in sample:
                self.samples_dict[sample] = {'dir' : {sample : files}, 'datatype' : 'DATA', 'xsec' : xsec, 'subsel' : {'none' : ''}, 'multiprocessing': multiprocess}
                continue

            found = False
            # match for '_ext' to identify extensions of same process
            for prod in production_content:

                if sample_stripped in prod: 
                    xsec = float(prod.split(',')[-1])
                    found = True
            if not found: print 'sample not found in sample table!', sample_stripped        

                     
            if sample_stripped not in self.samples_dict.keys():

                if 'WW' in sample_stripped or 'WZ' in sample_stripped or 'ZZ' in sample_stripped or 'QCD'in sample_stripped or 'ST'in sample_stripped: multiprocess = True

                self.samples_dict[sample_stripped] = {'dir' : { sample : files}, 'datatype' : 'MC', 'xsec' : xsec,  'subsel' : {'none' : ''} ,'multiprocessing': multiprocess}
                if 'WJets' in sample_stripped:
                    self.samples_dict[sample_stripped]['subsel']['WToMuNu']  = ' && genVtype == 14'
                    #self.samples_dict[sample_stripped]['subsel']['WToETauNu'] = ' && (genVtype == 12 || genVtype == 16)'        
            else:
                self.samples_dict[sample_stripped]['dir'].update({sample : files})

    def getSampleDict(self):

        self.parse()

        return self.samples_dict
