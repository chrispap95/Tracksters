import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing ('python')

options.register('delta',
                 '1p5', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of delta.")

options.register('cmssw',
                 'CMSSW_11_3_4', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of CMSSW.")

options.register('geometry',
                 'D76', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of geometry.")

options.register('campaign',
                 'camp5', # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string,         # string, int, or float
                 "Value of campaign.")

options.parseArguments()

tags = {
    "camp5": {
        "1p5":"210917_164144",
        "2":  "210917_164210",
        "2p5":"210917_164241",
        "3":  "210917_164306",
        "3p5":"210917_164333",
        "4":  "210917_164402",
        "4p5":"210917_164445",
    },
    "clue3D": {
        "1p5":"211007_214742",
        "2":  "211007_214757",
        "2p5":"211007_214812",
        "3":  "211007_214827",
        "3p5":"211007_214843",
        "4":  "211007_214857",
        "4p5":"211007_214913",
    },
}

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )

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

process.ana = cms.EDAnalyzer('Centrality')

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('output/hist_clue3D_'+options.delta+'.root')
)

process.p = cms.Path(process.ana)
