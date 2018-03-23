#define NDEBUG
#include "231DFA.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>

namespace llvm{


class ReachingInfo : public Info {
public:
    ReachingInfo() = default;

    ReachingInfo(const ReachingInfo &other) = default;

    ~ReachingInfo() = default;

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
    static bool equals(ReachingInfo *info1, ReachingInfo *info2) {


      bool is_equal = info1->liveness_defs == info2->liveness_defs;



      return is_equal;
    }

    /*
     * Join two pieces of information.
     * The third parameter points to the result.
     *
     * Direction:
     *   In your subclass you need to implement this function.
     */
    static void join(ReachingInfo *info1, ReachingInfo *info2, ReachingInfo *result) {
      //union; since they are sets, just insert everything.
      ReachingInfo *info_in[2] = {info1, info2};
      for (auto curr_info : info_in) {
        if (!equals(curr_info, result)) {//since we sometimes join result with something else and put it back in result
          for (unsigned reaching_def : curr_info->liveness_defs) {
            result->liveness_defs.insert(reaching_def);
          }
        }
      }


    }
};

class ReachingDefinitionAnalysis : public DataFlowAnalysis<ReachingInfo, true> {
private:
    typedef std::pair<unsigned, unsigned> Edge;

public:
    ReachingDefinitionAnalysis(ReachingInfo &bottom, ReachingInfo &initialState) :
            DataFlowAnalysis<ReachingInfo, true>::DataFlowAnalysis(bottom, initialState) {}


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
                  All the instructions under binary bitwise operations;

    Second Category: IR instructions that do not return a value (+ misc)
    Third Category: phi instructions

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
                      std::vector<ReachingInfo *> &Infos) {
      if (I == nullptr)
        return;


      auto InstrToIndex = getInstrToIndex();
      auto EdgeToInfo = getEdgeToInfo();
      unsigned int instr_index = InstrToIndex[I];
      unsigned int instr_opcode = I->getOpcode();

//the first step of any flow function should be joining the incoming data flows.

      //join incoming edges
      auto *incoming_reaching_info = new ReachingInfo();

      for (auto incoming_edge :IncomingEdges) {
        Edge edge = Edge(incoming_edge, instr_index);
        ReachingInfo *curr_info = EdgeToInfo[edge];
        ReachingInfo::join(curr_info, incoming_reaching_info, incoming_reaching_info);
      }

      auto *locally_computed_reaching_info = new ReachingInfo();



          if (instr_opcode == 53) {
        // 3 (phi)
        //union
        Instruction *curr_instruction = I;

        while (curr_instruction != nullptr && curr_instruction->getOpcode() == 53) {
          //store phi block in a temp reaching info


          locally_computed_reaching_info->liveness_defs.insert(InstrToIndex[curr_instruction]);
          curr_instruction = curr_instruction->getNextNode();
        }



      } else if ((11 <= instr_opcode && instr_opcode <= 30) //{bin}, {bitwise}, alloc, load
                 | (instr_opcode == 32) //getelementptr
                 | (51 <= instr_opcode && instr_opcode <= 52) //icmp, fcmp
                 | (instr_opcode == 55) //select
              ) {
        // 1 (returns result)

        locally_computed_reaching_info->liveness_defs.insert(instr_index);//single instruction
      } else {


        // 2 (non-returning values or non-specified)
        //out = in; do nothing
      }

      //final reaching info
      ReachingInfo::join(locally_computed_reaching_info, incoming_reaching_info, incoming_reaching_info);


      //set new outgoing infos; each outgoing edge has the same info
      for (unsigned int i = 0; i < OutgoingEdges.size(); ++i) {
        ReachingInfo * reaching_info = new ReachingInfo();
        reaching_info->liveness_defs = incoming_reaching_info->liveness_defs;

        Infos.push_back(reaching_info);
      }




      delete locally_computed_reaching_info;//dealloc
      delete incoming_reaching_info;

    }

};


namespace {
    struct ReachingDefinitionAnalysisPass : public FunctionPass {
        static char ID;

        ReachingDefinitionAnalysisPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
          ReachingInfo bottom;
          ReachingInfo initial_state;
          ReachingDefinitionAnalysis  analysis (bottom, initial_state);
          analysis.runWorklistAlgorithm(&F);
          analysis.print();
          return false;
        }
    }; // end of struct ReachingDefinitionAnalysis
}  // end of anonymous namespace

char ReachingDefinitionAnalysisPass::ID = 0;
static RegisterPass<ReachingDefinitionAnalysisPass> X("cse231-reaching", "Developed to determine reaching definitions",
                                                      false /* Only looks at CFG */,
                                                      false /* Analysis Pass */);

}