workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "d2ba494a40f51064690bbb3e50ee6246803d6ad87dffba903760ff94bc87effc",
    strip_prefix = "bazel-toolchain-795d76fd03e0b17c0961f0981a8512a00cba4fa2",
    url = "https://github.com/grailbio/bazel-toolchain/archive/795d76fd03e0b17c0961f0981a8512a00cba4fa2.zip",
)

load("@com_grail_bazel_toolchain//toolchain:deps.bzl", "bazel_toolchain_dependencies")

bazel_toolchain_dependencies()

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    cxx_standard = {"linux": "c++20"},
    distribution = "clang+llvm-15.0.6-x86_64-linux-gnu-ubuntu-18.04.tar.xz",
    llvm_version = "15.0.6",
)

load("@llvm_toolchain//:toolchains.bzl", "llvm_register_toolchains")

llvm_register_toolchains()

load("//:repositories.bzl", "ecsact_rtb_repositories")

ecsact_rtb_repositories()

load("//:workspace.bzl", "ecsact_rtb_workspace")

ecsact_rtb_workspace()

http_archive(
    name = "hedron_compile_commands",
    sha256 = "b0e084fa3fb7cf43d149debedfcc941c3e3607b2b79de272f5e61463c52860c3",
    strip_prefix = "bazel-compile-commands-extractor-57046dba8d60f819887ea81933ed90f8e23a458a",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/57046dba8d60f819887ea81933ed90f8e23a458a.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
