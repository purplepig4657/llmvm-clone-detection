#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassInstrumentation.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include <llvm/AsmParser/Parser.h>
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <stdlib.h>
#include <cstring>

namespace {

    std::string result;

    class ProgramAbstractionAnalysis : public llvm::AnalysisInfoMixin<ProgramAbstractionAnalysis> {
    public:
        explicit ProgramAbstractionAnalysis() = default;
        ~ProgramAbstractionAnalysis() = default;

        static inline llvm::AnalysisKey Key;
        friend llvm::AnalysisInfoMixin<ProgramAbstractionAnalysis>;

        using Result = std::string;

        static Result run(llvm::Module &M, [[maybe_unused]] llvm::ModuleAnalysisManager &MAM) {
            for (llvm::Function &F : M) {
                if (shouldIgnoreFunction(F)) {
                    continue;
                }

                for (llvm::BasicBlock &BB : F) {
                    for (llvm::Instruction &I : BB) {
                        std::string typeStr;
                        llvm::raw_string_ostream rso(typeStr);

                        llvm::Type *ty = I.getType();

                        if (I.getOpcode() == llvm::Instruction::Ret) {
                            result += "Return: ";

                            if (auto *returnInst = llvm::dyn_cast<llvm::ReturnInst>(&I)) {
                                llvm::Value *ret_value = returnInst->getReturnValue();

                                if (ret_value) {
                                    ret_value->getType()->print(rso);
                                } else {
                                    I.getType()->print(rso);
                                }
                            }

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Br) {
                            result += "Branch: ";

                            if (auto *brInst = llvm::dyn_cast<llvm::BranchInst>(&I)) {
                                typeStr += brInst->getSuccessor(0)->getName();
                                if (brInst->isConditional()) {
                                    typeStr += " ";
                                    typeStr += brInst->getSuccessor(1)->getName();
                                }
                            }

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Switch) {
                            result += "Switch: ";

                        } else if (I.getOpcode() == llvm::Instruction::IndirectBr) {
                            result += "IndirectBranch: ";

                        } else if (I.getOpcode() == llvm::Instruction::Invoke) {
                            result += "Invoke: ";

                        } else if (I.getOpcode() == llvm::Instruction::Unreachable) {
                            result += "Unreachable: ";

                        } else if (I.getOpcode() == llvm::Instruction::Add || I.getOpcode() == llvm::Instruction::FAdd ||
                                   I.getOpcode() == llvm::Instruction::Sub || I.getOpcode() == llvm::Instruction::FSub ||
                                   I.getOpcode() == llvm::Instruction::Mul || I.getOpcode() == llvm::Instruction::FMul ||
                                   I.getOpcode() == llvm::Instruction::Shl || I.getOpcode() == llvm::Instruction::And ||
                                   I.getOpcode() == llvm::Instruction::Or || I.getOpcode() == llvm::Instruction::Xor) {
                            std::string opName = I.getOpcodeName();
                            result += opName + ": ";

                            if (I.getNumOperands() > 0) {
                                I.getOperand(0)->getType()->print(rso);
                                typeStr += " ";
                            }
                            if (I.getNumOperands() > 1) {
                                I.getOperand(1)->getType()->print(rso);
                            }

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Alloca) {
                            result += "Alloca: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Load) {
                            result += "Load: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Store) {
                            result += "Store: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getOperand(1)->getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::GetElementPtr) {
                            result += "GetElementPtr: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getOperand(1)->getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::PtrToInt) {
                            result += "PtrToInt: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::IntToPtr) {
                            result += "IntToPtr: ";

                            I.getOperand(0)->getType()->print(rso);
                            typeStr += " ";
                            I.getType()->print(rso);

                            result += typeStr + " ";
                        } else if (I.getOpcode() == llvm::Instruction::Call) {
                            result += "Call: ";

                            if (auto *callInst = llvm::dyn_cast<llvm::CallInst>(&I)) {
                                llvm::Function *called_f = callInst->getCalledFunction();
                                if (called_f != nullptr) {
                                    typeStr += called_f->getName().str();
                                    typeStr += " ";
                                    called_f->getType()->print(rso);
                                }

                                result += typeStr + " ";
                            }
                        } else {
                            result += "Other: ";

                            I.getType()->print(rso);

                            result += typeStr + " ";
                        }
                    }
                }
            }

            return result;
        }

    private:
        static bool shouldIgnoreFunction(const llvm::Function &F) {
            llvm::StringRef functionName = F.getName();
            return functionName.startswith("_ZN") || functionName.startswith("__") || functionName.startswith("_");
        }
    };

    class ProgramAbstractionPass : public llvm::PassInfoMixin<ProgramAbstractionPass> {
    public:
        explicit ProgramAbstractionPass() = default;
        ~ProgramAbstractionPass() = default;

        static llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
            auto result = MAM.getResult<ProgramAbstractionAnalysis>(M);
            return llvm::PreservedAnalyses::none();
        }
    };

}

extern "C" char* transform(const char* targetString) {
    llvm::SMDiagnostic Diag;
    llvm::LLVMContext CTX;
    llvm::StringRef input(targetString);
    std::unique_ptr<llvm::Module> M = llvm::parseAssemblyString(input, Diag, CTX);

    llvm::PassBuilder PB;
    llvm::ModuleAnalysisManager MAM;
    llvm::ModulePassManager MPM;
    ProgramAbstractionAnalysis PAA;

    MAM.registerPass([&]() { return PAA; });
    PB.registerModuleAnalyses(MAM);
    MPM.addPass(ProgramAbstractionPass());
    MPM.run(*M, MAM);

    int result_length = result.length();

    char* str = (char*)malloc(sizeof(char) * result_length + 1);
    strcpy(str, result.c_str());

    return str;
}

extern "C" void myFree(char* ptr) {
    result = "";
    free(ptr);
}

