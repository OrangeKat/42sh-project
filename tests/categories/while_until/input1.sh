#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF_ERR=".refstd.err"
TEST_ERR=".teststd.err"
REF="bash --posix"
TEST="./../src/42sh"

cat in1 | $REF >"$REF_OUT" 2>"$REF_ERR"
cat in1 | $TEST >"$TEST_OUT" 2>"$TEST_ERR"
diff "$REF_OUT" "$TEST_OUT"
DIFF_CODE=$?

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

exit $DIFF_CODE && $ERR_CODE
