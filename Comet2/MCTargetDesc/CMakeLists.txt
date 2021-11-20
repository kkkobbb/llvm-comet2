add_llvm_component_library(LLVMComet2Desc
  Comet2AsmBackend.cpp
  Comet2ELFObjectWriter.cpp
  Comet2InstPrinter.cpp
  Comet2MCAsmInfo.cpp
  Comet2MCCodeEmitter.cpp
  Comet2MCExpr.cpp
  Comet2MCTargetDesc.cpp
  Comet2TargetStreamer.cpp
  Comet2ELFStreamer.cpp

  LINK_COMPONENTS
  MC
  Comet2Info
  Support

  ADD_TO_COMPONENT
  Comet2
)
