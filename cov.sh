#!/usr/bin/env bash
set -e

cd build
CTEST_REPORT_PATH="../.reports/ctest.xml"
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report

cmake .. -DENABLE_COVERAGE=true
make

ctest -VV --output-junit ${CTEST_REPORT_PATH}
# lcov --rc lcov_branch_coverage=1 -c -d . -o ${COVERAGE_FILE}
# lcov --rc lcov_branch_coverage=1 -e ${COVERAGE_FILE}_tmp "*autolabel/modules*" -o ${COVERAGE_FILE}
# genhtml --rc genhtml_branch_coverage=1 --highlight --legend ${COVERAGE_FILE} -o ${REPORT_FOLDER}
gcovr -r . --xml >../.reports/coverage.xml
gcovr -r . --xml >.reports/coverage.xml