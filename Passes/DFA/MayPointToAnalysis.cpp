//#define NDEBUG

#include "231DFA.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <assert.h>
#include <map>
#include <set>
#include <string>

namespace llvm{


    class MayPointToInfo : public Info {
    public:
        MayPointToInfo() = default;

        MayPointToInfo(const MayPointToInfo &other) = default;

        ~MayPointToInfo() = default;

        std::map<unsigned, std::set<unsigned >> mayPointTo_defs ={};//Edge[space][src]->Edge[space][dst]: (printed by DataFlowAnalysis) point-to-i -> ij / ik/  ...., | point-to-i' ->

        /*
         * Print out the information
         *
         * Direction:
         *   In your subclass you should implement this function according to the project specifications.
         */
        void print() {
//      Edge[space][src]->Edge[space][dst]: (printed by DataFlowAnalysis)


            /*Edge[space][src]->Edge[space][dst]:
             *
             * [point-to 1]|[point-to 2]| ... [point-to K]|\n
             *
where [src] is the index of the instruction that is the start of the edge,
             [dst] is the index of the instruction that is the end of the edge.

             [point-to i] represents what the ith pointer may point to at this moment, and it should be in the following form:
[pointer's DFA ID]->([pointee 1's DFA ID][slash][pointee 2's DFA ID][slash] ... [pointee m's DFA ID][slash])*/

            for(auto pointer = mayPointTo_defs.begin(), end = mayPointTo_defs.end(); pointer != end; pointer++ ) {
                unsigned int pointerID = pointer->first;
                char pointer_prefix = 'R';
                errs() << pointer_prefix << pointerID << "->(";
                for (auto pointeeID: pointer->second) {
                    char pointee_prefix = pointer_prefix;
                    if (pointeeID == pointerID){//alloca
                        pointee_prefix = 'M';
                    }
                    errs() << pointee_prefix << pointeeID << "/";
                }
                errs() << ")|";

            }
            errs() << "\n";
        }

        /*
         * Compare two pieces of information
         *
         * Direction:
         *   In your subclass you need to implement this function.
         */
        static bool equals(MayPointToInfo *info1, MayPointToInfo *info2) {
//      errs() << "rd equals" <<"\n\n";
            for(auto info1_ptr = info1->mayPointTo_defs.begin(), info1_end = info1->mayPointTo_defs.end(); info1_ptr != info1_end; info1_ptr++ ) {

                unsigned int info1_pointerID = info1_ptr->first;
                bool pointer_1_in_info2 =  info2->mayPointTo_defs.find(info1_pointerID) != info2->mayPointTo_defs.end();
                if (!pointer_1_in_info2){
                    return false;
                }
            }

            //compare against values in info2_ptr
            for(auto info2_ptr = info2->mayPointTo_defs.begin(), info2_end = info2->mayPointTo_defs.end(); info2_ptr != info2_end; info2_ptr++ ) {

                unsigned int info2_pointerID = info2_ptr->first;
                bool pointer_2_in_info1 =  info1->mayPointTo_defs.find(info2_pointerID) != info1->mayPointTo_defs.end();
                if (!pointer_2_in_info1){
                    return false;
                }
            }

            for(auto info1_ptr = info1->mayPointTo_defs.begin(), info1_end = info1->mayPointTo_defs.end(); info1_ptr != info1_end; info1_ptr++ ) {

                unsigned int info1_pointerID = info1_ptr->first;
                bool is_equal = info1_ptr->second == info2->mayPointTo_defs[info1_pointerID];
                if (!is_equal){
                    return false;
                }
            }
//      errs() << is_equal <<"\n\n";


            return true;
        }

        /*
         * Join two pieces of information.
         * The third parameter points to the result.
         *
         * Direction:
         *   In your subclass you need to implement this function.
         */
        static void join(MayPointToInfo *info1, MayPointToInfo *info2, MayPointToInfo *result) {
            //union; since they are sets, just insert everything.
            MayPointToInfo *info_in[2] = {info1, info2};
            for (auto curr_info : info_in) {
                if (!equals(curr_info, result)) {//since we sometimes join result with something else and put it back in result
                    for(auto info_ptr = curr_info->mayPointTo_defs.begin(), info_end = info1->mayPointTo_defs.end(); info_ptr != info_end; info_ptr++ ) {
                        unsigned int pointerID = info_ptr->first;
                        result->mayPointTo_defs[pointerID].insert( info_ptr->second.begin(),  info_ptr->second.end());

//                        for (unsigned mayPointToi: info_ptr->second){
//                            result->mayPointTo_defs[pointerID].insert(mayPointToi);
//                        }
                    }
                }
            }


        }
    };
    const bool isForwardDirection = true;
    class MayPointToAnalysis : public DataFlowAnalysis<MayPointToInfo, isForwardDirection> {
    private:
        typedef std::pair<unsigned, unsigned> Edge;

