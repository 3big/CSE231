//
// Created by Teofilo Zosa on 1/23/18.
//
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <vector>
using namespace llvm;


namespace {



    struct BranchBias : public FunctionPass {

        static char ID;
        BranchBias() : FunctionPass(ID) {}
        bool runOnFunction(Function &F) override
        {
            Module* mod = (F.getParent());
            LLVMContext &context = mod->getContext();
            Function *updateBranchInfoFunc = cast<Function>(mod->getOrInsertFunction(
                    "updateBranchInfo",                           // name of function
                    Type::getVoidTy(context),        // return type
                    Type::getInt1Ty(context)       // first parameter type
                
            ));
            Function *printOutBranchInfoFunc = cast<Function>(
                    mod->getOrInsertFunction(
                            "printOutBranchInfo",                           // name of function
                            Type::getVoidTy(context)        // return type
                                                             // no parameters
                    ));
            for (Function::iterator B = F.begin(), BE = F.end(); B != BE; ++B) {
                // Here B is a pointer to a basic block

                for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I) {
                    // Here I is a pointer to an instruction

                    int instruction = I->getOpcode();
                    if (instruction == 2){
                        BranchInst* brInst = cast<BranchInst>(I);
                        if(brInst->isConditional()){
                            IRBuilder<> Builder(brInst);
                            std::vector<Value*> args;
                            args.push_back(brInst->getCondition());
                            Builder.CreateCall(updateBranchInfoFunc, args);

                        }
                    }

                }

            }
            IRBuilder<> Builder(&(F.back().back()));
            Builder.CreateCall(printOutBranchInfoFunc);
            return true;
        }
    }; // end of struct TestPass
}  // end of anonymous namespace
char BranchBias::ID = 2;
static RegisterPass<BranchBias> X("cse231-bb", "Developed to count the branches taken",
                                                false /* Only looks at CFG */,
                                                false /* Analysis Pass */);


