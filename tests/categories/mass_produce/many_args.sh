#!/bin/sh

REF_OUT=".refstd.out"
TEST_OUT=".teststd.out"
REF_ERR=".refstd.err"
TEST_ERR=".teststd.err"
REF="bash --posix"
TEST="./../src/42sh"

COMMAND='echo Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Egestas purus viverra accumsan in nisl nisi. At quis risus sed vulputate. Neque laoreet suspendisse interdum consectetur libero. Et molestie ac feugiat sed lectus vestibulum mattis. Tristique nulla aliquet enim tortor at auctor. Aliquet porttitor lacus luctus accumsan tortor. Tellus cras adipiscing enim eu turpis. Mattis ullamcorper velit sed ullamcorper morbi tincidunt ornare. Nec sagittis aliquam malesuada bibendum arcu vitae elementum. Consequat id porta nibh venenatis cras. Dolor magna eget est lorem ipsum. Vivamus at augue eget arcu dictum varius duis. Aliquam eleifend mi in nulla. Cursus risus at ultrices mi tempus imperdiet nulla malesuada. Adipiscing elit ut aliquam purus sit amet luctus venenatis. Risus at ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Mauris cursus mattis molestie a iaculis at erat pellentesque adipiscing.'
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