    public:
        MayPointToAnalysis(MayPointToInfo &bottom, MayPointToInfo &initialState) :
                DataFlowAnalysis<MayPointToInfo, isForwardDirection>::DataFlowAnalysis(bottom, initialState) {}



/*When encountering a phi instruction,
 * the flow function should process the series of phi instructions together
 * (effectively a PHI node from the lecture) rather than process each phi instruction individually.
 *
 * This means that the flow function needs to look at the LLVM CFG to:
 * iterate through all the later phi instructions at the beginning of the same basic block
 * until the first non-phi instruction.*/

        /*LLVM IR is in Single Static Assignment (SSA) form,
         * so every LLVM IR variable has exactly one definion.
         *
         * Also, because any LLVM IR instruction that can define a variable (i.e. has a non-void return type)
         * can only define one variable at a time,
         *
         * there is a one-to-one mapping between LLVM IR variables and IR instructions that can define variables.*/

        /*%result = add i32 4, %var
    %result is the variable defined by this add instruction.
         None of any other instructions can redefine %result.

         This means that we can use the index of this add instruction to represent this definition of %result.
         Since a definition can be represented by the index of the defining instruction,
         unlike the reaching analysis taught in class,
         the domain D for this analysis is Powerset(S) where*/

        /* *************
         * S = a set of the indices of all the instructions in the function.
         *
         * bottom = the empty set.
         *
         * top = S.
         *
         * ⊑ = ⊆ ("is subset of").
         *
         * *************/



        /*This analysis works at the LLVM IR level, so operations that the flow functions process are IR instructions.
         * You need to implement them in flowfunction in your subclass of DataFlowAnalysis.
         * There are three categories of IR instructions:

    First Category: IR instructions that return a value (defines a variable):
                 All the instructions under binary operations;
                 llvm::Instruction::isBinaryOp()
                              ALL RETURN
                          Binary Operations
                          'add' Instruction 11
                          'fadd' Instruction
                          'sub' Instruction
                          'fsub' Instruction
                          'mul' Instruction
                          'fmul' Instruction
                          'udiv' Instruction
                          'sdiv' Instruction
                          'fdiv' Instruction
                          'urem' Instruction
                          'srem' Instruction
                          'frem' Instruction22

                                 if 11 <= instruction_index <=22: bin instruction_index

                  All the instructions under binary bitwise operations;
                        Bitwise Binary Operations
                                 ALL RETURN
                          'shl' Instruction 23
                          'lshr' Instruction
                          'ashr' Instruction
                          'and' Instruction
                          'or' Instruction
                          'xor' Instruction 28

                                  if 23 <= instruction_index <= 28: bitwise instruction_index (bin)

                          alloca; 29 returning a pointer to alloced mem
                          load; 30 returns loaded val
                          getelementptr; 32 returns pointer to element

                          icmp; 51 returns bool or vector of bools
                          fcmp; 52 returns bool or vector of bools
                          select. 55 returns the first value argument; otherwise, it returns the second value argument.
                          =>

                          if 11 <= instruction_index <=30 | instruction_index == 32 | 51<= instruction_index <= 52 | instruction_index == 55
                            1st cat

    Second Category: IR instructions that do not return a value (+ misc)
              br; 2 //4? NO RETURN
              switch; 3 NO RETURN
              store; 31 NO RETURN
              => final else

    Third Category: phi instructions
              phi; 53

                if instruction_index == 53

    =>
         if (instruction_index == 53){
              // 3 (phi)
         } elif (11 <= instruction_index <=30 | instruction_index == 32 | 51<= instruction_index <= 52 | instruction_index == 55 ){
              // 1 (return result)
         } else{
              // 2 (non-returning values or non-specified)
         }
    */
        /*For the reaching definition analysis, you only need to consider the following IR instructions:






    Every instruction above falls into one of the three categories.

    If an instruction has multiple outgoing edges, all edges have the same information.

    Any other IR instructions that are not mentioned above should be treated as IR instructions that
    do not return a value (the second categories above).
    */

        /*
        * The flow function.
        *   Instruction I: the IR instruction to be processed.
        *   std::vector<unsigned> & IncomingEdges: the vector of the indices of the source instructions of the incoming edges.
        *   std::vector<unsigned> & OutgoingEdges: the vector of indices of the source instructions of the outgoing edges.
        *   std::vector<Info *> & Infos: the vector of the newly computed information for each outgoing edge.
        *
        * Direction:
        * 	 Implement this function in subclasses.
        */

