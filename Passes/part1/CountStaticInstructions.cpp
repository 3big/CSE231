//
// Created by Teofilo Zosa on 1/23/18.
//
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
using namespace llvm;


namespace {
    struct CountStaticInstructions : public FunctionPass {
        static char ID;
        CountStaticInstructions() : FunctionPass(ID) {}
        std::map<std::string, int> defaultDict;
//        virtual bool doInitialization(Module &mod) override {
//            LLVMContext &context = mod.getContext();
//        }

        bool runOnFunction(Function &F) override {
            int num_func_blocks = 0;
            for (Function::iterator B = F.begin(), BE = F.end(); B != BE; ++B) {
                // Here B is a pointer to a basic block
                int num_instr = 0;
                for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I) {
                    // Here I is a pointer to an instruction
                    num_instr += 1;
                    //                errs() << "Hello: ";
//                    errs().write_escaped(I->getOpcodeName()) << '\n';
                    std::string instruction = I->getOpcodeName();
                    defaultDict[instruction] += 1;
                }
            }
            for (std::map<std::string, int>::iterator i=defaultDict.begin(); i!=defaultDict.end(); i++) {
                errs()<< i->first << '\t' << i->second << '\n';
            }

            return false;
        }
    }; // end of struct TestPass
}  // end of anonymous namespace

char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi", "Developed to count static instructions",
                                false /* Only looks at CFG */,
                                false /* Analysis Pass */);


