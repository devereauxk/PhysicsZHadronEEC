// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIdatadIyjleedIPhysicsZHadronEECdITrackingEfficiencydI20240806_ResidualCorrectiondIworkflowdIplot_corrections_C_ACLiC_dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/data/yjlee/PhysicsZHadronEEC/TrackingEfficiency/20240806_ResidualCorrection/workflow/./plot_corrections.C"

// Header files passed via #pragma extra_include

namespace {
  void TriggerDictionaryInitialization_plot_corrections_C_ACLiC_dict_Impl() {
    static const char* headers[] = {
"./plot_corrections.C",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/include",
"/home/data/yjlee/CMSSW_13_2_4/src",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/cms/cmssw/CMSSW_13_2_4/src",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/mctester/1.25.1-41c9f0aaa5f04e120ea64f10e569f259/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/hydjet2/2.4.3-1286f81ca0e127533664ee1045ef4eb4/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/dd4hep/v01-23x-bb01de2ca72c68ae00d8a4dcb2578662/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/herwig7/7.2.2-34c68f049cb8c83c64b7c4d31ddc0578/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/tauolapp/1.1.8-d07a61ef8dc6999ef68f31d436920f68/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/sherpa/2.2.15-4d08c661f99a49ca2e0ff63fd2810c32/include/SHERPA-MC",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/lwtnn/2.13-1ff455278ab3aa9a1eec13451e3b6b86/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/thepeg/2.2.2-60efb49167d3673f799dd70dfad12232/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/pythia8/309-49397a2ef4a88bc21a220707b80259bd/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/starlight/r193-3f37e15dcb870744916703751c7475cc/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/rivet/3.1.8-ec907f0a3a6e2376f9ab661bb186be57/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/highfive/2.3.1-0e6a778533d40e19f29620ee283e157b/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/geant4/11.1.1-85871d3cdd4056ae82ca1222f4c691a8/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/classlib/3.1.3-1d308dca8ca65ff1cbc6e2054c82f068/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/lhapdf/6.4.0-0567ceac11a56211ab3d355136404861/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/alpaka/develop-20230621-4a5b307842945a74a8938685da2f8981/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/tkonlinesw/4.2.0-1_gcc7-e7864fe6081e777a0847c68393a48563/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/pcre2/10.36-1886f26dc2e8df76d3f914744d19acb2/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libungif/4.1.4-61dbf35cddb46b2883326eba676144bf/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libtiff/4.0.10-8fe8665213ddacfc3eba961f976027a8/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libpng/1.6.37-81ddbdaac0ba5de8773b78314e824dd5/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/frontier_client/2.10.2-196a97fbfe5e0b3f6d1bc79a3384ac95/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/pcre/8.43-bd2b09f5d686f0f36e748ce001d315ad/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/zstd/1.5.2-9c1c70d9486630411103f55761f0d5b9/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/xrootd/5.5.4-deca31cae0444e86d559664becbac47e/include/xrootd/private",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/cms/vdt/0.4.3-7f0e798709b0c2350f0dab5c6fffd8dc/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/valgrind/3.17.0-7bfcd2b5e4f162fb4b127c18285f46f6/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/utm/utm_0.11.2-39aa53514c8231242b9c1e6bcf667912/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/sigcpp/3.2.0-bed3c55104fa82ab92d9831535418de9/include/sigc++-3.0",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/sqlite/3.36.0-c837897209ac3cd9a2a02f4c19fac98c/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/pacparser/1.4.2-92f8d09349218d64316a12325aa74de0/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/oracle/19.11.0.0.0dbru-d78b873b6f23ccd82b2fba6b96e8a07e/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/py3-numpy/1.23.5-9372f23a4cd71891ea1e76ebb6379964/c-api/core/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/meschach/1.2.pCMS1-ff9e9df2d4adaf1c59a7f93c9f063fb6/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/lz4/1.9.2-9d28b0ce2b953bcb372016192753a787/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libuuid/2.34-f7577986509a353c203144983884d697/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libjpeg-turbo/2.0.2-329ab09875e3037b12921a8a27c997ef/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/ktjet/1.06-0f9785ccb8131d966f009132b227fff8/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/jemalloc-prof/5.3.0-a727ee9002629169a3459ece4cd1ffa2/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/jemalloc-debug/5.3.0-75f5d20e1a9c38bb9292d13cf3ede57d/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/jemalloc/5.3.0-06c2c86c055776fb91f0788e6ce07e94/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/hls/2019.08-4428055fbd31b592a2034ea710d783a0/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/madgraph5amcatnlo/2.7.3-565058dc166d62fe687db89b5b2e0a95",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/heppdt/3.04.01-ed8ba505fa2cb25fa1d10290fef55269/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/hector/1.3.4_patch1-9610c362cc8ee5193f4ad4d13b068807/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/protobuf/3.15.1-22ec1dcddbf458d5d16ff948fdc8121d/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libunwind/1.6.2-master-fa16514b09fe7d17bf150f74dfbab3e6/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/giflib/5.2.0-7d9891c7554db9367a9378e414961bc9/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/gdbm/1.10-382e01e82d1e659622ead4d97195acce/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/freetype/2.10.0-70eef1c793b0344adc788bd418cd2875/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/fftw3/3.3.8-38111740ee8754cdad106c5e8b70590c/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/fftjet/1.5.0-411c50ed3f46ef01064a239f066dc9e3/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/fastjet/3.4.1-f28a80272e1d06ca29375cb50e90ea55/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/expat/2.1.0-ef6b5417054cd31c1e70e4b94002d0fd/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/vecgeom/v1.2.1-8a31bd651cdd8476092d8c2c16f28c91/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/vecgeom/v1.2.1-8a31bd651cdd8476092d8c2c16f28c91/include/VecGeom",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/xerces-c/3.1.3-34f18b2d106eeefb6ef989b2cb7004d9/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/xz/5.2.5-56c8544f64e9d56c1108fbe00c3ecb67/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/dcap/2.47.12-019f2f54394afc3e614bb2fce1e0bf7b/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/libxml2/2.9.10-a1b4b795e753f7a6ea1deecf9cc33818/include/libxml2",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/curl/7.79.0-639c75cb2c36dac1daa9f13cb5b6668e/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/cppunit/1.15.x-6748e4047047d9556fd9ff8edb671e5f/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/cms/coral/CORAL_2_3_21-c8ac0d60d6112154786c812c4cf91acb/include/LCG",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/clhep/2.4.6.0-25a101e56144b60e8e6c8533bc6203b4/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/zlib/1.2.11-a365170a889b785ec23815da2b99d7d1/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/pythia6/426-ca47123f257e51b9a8e8530551b17c47/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/hepmc/2.06.10-85bd895bb097aa1637e1468b4ed8f785/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/gsl/2.6-293f1973c8de87040110bce5dc9d71f6/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/bz2lib/1.0.6-24b287d9981341b8441eb85733326b1a/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/tbb/v2021.8.0-7e31093a7b4a477d01bc3946dd0bf612/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/rocm-rocrand/5.4.3-59ec7ff1e128a201a896841ef6b6354d/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/cuda/11.8.0-9f0af0f4206be7b705fe550319c49a11/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/boost/1.80.0-5305613b2f750cf1a05dcadf0d672647/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/xgboost/1.7.5-341fbcc552d52a1d84e61813dc6267a2/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/tinyxml2/6.2.0-f05bc085db13b8b4b752c87703ff413d/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/rdma-core/39.1-98699332870e832aee9b877306e0ec32/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/ittnotify/16.06.18-8326c97bb3ba7e2034a606d79815459d/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/gosamcontrib/2.0-20150803-e6b90a34027839b6651dfdcbd284840f/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/fmt/8.0.1-89199f97a8c166a965017c69137de0d0/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/eigen/82dd3710dac619448f50331c1d6a35da673f764a-f9c27fce684e89466e2ef07869cd264d/include/eigen3",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/rocm/5.4.3-1dab3e4b9440033e3ae9cb3d28564caf/include",
"/usr/local/include",
"/usr/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/etc/",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/etc//cling",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/etc//cling/plugins/include",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/include/",
"/cvmfs/cms.cern.ch/el8_amd64_gcc11/lcg/root/6.26.11-50eed3272fcfa103ebe9cf3182b98eb9/include/",
"/home/data/yjlee/PhysicsZHadronEEC/TrackingEfficiency/20240806_ResidualCorrection/workflow/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "plot_corrections_C_ACLiC_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "plot_corrections_C_ACLiC_dict dictionary payload"

#ifndef __ACLIC__
  #define __ACLIC__ 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "./plot_corrections.C"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"main", payloadCode, "@",
"plot_corrections", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("plot_corrections_C_ACLiC_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_plot_corrections_C_ACLiC_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_plot_corrections_C_ACLiC_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_plot_corrections_C_ACLiC_dict() {
  TriggerDictionaryInitialization_plot_corrections_C_ACLiC_dict_Impl();
}
