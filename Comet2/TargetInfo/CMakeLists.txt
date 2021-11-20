add_llvm_component_library(LLVMComet2Info
  Comet2TargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  Comet2
  )
