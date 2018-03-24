#!/bin/bash

# path to clang++, llvm-dis, and opt
LLVM_BIN=/LLVM_ROOT/build/bin
# path to CSE231.so
LLVM_SO=/LLVM_ROOT/build/lib
# path to lib231.c
LIB_DIR=/lib231
# path to the test directory
TEST_DIR=.

# paths to the LLVM test directories
LLVM_TEST_DIR=/LLVM_ROOT/llvm/test/Transforms/SimplifyCFG

LIVE_TEST_DIR=/LLVM_ROOT/llvm/test/Transforms/MemCpyOpt/


#build the project
make -f /LLVM_ROOT/build/lib/Transforms/CSE231_Project/Makefile

#DEPRECATED

#generate test program LLVM IR
# $LLVM_BIN/clang -c -O0 $TEST_DIR/test1.c -emit-llvm -S -o /tmp/test1-c.ll
# $LLVM_BIN/clang++ -c -O0 $TEST_DIR/test1.cpp -emit-llvm -S -o /tmp/test1.ll
# $LLVM_BIN/clang++ -c $LIB_DIR/lib231.cpp -emit-llvm -S -o /tmp/lib231.ll
# $LLVM_BIN/clang++ -c $TEST_DIR/test1-main.cpp -emit-llvm -S -o /tmp/test1-main.ll


#test output against known solutions
find $LIVE_TEST_DIR -name "*.ll"  -print | while read f; do
#maypointto
	$LLVM_BIN/opt -load $LLVM_SO/CSE231_maypointto.so -cse231-maypointto < "$f" > /dev/null 2> /tmp/mpt.result
	/solution/opt -cse231-maypointto < "$f" > /dev/null 2> /tmp/mpt.result.sol
#liveness
	$LLVM_BIN/opt -load $LLVM_SO/CSE231_maypointto.so -cse231-liveness < "$f" > /dev/null 2> /tmp/liveness.result
	/solution/opt -cse231-liveness < "$f"   > /dev/null 2> /tmp/liveness.result.sol
L_DIFF=$(diff /tmp/liveness.result /tmp/liveness.result.sol) 
M_DIFF=$(diff /tmp/mpt.result /tmp/mpt.result.sol) 

if [ "$L_DIFF" != "" ] 
then
	echo "$f"
	    
    echo '********LIVENESS LL*********'
    cat "$f"
    
	echo '********ORIG*********'
    cat /tmp/liveness.result
    
    echo '********SOLN *********'
    cat /tmp/liveness.result.sol

    
    echo '**********LIVENESS DIFF *************'
	echo "$L_DIFF"
fi

if [ "$M_DIFF" != "" ] 
then
	echo "$f"
	
    echo '********MPT LL*********'
    cat "$f"
    
    echo '**********MPT DIFF *************'
	echo "$M_DIFF"
fi

	
done

find $LLVM_TEST_DIR -name "*.ll"  -print | while read f; do
#maypointto
	$LLVM_BIN/opt -load $LLVM_SO/CSE231_maypointto.so -cse231-maypointto < "$f" > /dev/null 2> /tmp/mpt.result
	/solution/opt -cse231-maypointto < "$f" > /dev/null 2> /tmp/mpt.result.sol
#liveness
	$LLVM_BIN/opt -load $LLVM_SO/CSE231_maypointto.so -cse231-liveness < "$f" > /dev/null 2> /tmp/liveness.result
	/solution/opt -cse231-liveness < "$f"   > /dev/null 2> /tmp/liveness.result.sol
L_DIFF=$(diff /tmp/liveness.result /tmp/liveness.result.sol) 
M_DIFF=$(diff /tmp/mpt.result /tmp/mpt.result.sol) 

if [ "$L_DIFF" != "" ] 
then
	echo "$f"
	    
    echo '********LIVENESS LL*********'
    cat "$f"
    
	echo '********ORIG*********'
    cat /tmp/liveness.result
    
    echo '********SOLN *********'
    cat /tmp/liveness.result.sol

    
    echo '**********LIVENESS DIFF *************'
	echo "$L_DIFF"
fi

if [ "$M_DIFF" != "" ] 
then
	echo "$f"
	
    echo '********MPT LL*********'
    cat "$f"
    
    echo '**********MPT DIFF *************'
	echo "$M_DIFF"
fi

	
done