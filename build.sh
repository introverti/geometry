#!/usr/bin/env bash
set -e





# ctest and gcovr
rm -rf build
mkdir -p build && cd build
CTEST_REPORT_PATH="../.reports/ctest.xml"
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report
cmake .. -DENABLE_COVERAGE=true
make
ctest --output-junit ${CTEST_REPORT_PATH}
cd ..
gcovr -r . --xml >.reports/coverage.xml

