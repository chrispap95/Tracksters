import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

delta = "1p5"

hashes = {
    "1p5":"210917_164144",
    "2":  "210917_164210",
    "2p5":"210917_164241",
    "3":  "210917_164306",
    "3p5":"210917_164333",
    "4":  "210917_164402",
    "4p5":"210917_164445",
}

directory = "/store/user/chpapage/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "_CMSSW_11_3_4_upgrade2026_D76_camp5_step3/"+hashes[delta]+\
            "/0000/"


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
                                       fileName = cms.string('hist_'+delta+'.root')
                                   )

process.p = cms.Path(process.ana)
