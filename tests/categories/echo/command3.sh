#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF="bash --posix"
TEST="./../src/42sh"

$REF -c "echo -ne 'foo\tbar'" >"$REF_OUT"
$TEST -c "echo -ne 'foo\tbar'" >"$TEST_OUT"
diff "$REF_OUT" "$TEST_OUT"
DIFF_CODE=$?

rm $REF_OUT
rm $TEST_OUT

exit $DIFF_CODE
