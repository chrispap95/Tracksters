#
# This script runs the module TrackstersTestCLUE3D.
#

import FWCore.ParameterSet.Config as cms
from campaigns import tags

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5) )

delta = '1p5'
campaign = 'clue3D'
cmssw = 'CMSSW_12_0_2'

directory = "/store/user/chpapage/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "/CloseByDoubleGamma_E25Eta1p88Delta"+delta+"_"+cmssw+"_upgrade2026_D76_"+\
            campaign+"_step3/"+tags[campaign][delta]+"/0000/"

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        directory+"step3_1.root",
    ),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

process.ana = cms.EDAnalyzer('TrackstersTestCLUE3D')

process.p = cms.Path(process.ana)
