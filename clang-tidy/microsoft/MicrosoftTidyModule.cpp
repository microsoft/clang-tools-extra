//===--- MicrosoftTidyModule.cpp - clang-tidy ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "IdentifierNameCheck.h"

namespace clang {
namespace tidy {

class MicrosoftModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<IdentifierNameCheck>("microsoft-identifier-name");
  }
};

// Register the LLVMTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<MicrosoftModule> X("microsoft-module",
                                                       "Adds Microsoft lint checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the MicrosoftModule.
volatile int MicrosoftModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
