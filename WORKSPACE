workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "ba89390c2fff1a498456de977dab5c6833b4dcb364b88c405dc375b99e1f5c46",
    strip_prefix = "bazel-toolchain-d52ce5d96915428a5ee6f7079078df706c5fddab",
    url = "https://github.com/grailbio/bazel-toolchain/archive/d52ce5d96915428a5ee6f7079078df706c5fddab.zip",
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
    sha256 = "c00e0ba4aa7cb480849861333b4f9f9b2ceca997052f6129eec9778b90ee7700",
    strip_prefix = "bazel-compile-commands-extractor-7831ea1a9e183a0b23206f7f56726e313ddac5c1",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/7831ea1a9e183a0b23206f7f56726e313ddac5c1.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
