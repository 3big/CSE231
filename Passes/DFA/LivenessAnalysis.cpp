//#define NDEBUG

#include "231DFA.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <assert.h>
#include <map>
#include <set>

namespace llvm{


    class MayPointToInfo : public Info {
    public:
        MayPointToInfo() = default;

        MayPointToInfo(const MayPointToInfo &other) = default;

        ~MayPointToInfo() = default;

        std::set<unsigned> liveness_defs ={};

        /*
         * Print out the information
         *
         * Direction:
         *   In your subclass you should implement this function according to the project specifications.
         */
        void print() {
//      Edge[space][src]->Edge[space][dst]: (printed by DataFlowAnalysis)


//          [def 1]|[def 2]| ... [def K]|\n
            for (auto def: liveness_defs) {
                errs() << def << "|";
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

            bool is_equal = info1->liveness_defs == info2->liveness_defs;
//      errs() << is_equal <<"\n\n";


            return is_equal;
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
                    for (unsigned reaching_def : curr_info->liveness_defs) {
                        result->liveness_defs.insert(reaching_def);
                    }
                }
            }


        }
    };
    bool isForwardDirection = false;//is backward
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

            auto *incoming_reaching_info = new MayPointToInfo();
            for (auto incoming_edge :IncomingEdges) {
                Edge edge = Edge(incoming_edge, instr_index);
                MayPointToInfo *curr_info = EdgeToInfo[edge];
                MayPointToInfo::join(curr_info, incoming_reaching_info, incoming_reaching_info);
            }

            auto *locally_computed_reaching_info = new MayPointToInfo();
//          errs()<<"Instruction " <<instr_opcode << ":\t"<<I->getOpcodeName() << "\n";
//          errs() << "Incoming Edges #: "<<IncomingEdges.size() << "\n";

////////////////////// 3 (phi)////////////////////////////////////////

            if (instr_opcode == 53) {

                //local copy of in[0] U... ... U in[k]
                locally_computed_reaching_info->liveness_defs = incoming_reaching_info->liveness_defs;

//        errs() << "Phi Node" <<"\n";


                //-{result_i | i in [1..x]}
                Instruction *curr_instruction = I;
                while (curr_instruction != nullptr && curr_instruction->getOpcode() == 53) {
//          errs() << "Phi Node: " <<InstrToIndex[curr_instruction] <<"\n";

                    //-result_i
                    locally_computed_reaching_info->liveness_defs.erase(InstrToIndex[curr_instruction]);
                    curr_instruction = curr_instruction->getNextNode();
                }
//            errs() << "Phi Nodes #: " << locally_computed_reaching_info->liveness_defs.size() <<"\n";


                //iterate again


                //out[k] = ...  U {ValuetoInstr_v_ij) | label k == label_ij}

                    for (unsigned int i = 0; i < OutgoingEdges.size(); ++i) {
                        //                        out[k]
                        MayPointToInfo * liveness_info = new MayPointToInfo();
                        liveness_info->liveness_defs = locally_computed_reaching_info->liveness_defs;

                        curr_instruction = I;
                        while (curr_instruction != nullptr && curr_instruction->getOpcode() == 53) {

                            /*Here we include all incoming facts like before and
                             * exclude variables defined at each phi instruction.
                             *
                             * In addition,
                             * each outgoing set out[k] contains those phi variables v_ij that are defined in its matching basic block (labeled with label_k).
                             *
                             * The function ValueToInstr is merely used to extract the defining instruction from each phi value.*/

/*
                         The pair [<v_11>, label_11], for example, guarantees that <v_11> comes from the block labeled with label_11.

                        final reaching info
                        U operands*/
                        //label k
                        unsigned int out_instr_index = OutgoingEdges[i];
                        Instruction *out_instr = IndexToInstr[out_instr_index];

                        //label_ij
                        for (auto op = curr_instruction->operands().begin(), end = curr_instruction->operands().end(); op !=end; ++op){

                            const bool defined_var = InstrToIndex.find(instr_index) !=InstrToIndex.end();
                            const bool same_building_block = out_instr->getParent() == op->getParent();

                            //U {ValuetoInstr_v_ij) | label k == label_ij}
                            if (defined_var && same_building_block){
                                liveness_info->liveness_defs.insert(InstrToIndex[op]);
                            }

                        }//end for
                        curr_instruction = curr_instruction->getNextNode();

//        incoming_reaching_info->print();
                    }//end while
                        Infos.push_back(liveness_info);
                    }//end for


            }





