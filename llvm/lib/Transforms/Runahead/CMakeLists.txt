add_llvm_component_library(LLVMRunaheadOpts
  InsertJRE.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/Runahead

  DEPENDS
  intrinsics_gen

  COMPONENT_NAME
  Runahead

  LINK_COMPONENTS
  AggressiveInstCombine
  Analysis
  Core
  InstCombine
  Support
  TransformUtils
  )
