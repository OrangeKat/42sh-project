#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF="bash --posix"
TEST="./../src/42sh"

cat in1 | $REF >"$REF_OUT"
cat in1 | $TEST >"$TEST_OUT"
diff "$REF_OUT" "$TEST_OUT"
DIFF_CODE=$?

rm $REF_OUT
rm $TEST_OUT

exit $DIFF_CODE