//////////////////////1 (returns result)///////////////////////////////

                /*... U operands - {index } where
                 * operands is the set of variables used (and therefore needed to be live) in the body of the instruction,
                 * index is the index of the IR instruction, which corresponds to the variable <result> being defined.
                 *
                 * For example, in the instruction
                 *
                 * %result = add i32 4,  %var
                 *
                 * operands would be the singleton set { I%var },
                 * where I%var is the index of the instruction that defines %var.*/

            else if ((11 <= instr_opcode && instr_opcode <= 30) //{bin}, {bitwise}, alloc, load
                       | (instr_opcode == 32) //getelementptr
                       | (51 <= instr_opcode && instr_opcode <= 52) //icmp, fcmp
                       | (instr_opcode == 55) //select
                    ) {



//        errs() << "Return Result" <<"\n";

                //U operands
                for (auto op = I->operands().begin(), end = I->operands().end(); op !=end; ++op){
                    const bool defined_var = InstrToIndex.find(instr_index) !=InstrToIndex.end();
                    if (defined_var){
                        locally_computed_reaching_info->liveness_defs.insert(InstrToIndex[op]);
                    }
                }
                //-{index}
                locally_computed_reaching_info->liveness_defs.erase(instr_index);//single instruction

                //final reaching info
                MayPointToInfo::join(locally_computed_reaching_info, incoming_reaching_info, incoming_reaching_info);
//      errs() << "assigning new infos" <<"\n";

                //set new outgoing infos; each outgoing edge has the same info
                for (unsigned int i = 0; i < OutgoingEdges.size(); ++i) {
                    MayPointToInfo * reaching_info = new MayPointToInfo();
                    reaching_info->liveness_defs = incoming_reaching_info->liveness_defs;
//        incoming_reaching_info->print();
                    Infos.push_back(reaching_info);
                }

//      errs() << "assigned infos"<<"\n";
//          errs() << "info out size: "<< Infos.size() <<"\n";
            }


//////////////// 2 (non-returning values or non-specified)///////////////////
/*... U operands*/
            else {
//        errs() << "NO Result" << "\n";

                //U operands
                for (auto op = I->operands().begin(), end = I->operands().end(); op !=end; ++op){
                    const bool defined_var = InstrToIndex.find(instr_index) !=InstrToIndex.end();
                    if (defined_var){
                        locally_computed_reaching_info->liveness_defs.insert(InstrToIndex[op]);
                    }
                }

                //final reaching info
                MayPointToInfo::join(locally_computed_reaching_info, incoming_reaching_info, incoming_reaching_info);
//      errs() << "assigning new infos" <<"\n";

                //set new outgoing infos; each outgoing edge has the same info
                for (unsigned int i = 0; i < OutgoingEdges.size(); ++i) {
                    MayPointToInfo * reaching_info = new MayPointToInfo();
                    reaching_info->liveness_defs = incoming_reaching_info->liveness_defs;
//        incoming_reaching_info->print();
                    Infos.push_back(reaching_info);
                }

//      errs() << "assigned infos"<<"\n";
//          errs() << "info out size: "<< Infos.size() <<"\n";
            }

//////////////////////////////////////////////////////////////////

            delete locally_computed_reaching_info;//dealloc
            delete incoming_reaching_info;

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
    static RegisterPass<MayPointToAnalysisPass> X("cse231-liveness", "Developed to determine liveness of IR instructions",
                                                          false /* Only looks at CFG */,
                                                          false /* Analysis Pass */);

}