from variables import *
from selections import *
from systematics import *
import re
import copy
import json

def columnFromIndex(collectionToRead, collectionToDefine, idx, variables_, syst={}, varcolumns={}):
    varcolumns['nominal'] = {}
    for nomE in syst.keys():
        varcolumns[nomE] = {}
    for v, defn in variables_.iteritems():
        #print v
        varcolumns['nominal'][collectionToDefine + '_{}'.format(v)] = (collectionToRead + '_{vec}[{i}]'.format(vec=v,i=idx), defn[0], defn[1], defn[2], defn[3])
        for nomE, variations in syst.iteritems():
            if nomE in v:
                #print nomE, " for variable ", v
                for ve in variations:
                    systVar = v.replace(nomE, ve)
                    varcolumns[nomE][collectionToDefine + '_{}'.format(systVar)] = (collectionToRead + '_{vec}[{i}]'.format(vec=systVar,i=idx), defn[0], defn[1], defn[2], defn[3])

def columnFromCollection(collectionToRead, collectionToDefine, variables_, syst={}, varcolumns={}):
    varcolumns['nominal'] = {}
    for nomE in syst.keys():
        varcolumns[nomE] = {}
    for v, defn in variables_.iteritems():
        #print v
        varcolumns['nominal'][collectionToDefine + '_{}'.format(v)] = (collectionToRead + '_{vec}'.format(vec=v), defn[0], defn[1], defn[2], defn[3])
        for nomE, variations in syst.iteritems():
            if nomE in v:
                #print nomE, " for variable ", v
                for ve in variations:
                    systVar = v.replace(nomE, ve)
                    varcolumns[nomE][collectionToDefine + '_{}'.format(systVar)] = (collectionToRead + '_{vec}'.format(vec=systVar), defn[0], defn[1], defn[2], defn[3])



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
        weightColumns[dataType]["totalWeight_Data"]= {"nominal" : "1"}
        continue
    #Define the columns for weights
    for wtname, weight in histoweights[dataType].iteritems():
        weightColumns[dataType][wtname] = {}
        weightColumns[dataType][wtname]['nominal'] = 'lumiweight*{}'.format(weight)

        if 'WeightVariations' not in systematics.keys():   continue
        
        ##now loop over variations of this weight
        for nomwt, nomwtvars in systematics['WeightVariations'].iteritems():
            weightColumns[dataType][wtname][nomwt] = {}
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
                weightColumns[dataType][wtname][nomwt][wtname+ '_{}'.format(wtvar)] = 'lumiweight*{}'.format(newWeight)   

with open('weightColumns.json', 'w') as fp:
    json.dump(weightColumns, fp, indent=4)
    
variablesColumns={}
#Loop over variables now and create branches for UpDown variations
for variables, vardict in variables.iteritems(): 
    collectionToRead = vardict['inputCollection']
    collectionToDefine = collectionToRead
    if vardict.has_key('newCollection') and vardict['newCollection'] != '':
        collectionToDefine = vardict['newCollection']  
    print collectionToDefine
    variablesColumns[collectionToDefine]={}
    variablesColumns[collectionToDefine]['appliesTo'] = vardict['appliesTo'] 
    print  vardict['appliesTo'] 
    variablesColumns[collectionToDefine]['D1variables'] = {}
    syst={}
    if systematics.has_key('ColumnVariations') : 
        syst = systematics['ColumnVariations']

    #print syst
    if 'index' in vardict:
        columnFromIndex(collectionToRead, collectionToDefine, vardict['index'], vardict['variables'], syst, variablesColumns[collectionToDefine]['D1variables'])
    else:
        columnFromCollection(collectionToRead, collectionToDefine, vardict['variables'], syst, variablesColumns[collectionToDefine]['D1variables'])

        
    if vardict.has_key('newvariables'):
      variablesColumns[collectionToDefine]['D1newvariables'] = {}
      variablesColumns[collectionToDefine]['D1newvariables']['nominal'] = {}
      for nomE in syst.keys():
          variablesColumns[collectionToDefine]['D1newvariables'][nomE] = {}

      for newvar, defn in vardict['newvariables'].iteritems():
          # 4th entries in tuple is the string that defines the new variable
          cDefinition = defn[4]
          variablesColumns[collectionToDefine]['D1newvariables']['nominal'][collectionToDefine + '_{}'.format(newvar)] = (cDefinition, defn[0], defn[1], defn[2], defn[3])
          for nomE, variations in syst.iteritems():
              if nomE not in cDefinition: continue
              #print nomE, " for variable ", v
              for ve in variations:
                  systVar = newvar.replace(nomE, ve)
                  ndefn = cDefinition.replace(nomE, ve)
                  variablesColumns[collectionToDefine]['D1newvariables'][nomE][collectionToDefine + '_{}'.format(systVar)] = (ndefn, defn[0], defn[1], defn[2], defn[3])

    #Trying to define new collections from exisiting collections
    if vardict.has_key('D2variables'): 
        variablesColumns[collectionToDefine]['D2variables']= {}
        variablesColumns[collectionToDefine]['D2variables']['nominal']= {}
        for nomE in syst.keys():
           variablesColumns[collectionToDefine]['D2variables'][nomE] = {}        
        for var,tools in vardict['D2variables'].iteritems():
            xVar = collectionToDefine + '_' + tools[7]
            yVar = collectionToDefine + '_' + tools[8]
            variablesColumns[collectionToDefine]['D2variables']['nominal'][var] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], xVar, yVar))
            for nomE, variations in syst.iteritems():
                if nomE in xVar:
                    for ve in variations:
                        systVar = xVar.replace(nomE, ve)
                        variablesColumns[collectionToDefine]['D2variables'][nomE][collectionToDefine + '_X_{}'.format(systVar)] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], systVar, yVar))
                if nomE in yVar:
                    for ve in variations:
                        systVar = yVar.replace(nomE, ve)
                        variablesColumns[collectionToDefine]['D2variables'][nomE][collectionToDefine + '_Y_{}'.format(systVar)] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], xVar, systVar))




    if vardict.has_key('D3variables'): 
        variablesColumns[collectionToDefine]['D3variables'] = {} 
        variablesColumns[collectionToDefine]['D3variables']['nominal'] = {} 
        for var,tools in vardict['D3variables'].iteritems():
                xVar = collectionToDefine + '_' + tools[10]
                yVar = collectionToDefine + '_' + tools[11]
                zVar = collectionToDefine + '_' + tools[12]
                variablesColumns[collectionToDefine]['D3variables']['nominal'][var] = tuple((tools[0], tools[1], tools[2], tools[3], tools[4], tools[5], tools[6], tools[7], tools[8], tools[9], xVar, yVar, zVar))


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
            
with open('variablesColumns.json', 'w') as vp:
    json.dump(variablesColumns, vp,  indent=4)                    
               
