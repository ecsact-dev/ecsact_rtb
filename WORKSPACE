workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "3cf5ac4bb7dae3776da586fe85685274f5d89cbb59436f50f572928354e72198",
    strip_prefix = "bazel-toolchain-70a0fb556662fe3cadb07da32c91e232eb0d883d",
    url = "https://github.com/grailbio/bazel-toolchain/archive/70a0fb556662fe3cadb07da32c91e232eb0d883d.zip",
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
    sha256 = "7f6ebb62298694d8cf3ecaed81b3bb48de559819ac1909d4055abdc8c0ae1000",
    strip_prefix = "bazel-compile-commands-extractor-800b9cd260ce3878e94abb7d583a7c0865f7d967",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/800b9cd260ce3878e94abb7d583a7c0865f7d967.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
