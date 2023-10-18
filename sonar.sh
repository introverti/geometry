#!/usr/bin/env bash
set -e

# static check
mkdir -p .reports
mkdir -p .logs

# https://stackoverflow.com/questions/6986033/cppcheck-cant-find-include-files
# --include= \
cppcheck -itest include src \
  --suppress=missingIncludeSystem . \
  --std=c++20 \
  -v --enable=all -I include src --xml 2>.reports/cppcheck.xml

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

# sonar 9.9.1
/home/xavier/Codes/sonar-scanner-5.0.1.3006-linux/bin/sonar-scanner -X \
  -Dsonar.projectKey=Geometry \
  -Dsonar.sources=include,src \
  -Dsonar.host.url=http://localhost:9000 \
  -Dsonar.login=sqp_a9bafa6a29e9ebc4e409dce3d915d942375b5a84 \
  &>.logs/sonar-scanner.log

# clean up
rm -rf .coverage
rm -rf .pytest_cache
rm -rf .reports
rm -rf .logs
rm -rf .scannerwork
