add_llvm_component_group(Comet2)

set(LLVM_TARGET_DEFINITIONS Comet2.td)

tablegen(LLVM Comet2GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM Comet2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Comet2GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Comet2GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Comet2GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM Comet2GenGlobalISel.inc -gen-global-isel)
tablegen(LLVM Comet2GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Comet2GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM Comet2GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM Comet2GenRegisterBank.inc -gen-register-bank)
tablegen(LLVM Comet2GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Comet2GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(Comet2CommonTableGen)

add_llvm_target(Comet2CodeGen
  Comet2AsmPrinter.cpp
  Comet2FrameLowering.cpp
  Comet2ISelDAGToDAG.cpp
  Comet2ISelLowering.cpp
  Comet2InstrInfo.cpp
  Comet2MCInstLower.cpp
  Comet2RegisterInfo.cpp
  Comet2Subtarget.cpp
  Comet2TargetMachine.cpp
  Comet2TargetObjectFile.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  Core
  CodeGen
  MC
  Comet2Desc
  Comet2Info
  SelectionDAG
  Support
  Target

  ADD_TO_COMPONENT
  Comet2
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
