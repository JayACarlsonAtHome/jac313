add_executable(jac313_jtext_v002_test v002_test.cpp)
target_link_libraries(jac313_jtext_v002_test PRIVATE jac313::jText)
add_test(NAME jac313_jtext_v002_test COMMAND jac313_jtext_v002_test)

# Negative / hardening tests for the line + structure parser and validator
# (ported from the standalone jText repo; exercises the internal jtext:: layer).
add_executable(jac313_jtext_v002_parser_validator_test parser_validator_test.cpp)
target_link_libraries(jac313_jtext_v002_parser_validator_test PRIVATE jac313::jText)
add_test(NAME jac313_jtext_v002_parser_validator_test COMMAND jac313_jtext_v002_parser_validator_test)

# Calling the writer out of order terminates (contracts; ported from the standalone jText repo).
add_executable(jac313_jtext_v002_contracts_test contracts_test.cpp)
target_link_libraries(jac313_jtext_v002_contracts_test PRIVATE jac313::jText)
add_test(NAME jac313_jtext_v002_contracts_test COMMAND jac313_jtext_v002_contracts_test)
