workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "51f4baa3bad0fd7ad3a20a12462549b6a2dc329db32226a513d4c648724bb24c",
    strip_prefix = "bazel-toolchain-ceeedcc4464322e05fe5b8df3749cc02273ee083",
    url = "https://github.com/grailbio/bazel-toolchain/archive/ceeedcc4464322e05fe5b8df3749cc02273ee083.zip",
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
    sha256 = "044b148b111e17cee61d8c3aaeed42069f7325460382340afca4919f8265094c",
    strip_prefix = "bazel-compile-commands-extractor-1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