        virtual void flowfunction(Instruction *I,
                                  std::vector<unsigned> &IncomingEdges,
                                  std::vector<unsigned> &OutgoingEdges,
                                  std::vector<MayPointToInfo *> &Infos) {
            if (I == nullptr)
                return;


            auto InstrToIndex = getInstrToIndex();
            auto EdgeToInfo = getEdgeToInfo();
            auto IndexToInstr = getIndexToInstr();
            unsigned int instr_index = InstrToIndex[I];
            unsigned int instr_opcode = I->getOpcode();

//the first step of any flow function should be joining the incoming data flows.

/////////////////////*join incoming edges*/////////////////////////////

            auto *incoming_mayPointTo_info = new MayPointToInfo();
            for (auto incoming_edge :IncomingEdges) {
                Edge edge = Edge(incoming_edge, instr_index);
                MayPointToInfo *curr_info = EdgeToInfo[edge];
                MayPointToInfo::join(curr_info, incoming_mayPointTo_info, incoming_mayPointTo_info);
            }

            auto *locally_computed_reaching_info = new MayPointToInfo();
//          errs()<<"Instruction " <<instr_opcode << ":\t"<<I->getOpcodeName() << "\n";
//          errs() << "Incoming Edges #: "<<IncomingEdges.size() << "\n";


            /*The DFA identifier of a IR pointer is Ri where 
 * i is the index of the defining IR instruction of this IR pointer (IR variable). 
 * 
 * The DFA identifier of a memory object is Mi where 
 * i is the index of the IR instruction that allocates this memory object. For example, suppose that the following IR instruction's index is 10*/
            //alloca
            if (instr_opcode == 29) {
                //U {Ri -> Mi}
                locally_computed_reaching_info->mayPointTo_defs[instr_index].insert(instr_index);
            }

                //bitcast... to
            else if (instr_opcode == 47) {

                Instruction *value = (Instruction *)I->getOperand(0);
                const bool defined_var = InstrToIndex.find(value) != InstrToIndex.end();

                if (defined_var){
                    unsigned  val_index = InstrToIndex[value];
                    //Rv -> X E in
                    const bool is_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(val_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                    if (is_in_incomingInfo){
                        //U {Ri->X | RV-> X E in}
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[val_index];
                        locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(X.begin(), X.end() );
                    }

                }

            }

                //getelementptr
            else if (instr_opcode == 32) {

                Instruction *ptrval = (Instruction *) ((GetElementPtrInst*)I)->getPointerOperand();

                const bool defined_var = InstrToIndex.find(ptrval) != InstrToIndex.end();

                if (defined_var){
                    unsigned  ptrval_index = InstrToIndex[ptrval];
                    //Rv -> X E in
                    const bool RV_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(ptrval_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                    if (RV_in_incomingInfo){
                        //U {Ri->X | RV-> X E in}
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[ptrval_index];
                        locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(X.begin(), X.end() );
                    }

                }

            }

                //load
            else if (instr_opcode == 30) {
                Instruction *pointer = (Instruction *)((LoadInst *)I)->getPointerOperand();
                const bool defined_var = InstrToIndex.find(pointer) != InstrToIndex.end();

                if (defined_var){
                    unsigned  pointer_index = InstrToIndex[pointer];
                    //Rp -> X E in
                    const bool RP_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(pointer_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();
                    if (RP_in_incomingInfo){
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[pointer_index];
                        std::set<unsigned> Y;
                        for (auto x : X){
                            //X -> Y E in
                            bool X_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(x) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                            if (X_in_incomingInfo){
//                                errs() << x << "\n";

                                std::set<unsigned> y_x = incoming_mayPointTo_info->mayPointTo_defs[x];
                                for (auto y: y_x){
//                                errs() << y << "\n";
                                    if (x != y){//not an alloca result
                                        Y.insert(y_x.begin(), y_x.end());
                                    }
                                }
                               }
                        }
                        for (auto x : X){
                            //X -> Y E in
                            bool X_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(x) !=  incoming_mayPointTo_info->mayPointTo_defs.end();
                           bool X_in_Y = Y.find(x) != Y.end();
                            if (X_in_incomingInfo && X_in_Y){
                                //U{Ri -> Y | Rp -> X E in INTERSECTION X -> Y E in}
                                locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(x);

                            }

                        }


                    }

                }
            }

                //store
            else if (instr_opcode == 31) {
                Instruction *value = (Instruction *)I->getOperand(0);
                Instruction *pointer = (Instruction *)((StoreInst *)I)->getPointerOperand();

                const bool defined_val = InstrToIndex.find(value) != InstrToIndex.end();
                const bool defined_pointer = InstrToIndex.find(pointer) != InstrToIndex.end();

                if (defined_val && defined_pointer){
                    unsigned  pointer_index = InstrToIndex[pointer];
                    unsigned value_index = InstrToIndex[value];
                    //Rv -> X E in INTERSECTION Rp -> Y E in
                    const bool Rp_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(pointer_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();
                    const bool Rv_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(value_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                    if (Rp_in_incomingInfo && Rv_in_incomingInfo){
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[value_index];
                        std::set<unsigned> Y = incoming_mayPointTo_info->mayPointTo_defs[pointer_index];
                        for (auto y : Y){
                                //U{Y -> X | Rv -> X E in INTERSECTION Rp -> Y E in}
                            bool y_in_X = X.find(y) != X.end();
                            if (y_in_X) {
                                locally_computed_reaching_info->mayPointTo_defs[y].insert(y);
                            }

                        }

                    }

                }

            }

                //select
            else if (instr_opcode == 55) {
                //getOperand(0) == condition
                Instruction *val1 = (Instruction *)I->getOperand(1);
                Instruction *val2 = (Instruction *)I->getOperand(2);

                const bool val1_defined_var = InstrToIndex.find(val1) != InstrToIndex.end();

                if (val1_defined_var){
                    unsigned  val1_index = InstrToIndex[val1];

                    //R1 -> X E in
                    const bool val1_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(val1_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                    if (val1_in_incomingInfo){
                        //U {Ri->X | R1-> X E in}
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[val1_index];
                        locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(X.begin(), X.end() );
                    }


                }

                const bool val2_defined_var = InstrToIndex.find(val2) != InstrToIndex.end();
                if (val2_defined_var){
                    unsigned  val2_index = InstrToIndex[val2];
                    //R2 -> X E in
                    const bool val2_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(val2_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();

                    if (val2_in_incomingInfo) {
                        //U {Ri->X | R2-> X E in}
                        std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[val2_index];
                        locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(X.begin(), X.end() );
                    }
                }


            }

                //phi
            else if (instr_opcode == 53) {
                Instruction *curr_instruction = I;
                while (curr_instruction != nullptr && curr_instruction->getOpcode() == 53) {
                    for (int val= 0; val < curr_instruction->getNumOperands(); val++){
                        Instruction *valK = (Instruction *)curr_instruction->getOperand(val);
                        const bool valK_defined_var = InstrToIndex.find(valK) != InstrToIndex.end();
                        if (valK_defined_var){
                            unsigned int valK_index = InstrToIndex[valK];
                            const bool valK_in_incomingInfo = incoming_mayPointTo_info->mayPointTo_defs.find(valK_index) !=  incoming_mayPointTo_info->mayPointTo_defs.end();
                            if (valK_in_incomingInfo){
                                //U {Ri -> X | Rk -> X E in}
                                std::set<unsigned> X = incoming_mayPointTo_info->mayPointTo_defs[valK_index];
                                locally_computed_reaching_info->mayPointTo_defs[instr_index] .insert(X.begin(), X.end() );
                            }
                        }


                    }

                    curr_instruction = curr_instruction->getNextNode();

//        incoming_reaching_info->print();
                }//end while

            }

                //any other
            else{
                //out = in

            }
            //final reaching info
            MayPointToInfo::join(locally_computed_reaching_info, incoming_mayPointTo_info, incoming_mayPointTo_info);
//      errs() << "assigning new infos" <<"\n";

            //set new outgoing infos; each outgoing edge has the same info
            for (unsigned int i = 0; i < OutgoingEdges.size(); ++i) {
                MayPointToInfo * mayPointTo_info = new MayPointToInfo();
                mayPointTo_info->mayPointTo_defs = incoming_mayPointTo_info->mayPointTo_defs;
//        incoming_reaching_info->print();
                Infos.push_back(mayPointTo_info);
            }

//      errs() << "assigned infos"<<"\n";
//          errs() << "info out size: "<< Infos.size() <<"\n";

            delete locally_computed_reaching_info;//dealloc
            delete incoming_mayPointTo_info;
        }

    };


    namespace {
        struct MayPointToAnalysisPass : public FunctionPass {
            static char ID;

            MayPointToAnalysisPass() : FunctionPass(ID) {}

            bool runOnFunction(Function &F) override {
                MayPointToInfo bottom;
                MayPointToInfo initial_state;
                MayPointToAnalysis  analysis (bottom, initial_state);//
                analysis.runWorklistAlgorithm(&F);
                analysis.print();
                return false;
            }
        }; // end of struct TestPass
    }  // end of anonymous namespace

    char MayPointToAnalysisPass::ID = 0;
    static RegisterPass<MayPointToAnalysisPass> X("cse231-maypointto", "Developed to determine the may-point-to sets of LLVM IR instructions",
                                                false /* Only looks at CFG */,
                                                false /* Analysis Pass */);

}