//===--- IdentifierNameCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MICROSOFT_IDENTIFIER_NAME_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MICROSOFT_IDENTIFIER_NAME_CHECK_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {

/// \brief Enforces LLVM Naming convention for Identifiers
class IdentifierNameCheck : public ClangTidyCheck {
public:
  IdentifierNameCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  enum CodeFlags {
    MustStartLowerCase = 0x1,
    MustStartUpperCase = 0x2,
    ShouldStartUpperCase = 0x4,
    UnderScorePermitted = 0x5
  };

  void warn(const clang::Decl *Declaration, StringRef Message);

  void functionCheck(const clang::FunctionDecl *Function);
  void namespaceCheck(const clang::NamespaceDecl *NameSpace);
  void variableCheck(const clang::VarDecl *Function);

  void isCamlCaseCheck(const clang::NamedDecl *Declaration, std::string Message,
                       unsigned CodeFlags);
};

} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MICROSOFT_IDENTIFIER_NAME_CHECK_H
