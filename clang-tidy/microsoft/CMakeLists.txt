set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyMicrosoftModule
  MicrosoftTidyModule.cpp
  IdentifierNameCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyReadabilityModule
  clangTidyUtils
  clangTooling
  )
