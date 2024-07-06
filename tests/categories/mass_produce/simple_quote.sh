#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF_ERR=".refstd.err"
TEST_ERR=".teststd.err"
REF="bash --posix"
TEST="./../src/42sh"

COMMAND="echo 'simple quote'"
echo $COMMAND >in

cat in | $REF >"$REF_OUT" 2>"$REF_ERR"
REF_CODE= $?
cat in | $TEST >"$TEST_OUT" 2>"$TEST_ERR"
TEST_CODE= $?
diff "$REF_OUT" "$TEST_OUT"
DIFF_CODE=$?

if [ $REF_CODE -ne $TEST_CODE ]; then
	echo "wrong return code"
fi

if [ $DIFF_CODE -ne 0 ]; then
	echo "wrong stdout"
fi

diff "$REF_ERR" "$TEST_ERR"
ERR_CODE=$?

if [ $ERR_CODE -ne 0 ]; then
	echo "wrong stderr"
fi

rm $REF_OUT
rm $TEST_OUT
rm $REF_ERR
rm $TEST_ERR
rm in

exit $DIFF_CODE && $ERR_CODE && [ $REF_CODE -eq $TEST_CODE]
