#!/bin/sh

REF="bash --posix"
TEST="./../src/42sh"

$REF -c "false"
REF_CODE=$?
$TEST -c "false"
TEST_CODE=$?

[ $REF_CODE -eq $TEST_CODE ]
exit $?
