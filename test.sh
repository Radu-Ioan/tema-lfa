#!/bin/bash
# credits to AI CG :D

SCRIPT_DIR=`realpath $(dirname $0)`
CHECKER_DIR=${SCRIPT_DIR}/checker
TEST_DIR=${CHECKER_DIR}/tests
LOG_DIR=${CHECKER_DIR}/logs
INPUT_FILE=${SCRIPT_DIR}/"dfa"

min() {
    awk -v n1="$1" -v n2="$2" 'BEGIN {printf "%s\n",(n1<n2?n1:n2)}'
}

max() {
    awk -v n1="$1" -v n2="$2" 'BEGIN {printf "%s\n",(n1<n2?n2:n1)}'
}

run_tests (){
    while read test_case
    do
        input="$(echo $test_case | cut -d' ' -f 1)"
        problem="$(echo $test_case | cut -d' ' -f 2)"
        weight="$(echo $test_case | cut -d' ' -f 3)"
        output=${TEST_DIR}/${input/.in/.out}
        OUTPUT_FILE=${SCRIPT_DIR}/${input}.out
        CLEARED_OUTPUT=${OUTPUT_FILE}.clear
        SYNC_OUTPUT=${OUTPUT_FILE}.seq

        if [ -f mytime.cfg ]
        then
            time_pref=`grep ^${input} mytime.cfg | cut -d ' ' -f 2`
        fi
        time=${time_pref:-${time_test}}
        time=`min ${time} ${TIME_LEFT}`

        rm ${INPUT_FILE} ${OUTPUT_FILE} ${CLEARED_OUTPUT} ${SYNC_OUTPUT} &> /dev/null
        cp ${TEST_DIR}/${input} $INPUT_FILE

        total_score=$[total_score + $weight]

        if [[ "$time" = "0" ]]
        then
            echo -e "Skipping \e[1;33m${input}\e[0m."
            continue
        fi

        echo "Time allowed $time"

        # Run the student homework
        START=$(date +%s.%N)

        if [ $problem = labyrinth ]
        then
            echo "Running: make -s labyrinth < ${input} > ${OUTPUT_FILE} 2> /dev/null"
            timeout $time make -s labyrinth < ${INPUT_FILE} > ${OUTPUT_FILE}
            if [ $? != 0 ]
            then
                echo -e "\e[31mFAILED\e[0m Test $\e[1;33m${input}\e[0m. You failed to win: $weight"
                echo "Failed converting labyrinth"
                continue
            fi
            cat "${OUTPUT_FILE}" | grep -vi make > "$CLEARED_OUTPUT"
            rm ${OUTPUT_FILE} ${INPUT_FILE} 2> /dev/null
            mv ${CLEARED_OUTPUT} ${INPUT_FILE}
        fi

        echo "Running: make -s run ${problem/labyrinth/synchronize} < ${input} > ${OUTPUT_FILE} 2> /dev/null"
        timeout $time make -s run problem=${problem/labyrinth/synchronize} < ${INPUT_FILE} > ${OUTPUT_FILE} 2> /dev/null
        timeret=$?
        END=$(date +%s.%N)
        DIFF=$(echo "$END - $START" | bc)

        TIME_LEFT=$(max `echo "$TIME_LEFT - $DIFF" | bc` 0)

        if [ $timeret = 124 ]
        then
            echo -e "\e[31mFAILED\e[0m Test \e[1;33m${input}\e[0m. You failed to win: $weight"
            echo "make -s run problem=$problem < ${input} > ${OUTPUT_FILE} 2> /dev/null" > ${LOG_DIR}/${input}.command
            echo "Timeout $DIFF/$time"
            rm ${OUTPUT_FILE} 2>/dev/null
        else
            if [ $problem = synchronize ]
            then
                cat "${OUTPUT_FILE}" | grep -vi Makefile > "$CLEARED_OUTPUT"
                make -s -C ${CHECKER_DIR} -f Makefile.checker run dfa=${INPUT_FILE} sequence=${CLEARED_OUTPUT}
                outcmp=$?
            elif [ $problem = labyrinth ]
            then
                cat "${OUTPUT_FILE}" | grep -vi Makefile > "$CLEARED_OUTPUT"
                make -s -C ${CHECKER_DIR} -f Makefile.checker run dfa=${TEST_DIR}/${input}.dfa sequence=${CLEARED_OUTPUT}
                outcmp=$?
            else
                cat ${OUTPUT_FILE} | grep -vi Makefile | sort -n > "$CLEARED_OUTPUT"
                diff -bBq "$CLEARED_OUTPUT" "${output}" &> /dev/null
                outcmp=$?
            fi

            if [ "$outcmp" = "0" ]
            then
                echo -e "\e[32mPASSED\e[0m Test \e[1;33m${input}\e[0m. You won: $weight"
                rm ${OUTPUT_FILE} ${CLEARED_OUTPUT} 2>/dev/null
                score=$[$score + $weight];
            else
                echo -e "\e[31mFAILED\e[0m Test $\e[1;33m${input}\e[0m. You failed to win: $weight"
                mv ${OUTPUT_FILE} ${CLEARED_OUTPUT} ${LOG_DIR}
                echo "make -s run problem=${problem} < ${TEST_DIR}/${input} > $OUTPUT_FILE" > ${LOG_DIR}/$test_name.command
                echo "failed outputs saved in ${LOG_DIR}"
                echo "Output comparison: ${outcmp}, expected 0"
            fi
        fi

        echo "Time left ${TIME_LEFT}"
    done < ${TEST_DIR}/tests
}

make -C ${CHECKER_DIR} -f Makefile.checker build
# Compile student homework
make build
mkdir -p ${LOG_DIR}
rm ${LOG_DIR}/* 2>/dev/null

echo "Starting"

# Run tests
score=0
total_score=0

if [[ `make run -n problem=x` =~ .*python.* ]]
then
    TIME_LEFT="300"
    time_test="40"
else
    TIME_LEFT="150"
    time_test="20"
fi

run_tests

rm $INPUT_FILE &> /dev/null
#make clean

echo "Score: $score out of $total_score"
