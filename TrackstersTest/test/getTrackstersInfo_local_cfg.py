#
# This script runs the module TrackstersTest.
#

import FWCore.ParameterSet.Config as cms
from campaigns_cff import tags

process = cms.Process("Demo")
process.load('RecoHGCal.Configuration.recoHGCAL_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5) )

# Options below are for CA
#delta = '1p5'
#campaign = 'camp6'
#cmssw = 'CMSSW_12_0_2'
#isCLUE3D = False

# Options below are for CLUE3D
delta = '1p5'
campaign = 'clue3D'
cmssw = 'CMSSW_12_0_2'
isCLUE3D = True

directory = "/store/user/chpapage/CloseByDoubleGamma_E25Eta1p88Delta"+delta+\
            "/CloseByDoubleGamma_E25Eta1p88Delta"+delta+"_"+cmssw+"_upgrade2026_D76_"+\
            campaign+"_step3/"+tags[campaign][delta]+"/0000/"

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:step3_delta1p5_clue3Dv3.root",
    ),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

process.ana = cms.EDAnalyzer('TrackstersPrintout',
    isCLUE3D = cms.bool(isCLUE3D)
)

process.p = cms.Path(process.ana)
