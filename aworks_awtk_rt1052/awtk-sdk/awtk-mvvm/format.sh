find src demos tests -name \*.c -exec clang-format -i {} \;
find src demos tests -name \*.h -exec clang-format -i {} \;
find src demos tests -name \*.cc -exec clang-format -i {} \;
find src demos tests -name \*.cpp -exec clang-format -i {} \;
find src demos tests -name \*.inc  -exec clang-format -i {} \;

