import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from campaigns import tags

options = VarParsing.VarParsing ('python')

options.register('input',
                 500, # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,         # string, int, or float
                 "Number of events.")

options.register('delta',
                 '1p5', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of delta.")

options.register('cmssw',
                 'CMSSW_12_0_2', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of CMSSW.")

options.register('geometry',
                 'D76', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of geometry.")

options.register('campaign',
                 'camp6', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of campaign.")

options.register('directed',
                 0, # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.bool,         # string, int, or float
                 "Create directed or undirected graph.")

options.register('weighted',
                 0, # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,         # string, int, or float
                 "Creates weighted graph if not 0. Methods 1 through 3.")

options.parseArguments()

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.input) )

directory = "/store/user/chpapage/CloseByDoubleGamma_E25Eta1p88Delta"+\
            options.delta+"/CloseByDoubleGamma_E25Eta1p88Delta"+options.delta+\
            "_"+options.cmssw+"_upgrade2026_"+options.geometry+"_"+\
            options.campaign+"_step3/"+tags[options.campaign][options.delta]+"/0000/"

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        directory+"step3_1.root",
        directory+"step3_2.root",
        directory+"step3_3.root",
        directory+"step3_4.root",
        directory+"step3_5.root",
        directory+"step3_6.root",
        directory+"step3_7.root",
        directory+"step3_8.root",
        directory+"step3_9.root",
        directory+"step3_10.root",
    ),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

isCLUE3D = False
if options.campaign == 'clue3D':
    isCLUE3D = True

process.ana = cms.EDAnalyzer('Centrality',
    directed = cms.bool(options.directed),
    weightMode = cms.int32(0),
    isCLUE3D = cms.bool(isCLUE3D)
)

# Create output tag for filename
nameTag = 'hist_'
if options.campaign == 'clue3D':
    nameTag += 'CLUE3D_'
else:
    nameTag += 'CA_'
nameTag += options.delta
if options.directed:
    nameTag += "_directed"
if options.weighted:
    nameTag += "_weighted%d"%(options.weighted)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('output/'+nameTag+'.root')
)

process.p = cms.Path(process.ana)
