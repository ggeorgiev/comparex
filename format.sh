bazel run //:format_Starlark_with_buildifier
find . -iname '*.hpp' -exec clang-format -i {} \;
find . -iname '*.cc' -exec clang-format -i {} \;
bazel run //:refresh_compile_commands
