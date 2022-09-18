workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "7fa5a8624b1148c36e09c7fa29ef6ee8b83f865219c9c219c9125aac78924758",
    strip_prefix = "bazel-toolchain-c3131a6894804ee586d059c57ffe8e88d44172e1",
    url = "https://github.com/grailbio/bazel-toolchain/archive/c3131a6894804ee586d059c57ffe8e88d44172e1.zip",
)

load("@com_grail_bazel_toolchain//toolchain:deps.bzl", "bazel_toolchain_dependencies")

bazel_toolchain_dependencies()

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    cxx_standard = {"linux": "c++20"},
    llvm_version = "14.0.0",
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
