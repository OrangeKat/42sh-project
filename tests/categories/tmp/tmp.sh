#!/bin/bash

run_test() {
	local test_script=$1
	local description=$2
	echo "Running test: $description"
	echo "Test script: $test_script"

	bash_out=$(bash -c "$test_script")
	bash_exit_code=$?

	_42sh_out=$(./../src/a.out -c "$test_script")
	_42sh_exit_code=$?

	if [ "$bash_out" == "$_42sh_out" ]; then
		echo "PASS: Outputs are identical."
	else
		echo "FAIL: Outputs differ."
		echo "bash: $bash_out"
		echo "42sh: $_42sh_out"
	fi

	if [ $bash_exit_code -eq $_42sh_exit_code ]; then
		echo "PASS: Exit codes match."
	else
		echo "FAIL: Exit codes differ."
		echo "bash: $bash_exit_code"
		echo "42sh: $_42sh_exit_code"
	fi

	echo "-------------------------"
}

runtest "echo hello"

echo "All tests completed."
