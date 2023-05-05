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
    sha256 = "e2155ef076dacd451287c7376637f2879d93a7e50a903cdebf8a78817c392e57",
    strip_prefix = "bazel-compile-commands-extractor-80ac7efb33a65f0cc627635e5b958ea3eb03231d",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/80ac7efb33a65f0cc627635e5b958ea3eb03231d.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
