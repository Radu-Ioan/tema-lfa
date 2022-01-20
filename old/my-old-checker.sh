make

function test_accessible {
    cp in/accessible.in .
    make run problem=accessible

    mv accessible.out out/
    diff -q -Z out/accessible.out ref/accessible.ref > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        printf "accessible FAILED\n"
    else
        printf "accessible PASSED\n"
    fi

    rm accessible.in
}

function test_productive {
    cp in/productive.in .
    make run problem=productive

    mv productive.out out/
    diff -q -Z out/productive.out ref/productive.ref > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        printf "productive FAILED\n"
    else
        printf "productive PASSED\n"
    fi

    rm productive.in
}

function test_utils {
    cp in/utils.in .
    make run problem=utils

    mv utils.out out/
    diff -q -Z out/utils.out ref/utils.ref > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        printf "utils FAILED\n"
    else
        printf "utils PASSED\n"
    fi

    rm utils.in
}

test_accessible
# test_productive
# test_utils
