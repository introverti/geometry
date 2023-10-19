#!/usr/bin/env bash
set -e
REPORT_DIR="scanner_report"

# static check
cppcheck -itest include src \
  --suppress=missingIncludeSystem . \
  --std=c++20 \
  -v --enable=all -I include src --xml 2>.reports/cppcheck.xml

# build
rm -rf build
mkdir -p build && cd build

# ctest
CTEST_REPORT_PATH="../.reports/ctest.xml"
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report
cmake .. -DENABLE_COVERAGE=true
make
ctest --output-junit ${CTEST_REPORT_PATH}

# gcovr
cd ..
gcovr -r . --xml >.reports/coverage.xml
