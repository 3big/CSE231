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



    struct CountDynamicInstructions : public FunctionPass {

        static char ID;
        CountDynamicInstructions() : FunctionPass(ID) {}
        bool runOnFunction(Function &F) override
        {
            Module* mod = (F.getParent());
            LLVMContext &context = mod->getContext();
            Function *updateInstrInfoFunc = cast<Function>(mod->getOrInsertFunction(
                    "updateInstrInfo",                           // name of function
                    Type::getVoidTy(context),        // return type
                    Type::getInt64Ty(context),       // first parameter type
                    Type::getInt32PtrTy(context),       // second parameter type
                    Type::getInt32PtrTy(context)       // third parameter type

            ));
            Function *printOutInstrInfoFunc = cast<Function>(
                    mod->getOrInsertFunction(
                    "printOutInstrInfo",                           // name of function
                    Type::getVoidTy(context)        // return type
                    // no parameters
            ));
            for (Function::iterator B = F.begin(), BE = F.end(); B != BE; ++B) {
                // Here B is a pointer to a basic block
                std::map<int , int > defaultDict;
                for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I) {
                    // Here I is a pointer to an instruction
                    int instruction = I->getOpcode();
                    defaultDict[instruction] += 1;
                }
                unsigned num_instructions = defaultDict.size();
                Constant * num_instr = ConstantInt::get(IntegerType::get(context, 64), num_instructions);
                uint32_t keys_static [num_instructions];
                uint32_t values_static [num_instructions];

                int j = 0;
                for (std::map<int, int>::iterator i=defaultDict.begin(); i!=defaultDict.end(); i++) {
                    keys_static[j] = i->first;
                    values_static[j] = i->second;
                    ++j;
                }

                IRBuilder<> Builder(&(B->back()));
                ArrayType* arrayTy = ArrayType::get(IntegerType::get(context, 32), num_instructions);
                GlobalVariable* keys = new GlobalVariable(
                        *mod,
                        arrayTy,
                        true,
                        GlobalValue::InternalLinkage,
                        ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(keys_static, num_instructions))),
                        "keys");
                GlobalVariable* values = new GlobalVariable(
                        *mod,
                        arrayTy,
                        true,
                        GlobalValue::InternalLinkage,
                        ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(values_static, num_instructions))),
                        "values");
                std::vector<Value*> args;
                args.push_back(num_instr);
                Value* key_0 = Builder.CreatePointerCast(keys, Type::getInt32PtrTy(context));
                Value* value_0 = Builder.CreatePointerCast(values, Type::getInt32PtrTy(context));
                args.push_back(key_0);
                args.push_back(value_0);
                Builder.CreateCall(updateInstrInfoFunc, args);
            }
            IRBuilder<> Builder(&(F.back().back()));
            Builder.CreateCall(printOutInstrInfoFunc);
            return true;
        }
    }; // end of struct TestPass

}  // end of anonymous namespace
char CountDynamicInstructions::ID = 1;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "Developed to count dynamic instructions",
                                               false /* Only looks at CFG */,
                                               false /* Analysis Pass */);


