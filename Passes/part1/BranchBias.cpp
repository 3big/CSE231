//
// Created by Teofilo Zosa on 1/23/18.
//
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
//#include "/lib231/lib231.cpp"
#include <map>
#include <vector>
using namespace llvm;


namespace {



    struct BranchBias : public FunctionPass {

        static char ID;
        BranchBias() : FunctionPass(ID) {}

        virtual bool doInitialization(Module &mod) override {
            LLVMContext &context = mod.getContext();



            return true;
        }


        bool runOnFunction(Function &F) override
        {
//            vector<bool>
//            errs() <<F.getName() << '\n';
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
                errs() <<B->getName() << '\n';

                bool taken = true; // since we're instrumenting the code, this will only happen if it's true.

                for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I) {
                    // Here I is a pointer to an instruction

                    int instruction = I->getOpcode();
                    if (instruction == 2){
//                        errs() << I->getOperand(0)->getName() << '\n';
//                        errs() << I->getOperand(1)->getName() << '\n';
                        BranchInst* brInst = cast<BranchInst>(I);
                        if(brInst->isConditional()){
                            IRBuilder<> Builder(brInst);
                            std::vector<Value*> args;
                            bool cond = brInst->getCondition() != 0;
                            Constant * cond_eval = ConstantInt::get(IntegerType::get(context, 1), cond);
                            Value * condVal= brInst->getCondition();
                            args.push_back(condVal);
                            Builder.CreateCall(updateBranchInfoFunc, args);
//
//                            errs() << brInst->getCondition() << '\n';
//                            errs() << brInst->getOperand(0) << '\n';
//                            errs() << brInst->getOperand(1) << '\n';
//                            errs() << brInst->getOperand(2) << '\n';

                        }
                    }


                }





            }
//
//
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


