//
// Created by Teofilo Zosa on 1/23/18.
//
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "/lib231/lib231.cpp"
#include <map>
#include <vector>
using namespace llvm;


namespace {



    struct CountDynamicInstructions : public FunctionPass {

        static char ID;
        CountDynamicInstructions() : FunctionPass(ID) {}

        virtual bool doInitialization(Module &mod) override {
            LLVMContext &context = mod.getContext();
//            Constant *updateInstrInfoFunc = mod.getOrInsertFunction(
//                    "updateInstrInfo",                           // name of function
//                    Type::getVoidTy(context),        // return type
//                    Type::getInt64Ty(context),       // first parameter type
//                    Type::getInt32PtrTy(context),       // second parameter type
//                    Type::getInt32PtrTy(context)       // third parameter type
//
//            );


            return true;
        }
//        virtual bool doFinalization(Module &mod) override{
//            LLVMContext &context = mod.getContext();
//            Constant *printOutInstrInfoFunc = mod.getOrInsertFunction(
//                    "printOutInstrInfo",                           // name of function
//                    Type::getVoidTy(context)        // return type
//                    // no parameters
//            );
//            IRBuilder<> Builder();
//            Builder.CreateCall(printOutInstrInfoFunc);
//            return true;
//        };


        bool runOnFunction(Function &F) override
        {
//            errs() <<F.getName() << '\n';
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
//                errs() <<B->getName() << '\n';

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
//
//
//            unsigned num_instructions = defaultDict.size();
//            Constant * num_instr = ConstantInt::get(IntegerType::get(context, 64), num_instructions);
////            Type * itype = IntegerType::get(mod->getContext(), 32);
//
//
////            Value*  keys_static [num_instructions];
////            Value* values_static [num_instructions];
//
//
//            uint32_t keys_static [num_instructions];
//            uint32_t values_static [num_instructions];
//
//
//            int j = 0;
//            for (std::map<int, int>::iterator i=defaultDict.begin(); i!=defaultDict.end(); i++) {
////                keys_static[j] = ConstantInt::get(IntegerType::get(context, 32), i->first);
////                values_static[j] = ConstantInt::get(IntegerType::get(context, 32), i->second);
//
//                keys_static[j] = i->first;
//                values_static[j] = i->second;
//
//
//                ++j;
//            }
            IRBuilder<> Builder(&(F.back().back()));
//            ArrayType* arrayTy = ArrayType::get(IntegerType::get(context, 32), num_instructions);
////
//            GlobalVariable* keys = new GlobalVariable(
//                    *mod,
//                    arrayTy,
//                    true,
//                    GlobalValue::InternalLinkage,
//                    ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(keys_static, num_instructions))),
//                    "keys");
//            GlobalVariable* values = new GlobalVariable(
//                    *mod,
//                    arrayTy,
//                    true,
//                    GlobalValue::InternalLinkage,
//                    ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(values_static, num_instructions))),
//                    "values");
////            GlobalVariable* num_instr = new GlobalVariable(
////                    *mod,
////                    Type::getInt32Ty,
////                    true,
////                    GlobalValue::InternalLinkage,
////                    DataArray::get(context, num_instructions),
////                    "num_instructions");
////            ArrayRef<ConstantInt> keys = ArrayRef<ConstantInt>(keys_static);//ArrayType::get(IntegerType::get(F.getContext(), 32), num_instructions);
////            ArrayRef<ConstantInt> values = ArrayRef<ConstantInt>(values_static);//ArrayType::get(IntegerType::get(F.getContext(), 32), num_instructions);
////            uint32_t * num_instr = IntegerType::getInt32PtrTy(F.getContext(), num_instructions);
//
////            uint32_t num_instr = ConstantInt::get(IntegerType::get(F.getContext(),32), num_instructions);
////            uint32_t * keys = ArrayType::get(IntegerType::get(F.getContext(),32), keys_static);
////            uint32_t * values = ArrayType::get(IntegerType::get(F.getContext(),32), values_static);
////            ConstantInt* keys = ConstantInt::get(keys_static[0]);
////            ConstantInt* values = ConstantInt::get(values_static[0]);
//
////            ConstantInt * key_0 = *keys_static;//&keys.front();
////            ConstantInt * value_0 = *values_static;//&values.front();
////            Value* key_0 = Builder.CreatePtrToInt(keys_static[0], Type::getInt32PtrTy(context));
////            Value* key_0 = Builder.CreatePtrToInt(&keys_static[0], Type::getInt32PtrTy(context));
//
////            Value* value_0 = Builder.CreatePtrToInt(values_static[0], Type::getInt32PtrTy(context));
//
////            PointerType * key_ptr = Type::get
////            IRBuilder<> Builder_M(&*Mod.end()->getFirstInsertionPt());
//            std::vector<Value*> args;
////            num_instr = Builder.getIntPtrTy()
//            args.push_back(num_instr);
////            Value* keyPtr = &keys[0];
////            Value* zero = ConstantInt::get(IntegerType::get(context, 32), 0);
////            PointerType* ptrTy = PointerType::get(IntegerType::get(context, 32), 0);
//
////            Constant* beginConstAddress = ConstantInt::get(IntegerType::get(context, 64), &(values_static[0]));
////            Value* beginConstPtr = ConstantExpr::getIntToPtr(
////                    beginConstAddress , PointerType::getUnqual(IntegerType::get(context, 32)));
//// We need an array of index values
////   Note - we need a type for constants, so use someValue's type
////            Value* indexList[2] = {ConstantInt::get(IntegerType::get(context, 32), 0),
////                                   ConstantInt::get(IntegerType::get(context, 32), 0)};
//////
//////
////            Value* gepInst = Builder.CreateGEP(ptrTy,*keys_static, Builder.getInt32(0), "a1");//IRBuilder<>::CreateGEP(keyPtr,ArrayRef<Value*>(indexList, 2), "keyPtr");
////            Value* gepVal = Builder.CreateGEP(*values_static, Builder.getInt32(0), "a2");//IRBuilder<>::CreateGEP(keyPtr,ArrayRef<Value*>(indexList, 2), "keyPtr");
////
////
////            Value* key_0 = Builder.CreatePointerCast(keys, Type::getInt32PtrTy(context));
////            Value* value_0 = Builder.CreatePointerCast(values, Type::getInt32PtrTy(context));
//// Value* key_0 = &keys_static[0];
//
////            GetElementPtrInst::Create(
////            arr, ArrayRef<Value*>(indexList, 2),
////            "arrayIdx",
////                                         <some location to insert>);
//
////GetElementPtrInst::Create(*&arrayTy, *&keys)
//            args.push_back(key_0);
//            args.push_back(value_0);
//
//            Builder.CreateCall(updateInstrInfoFunc, args);
            Builder.CreateCall(printOutInstrInfoFunc);
//            errs() << F.getParent()->getName();


//            updateInstrInfo(num_instructions, keys, values);
//            printOutInstrInfo();

            return true;
        }
    }; // end of struct TestPass

}  // end of anonymous namespace
char CountDynamicInstructions::ID = 1;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "Developed to count dynamic instructions",
                                               false /* Only looks at CFG */,
                                               false /* Analysis Pass */);


