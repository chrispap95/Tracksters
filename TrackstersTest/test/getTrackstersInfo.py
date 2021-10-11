import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

tags = {
    "1p5":"210917_164144",
    "2":  "210917_164210",
    "2p5":"210917_164241",
    "3":  "210917_164306",
    "3p5":"210917_164333",
    "4":  "210917_164402",
    "4p5":"210917_164445",
}

delta = '1p5'

directory = "/store/user/chpapage/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "_CMSSW_11_3_4_upgrade2026_D76_camp5_step3/"+tags[delta]+\
            "/0000/"

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#        'file:/uscms_data/d1/chpapage/HGCalDPG/CMSSW_11_3_0_pre6/src/37034.0_TTbar_14TeV+2026D82+TTbar_14TeV_TuneCP5_GenSimHLBeamSpot14+DigiTrigger+RecoGlobal+HARVESTGlobal/step3.root',
#        'file:/uscms_data/d1/chpapage/HGCalDPG/CMSSW_11_3_0_pre2/src/30693.0/step3.root'
        directory+"step3_1.root",
    ),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

process.ana = cms.EDAnalyzer('TrackstersTest')

process.p = cms.Path(process.ana)
