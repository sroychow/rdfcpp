from variables import *
from selections import *
from systematics import *
from sampleParser import *
import re
import copy
import json

myselections = {}
for cut in ['Signal', 'Sideband', 'Dimuon']:
    if cut=='Dimuon':
        myselections['%s' % cut] = copy.deepcopy(selections['%s' % cut])
        continue

    myselections['%sPlus' % cut]  = copy.deepcopy(selections['%s' % cut])
    myselections['%sMinus' % cut] = copy.deepcopy(selections['%s' % cut])
    for d in ['MC','DATA']:
        myselections['%sPlus' % cut][d]['cut']    += ' && Muon_charge[Idx_mu1]>0'
        myselections['%sMinus' % cut][d]['cut']   += ' && Muon_charge[Idx_mu1]<0'

#print myselections
with open('selections.json', 'w') as fp:
    json.dump(myselections, fp, indent=4)

with open('systematics.json', 'w') as sp:
    json.dump(systematics, sp,  indent=4)

    
weightColumns={}

for dataType in ['MC', 'DATA']:
    weightColumns[dataType]={}
    if dataType == 'DATA':
        weightColumns[dataType]["totalWeight_Data"]= "1"
        continue
    #Define the columns for weights
    for wtname, weight in histoweights[dataType].iteritems():
        weightColumns[dataType][wtname] = 'lumiweight*{}'.format(weight)
        if 'WeightVariations' not in systematics.keys():   continue
        ##now loop over variations of this weight
        for nomwt, nomwtvars in systematics['WeightVariations'].iteritems():
            ##what if the weight whose variations we seek are not present in original weight
            if nomwt not in weight: continue
            for wtvar in nomwtvars:
                ##Special treatment of LHEWeights
                if re.search("[0-9]", wtvar):
                    variation = wtvar[:re.search("[0-9]", wtvar).start()]
                    index = wtvar[re.search("[0-9]", wtvar).start():]
                    nw = '{}[{}]'.format(nomwt + "*" + variation,index)
                    newWeight = weight.replace(nomwt,nw)
                else:
                    newWeight = weight.replace(nomwt, wtvar)
                weightColumns[dataType][wtname+ '_{}'.format(wtvar)] = 'lumiweight*{}'.format(newWeight)   

with open('weightColumns.json', 'w') as fp:
    json.dump(weightColumns, fp, indent=4)
    
variablesColumns={}
#Loop over variables now and create branches for UpDown variations
for variables, vardict in variables.iteritems(): 
    collectionToRead = vardict['inputCollection']
    collectionToDefine = collectionToRead
    if vardict.has_key('newCollection') and vardict['newCollection'] != '':
        collectionToDefine = vardict['newCollection']  

    variablesColumns[collectionToDefine] = {}
    variablesColumns[collectionToDefine]['appliesTo'] = vardict['appliesTo']
    variablesColumns[collectionToDefine]['D1variables'] = {}
    if 'index' in vardict:
        idx = vardict['index']
        for v, defn in vardict['variables'].iteritems():
           variablesColumns[collectionToDefine]['D1variables'][collectionToDefine + '_{}'.format(v)] =  (collectionToRead + '_{vec}[{i}]'.format(vec=v,i=idx), defn[0], defn[1], defn[2], defn[3])
           if not systematics.has_key('ColumnVariations') : continue
           for nom, variations in systematics['ColumnVariations'].iteritems():
               if not nom in v: continue
               for var in variations:
                   newV = v.replace(nom,var)
                   variablesColumns[collectionToDefine]['D1variables'][collectionToDefine + '_{}'.format(newV)] =  (collectionToRead + '_{vec}[{i}]'.format(vec=newV,i=idx), defn[0], defn[1], defn[2], defn[3])
    else:
        for v,defn in vardict['variables'].iteritems():
           variablesColumns[collectionToDefine]['D1variables'][collectionToDefine + '_{}'.format(v)] =  (collectionToRead + '_{}'.format(v), defn[0], defn[1], defn[2], defn[3])
           if not systematics.has_key('ColumnVariations') : continue
           for nom, variations in systematics['ColumnVariations'].iteritems():
               if not nom in v: continue
               for var in variations:
                   newV = v.replace(nom,var)
                   variablesColumns[collectionToDefine]['D1variables'][collectionToDefine + '_{}'.format(newV)] =  (collectionToRead + '_{vec}[{i}]'.format(vec=newV,i=idx), defn[0], defn[1], defn[2], defn[3])

    #Trying to define new collections from exisiting collections
    if vardict.has_key('newvariables'):
        variablesColumns[collectionToDefine]['D1newvariables'] = {}
        for newvar, defn in vardict['newvariables'].iteritems():
	    # 4th entries in tuple is the string that defines the new variable
            cDefinition = defn[4].replace(collectionToRead, collectionToDefine)
            variablesColumns[collectionToDefine]['D1newvariables'][collectionToDefine + '_{}'.format(newvar)] = tuple((cDefinition, defn[0], defn[1], defn[2], defn[3]))
            if not systematics.has_key('ColumnVariations') : continue
            for nom, variations in systematics['ColumnVariations'].iteritems():
                if not nom in newvar: continue
                for var in variations:
                    newV = newvar.replace(nom,var)
                    variablesColumns[collectionToDefine]['D1newvariables'][collectionToDefine + '_{}'.format(newV)] =  (cDefinition.replace(nom, var), defn[0], defn[1], defn[2], defn[3])
    if vardict.has_key('D2variables'): 
        variablesColumns[collectionToDefine]['D2variables'] = {}
        for var,tools in vardict['D2variables'].iteritems():
                xVar = collectionToDefine + '_' + tools[7]
                yVar = collectionToDefine + '_' + tools[8]
                variablesColumns[collectionToDefine]['D2variables'][var] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], xVar, yVar))

    if vardict.has_key('D3variables'): 
        variablesColumns[collectionToDefine]['D3variables'] = {}
        for var,tools in vardict['D3variables'].iteritems():
                xVar = collectionToDefine + '_' + tools[10]
                yVar = collectionToDefine + '_' + tools[11]
                zVar = collectionToDefine + '_' + tools[12]
                variablesColumns[collectionToDefine]['D3variables'][var] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], tools[7], tools[8], tools[9], xVar, yVar, zVar))

'''
##now produce column variations
systColumns={}
for col, defn in variablesColumns.iteritems():
    if not systematics.has_key('ColumnVariations') :  break
    for nom, variations in systematics['ColumnVariations'].iteritems():
      #check if the key(e.g, corrected) is part of newVariable
      #(e.g, correctedPt*RelIso) 
     if not nom in col: continue
     for v in variations:
         systColumns[col.replace(nom,v)] = defn.replace(nom,v)

variablesColumns.update(systColumns)
'''                        
with open('variablesColumns.json', 'w') as vp:
    json.dump(variablesColumns, vp,  indent=4)                    


#sample info               
parser = sampleParser()
samples_dict = parser.getSampleDict()
with open('samples.json', 'w') as vp:
    json.dump(samples_dict, vp,  indent=4)                    
