#!/bin/sh

REF="bash --posix"
TEST="./../src/42sh"

$REF -c "true"
REF_CODE=$?
$TEST -c "true"
TEST_CODE=$?

[ $REF_CODE -eq $TEST_CODE ]
exit $?
