//===--- IdentifierNameCheck.cpp - clang-tidy -------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "IdentifierNameCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/ASTContext.h"
#include <ctype.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {

void IdentifierNameCheck::registerMatchers(ast_matchers::MatchFinder *Finder) {
  Finder->addMatcher(functionDecl().bind("function"), this);
  Finder->addMatcher(varDecl().bind("variable"), this);
  Finder->addMatcher(fieldDecl().bind("field"), this);
  Finder->addMatcher(recordDecl().bind("record"), this);
  Finder->addMatcher(typedefDecl().bind("typedef"), this);
  Finder->addMatcher(enumDecl().bind("enum"), this);
  Finder->addMatcher(enumConstantDecl().bind("enumConst"), this);
  Finder->addMatcher(namespaceDecl().bind("namespace"), this);
}

void IdentifierNameCheck::warn(const clang::Decl *Declaration,
                               StringRef Message) {
  diag(Declaration->getLocation(), Message);
}

// Check that a Declaration's identifier is written in CameCase,
// starting with an upper/lower case letter as specified in CodeFlags

void IdentifierNameCheck::isCamlCaseCheck(const clang::NamedDecl *Declaration,
                                          std::string Message,
                                          unsigned CodeFlags) {
  if (Declaration == nullptr) {
    return;
  }

  if (Declaration->isImplicit()) {
    return;
  }

  IdentifierInfo *identifier = Declaration->getIdentifier();

  if (identifier == nullptr) {
    // Skip definitions that do not define an identifier
    // Ex: unnamed class, C++ constructor, Objective-C selector, etc.
    return;
  }

  StringRef Name = Declaration->getName();
  char FirstLetter = Name.front();

  if ((CodeFlags & CodeFlags::MustStartUpperCase) ||
      (CodeFlags & CodeFlags::ShouldStartUpperCase)) {
    if (!clang::isUppercase(FirstLetter)) {

      if ((CodeFlags & CodeFlags::ShouldStartUpperCase) &&
          Name.equals(Name.lower())) {
        // Permit lower case names for integer variables
      } else {
        warn(Declaration, Message + " name must begin with Upper-case");
      }
    }

    // Check for all-Upper case variables, except trivial ones
    if ((Name.size() > 2) && Name.equals(Name.upper())) {
      warn(Declaration, Message + " name must be in camlCase, not UPPER case");
    }
  } else if (CodeFlags & CodeFlags::MustStartLowerCase) {
    if (!clang::isLowercase(FirstLetter)) {
      warn(Declaration, Message + " name must begin with lower-case");
    }
  }

  if ((CodeFlags & CodeFlags::UnderScorePermitted) &&
      (Name.find('_') != StringRef::npos)) {
    // Some relaxation may be needed here for STL-like defintions
    // like push_back(), start_globals(), etc.
    // Will be added when encountered.
    warn(Declaration, Message + " name must be camlCased (without '_')");
  }
}

void IdentifierNameCheck::functionCheck(const clang::FunctionDecl *Function) {
  if (Function == nullptr) {
    return;
  }

  if (Function->isOverloadedOperator()) {
    return;
  }

  isCamlCaseCheck(Function, "Function", CodeFlags::MustStartLowerCase);
}

void
IdentifierNameCheck::namespaceCheck(const clang::NamespaceDecl *NameSpace) {
  if (NameSpace == nullptr) {
    return;
  }

  StringRef Name = NameSpace->getName();

  if (Name.empty()) {
    warn(NameSpace, "Avoid using anonymous namespaces");
  }
}

void IdentifierNameCheck::variableCheck(const clang::VarDecl *Variable) {
  if (Variable == nullptr) {
    return;
  }

  // Variables generally must start with an Uppercase letter.
  // However, we allow certain integer variables to be in lowercase.
  // For example:
  //    for(int i=0; i<10; i++) { ... }
  //    int main(int argc, char **argv) { ... }
  //    while (*p++) { ... }
  // These variables marked "should start upper case" and must be one of:
  // (1) CamelCased starting with uppercase, or (2) All lowercase.

  clang::QualType VariableType = Variable->getType();
  bool IsIntegerLocal =
      VariableType->isIntegerType() || VariableType->isPointerType();

  isCamlCaseCheck(Variable, "Variable", IsIntegerLocal
                                            ? CodeFlags::ShouldStartUpperCase
                                            : CodeFlags::MustStartUpperCase);
}

void IdentifierNameCheck::check(const MatchFinder::MatchResult &Result) {
  // Match Functions
  // void F(int x)
  const clang::FunctionDecl *Function =
      Result.Nodes.getNodeAs<FunctionDecl>("function");
  functionCheck(Function);

  // Match variables -- locals, globals and parameters
  const clang::VarDecl *Variable = Result.Nodes.getNodeAs<VarDecl>("variable");
  variableCheck(Variable);

  // Match class/struct/union fields
  const clang::FieldDecl *Field = Result.Nodes.getNodeAs<FieldDecl>("field");
  isCamlCaseCheck(Field, "Field", CodeFlags::MustStartUpperCase);

  // Match class/struct/unions
  const clang::TypeDecl *Type = Result.Nodes.getNodeAs<TypeDecl>("type");
  isCamlCaseCheck(Type, "Type", CodeFlags::MustStartUpperCase);

  // Match typedefs
  const clang::TypeDecl *TypeDef = Result.Nodes.getNodeAs<TypeDecl>("typedef");
  isCamlCaseCheck(TypeDef, "Type", CodeFlags::MustStartUpperCase);

  // Match Enumerations
  const clang::TypeDecl *Enum = Result.Nodes.getNodeAs<TypeDecl>("enum");
  isCamlCaseCheck(Enum, "Enumeration", CodeFlags::MustStartUpperCase);

  // Match Enumeration Constants
  const clang::EnumConstantDecl *EnumConst =
      Result.Nodes.getNodeAs<EnumConstantDecl>("enumconst");
  isCamlCaseCheck(EnumConst, "Enumeration Constant",
                  CodeFlags::MustStartUpperCase |
                      CodeFlags::UnderScorePermitted);

  // Match Namespace declarations
  const clang::NamespaceDecl *NameSpace =
      Result.Nodes.getNodeAs<NamespaceDecl>("namespace");
  namespaceCheck(NameSpace);
}

} // namespace tidy
} // namespace clang
