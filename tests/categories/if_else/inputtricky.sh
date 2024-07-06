#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF="bash --posix"
TEST="./../src/42sh"

cat intricky | $REF >"$REF_OUT"
cat intricky | $TEST >"$TEST_OUT"
diff "$REF_OUT" "$TEST_OUT"
DIFF_CODE=$?

rm $REF_OUT
rm $TEST_OUT

exit $DIFF_CODE
