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
  Finder->addMatcher(enumConstantDecl().bind("enumconst"), this);
  Finder->addMatcher(namespaceDecl().bind("namespace"), this);
}

void IdentifierNameCheck::warn(const clang::SourceLocation Location,
                               StringRef Message) {
  diag(Location, Message);
}

void IdentifierNameCheck::checkCase(const clang::SourceLocation Location,
                                    const StringRef Name, std::string Message,
                                    unsigned CodeFlags) {
  char FirstLetter = Name.front();

  if ((CodeFlags & CodeFlags::MustStartUpperCase) ||
      (CodeFlags & CodeFlags::ShouldStartUpperCase)) {
    if (clang::isUppercase(FirstLetter)) {
      return;
    }

    if ((CodeFlags & CodeFlags::ShouldStartUpperCase) &&
        Name.equals(Name.lower())) {
      // Permit lower case names for integer variables
      return;
    }

    warn(Location, Message + " name must begin with Upper-case");
    return;
  }

  if (CodeFlags & CodeFlags::MustStartLowerCase) {
    if (clang::isLowercase(FirstLetter)) {
      return;
    }

    warn(Location, Message + " name must begin with lower-case");
  }
}

// Check that a Declaration's identifier is written in CameCase,
// starting with an upper/lower case letter as specified in CodeFlags

void IdentifierNameCheck::isCamlCaseCheck(const clang::SourceLocation Location,
                                          StringRef Name, std::string Message,
                                          unsigned CodeFlags) {
  checkCase(Location, Name, Message, CodeFlags);

  size_t UnderscorePos = Name.find('_');

  if (UnderscorePos != StringRef::npos) {
    if (CodeFlags & CodeFlags::UnderScorePermitted) {
      // For Enumerations, a single qualifier using underscore is permitted.
      // Normal naming rules are enforced after the qualifier.

      Name = Name.substr(UnderscorePos + 1);

      if (Name.empty()) {
        warn(Location, Message + " name has incorrect use of '_'");
      } else {
        isCamlCaseCheck(Location, Name, Message,
                        CodeFlags & ~CodeFlags::UnderScorePermitted);
      }

      return;
    }

    // Some relaxation may be needed here for STL-like definitions
    // like push_back(), start_globals(), etc.
    // Will be added when encountered.
    warn(Location, Message + " name must be camlCased (without '_')");
  }

  // Check for all-Upper case variables, except trivial ones
  if ((Name.size() > 2) && Name.equals(Name.upper())) {
    warn(Location, Message + " name must be in camlCase, not UPPER case");
  }
}

void IdentifierNameCheck::isCamlCaseCheck(const clang::NamedDecl *Declaration,
                                          std::string Message,
                                          unsigned CodeFlags) {
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

  if (Name.startswith("__")) {
    // As a special case, permit __special_names
    return;
  }

  isCamlCaseCheck(Declaration->getLocation(), Declaration->getName(), Message,
                  CodeFlags);
}

void IdentifierNameCheck::functionCheck(const clang::FunctionDecl *Function) {
  if (Function->isOverloadedOperator()) {
    return;
  }

  IdentifierInfo *identifier = Function->getIdentifier();
  if (identifier == nullptr) {
    return;
  }

  // Permit certain special names that may not conform to conventions
  StringRef Name = Function->getName();
  if (Name.equals("DllMain")) {
    return;
  }

  isCamlCaseCheck(Function, "Function", CodeFlags::MustStartLowerCase);
}

void
IdentifierNameCheck::namespaceCheck(const clang::NamespaceDecl *NameSpace) {
  StringRef Name = NameSpace->getName();

  if (Name.empty()) {
    warn(NameSpace->getLocation(), "Avoid using anonymous namespaces");
  }
}

void IdentifierNameCheck::variableCheck(const clang::VarDecl *Variable) {
#ifdef LENIENT_INT_PTR_RULES
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
#else
  isCamlCaseCheck(Variable, "Variable", CodeFlags::MustStartUpperCase);
#endif
}

void IdentifierNameCheck::check(const MatchFinder::MatchResult &Result) {
  // Match Functions
  // void F(int x)
  const clang::FunctionDecl *Function =
      Result.Nodes.getNodeAs<FunctionDecl>("function");
  if (Function != nullptr) {
    functionCheck(Function);
    return;
  }

  // Match variables -- locals, globals and parameters
  const clang::VarDecl *Variable = Result.Nodes.getNodeAs<VarDecl>("variable");
  if (Variable != nullptr) {
    variableCheck(Variable);
    return;
  }

  // Match class/struct/union fields
  const clang::NamedDecl *Field = Result.Nodes.getNodeAs<NamedDecl>("field");
  if (Field != nullptr) {
    isCamlCaseCheck(Field, "Field", CodeFlags::MustStartUpperCase);
    return;
  }

  // Match class/struct/unions
  const clang::NamedDecl *Type = Result.Nodes.getNodeAs<NamedDecl>("record");
  if (Type != nullptr) {
    isCamlCaseCheck(Type, "Type", CodeFlags::MustStartUpperCase);
    return;
  }

  // Match typedefs
  const clang::NamedDecl *TypeDef =
      Result.Nodes.getNodeAs<NamedDecl>("typedef");
  if (TypeDef != nullptr) {
    isCamlCaseCheck(TypeDef, "Type", CodeFlags::MustStartUpperCase);
    return;
  }

  // Match Enumerations
  const clang::NamedDecl *Enum = Result.Nodes.getNodeAs<NamedDecl>("enum");
  if (Enum != nullptr) {
    isCamlCaseCheck(Enum, "Enumeration", CodeFlags::MustStartUpperCase);
    return;
  }

  // Match Enumeration Constants
  const clang::NamedDecl *EnumConst =
      Result.Nodes.getNodeAs<NamedDecl>("enumconst");
  if (EnumConst != nullptr) {
    isCamlCaseCheck(EnumConst, "Enumeration Constant",
                    CodeFlags::MustStartUpperCase |
                        CodeFlags::UnderScorePermitted);
    return;
  }

  // Match Namespace declarations
  const clang::NamespaceDecl *NameSpace =
      Result.Nodes.getNodeAs<NamespaceDecl>("namespace");
  if (NameSpace != nullptr) {
    namespaceCheck(NameSpace);
    return;
  }
}

} // namespace tidy
} // namespace clang
