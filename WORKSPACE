workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_grail_bazel_toolchain",
    strip_prefix = "bazel-toolchain-c78a67db025e967707febf04a60d57c2286d21ac",
    url = "https://github.com/yaxum62/bazel-toolchain/archive/c78a67db025e967707febf04a60d57c2286d21ac.tar.gz",
    sha256 = "7fd9b2efd8d9d62a8f5263e917d8b601f4e1fd44affe0c894420b2ee636f8454",
)

load("@com_grail_bazel_toolchain//toolchain:deps.bzl", "bazel_toolchain_dependencies")

bazel_toolchain_dependencies()

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    llvm_version = "13.0.0",
    cxx_standard = "c++20",
)

load("@llvm_toolchain//:toolchains.bzl", "llvm_register_toolchains")

llvm_register_toolchains()

http_archive(
    name = "bazelregistry_docopt_cpp",
    strip_prefix = "docopt.cpp-1e38ceb67655f576814ae1c95f138eb8b61453c9",
    url = "https://github.com/bazelregistry/docopt.cpp/archive/1e38ceb67655f576814ae1c95f138eb8b61453c9.zip",
    sha256 = "a06e705978b1c09427f130a187cb361916c1e7d66b69e91b865ebcd5390a6774",
)

http_archive(
    name = "boost",
    strip_prefix = "boost-563e8e0de4eac4b48a02d296581dc2454127608e",
    urls = ["https://github.com/bazelboost/boost/archive/563e8e0de4eac4b48a02d296581dc2454127608e.zip"],
    sha256 = "c41441a6e9f8038ad626e9f937ddc3675ab896b6c3512eefc6840037b3816d03",
)

load("@boost//:index.bzl", "boost_http_archives")
boost_http_archives()

git_repository(
    name = "ecsact",
    remote = "git@github.com:seaube/ecsact.git",
    commit = "07c29b81db1dd708e2cdb458f34349d00b9037b3",
    shallow_since = "1652477290 -0700",
)

http_archive(
    name = "com_github_skypjack_entt",
    strip_prefix = "entt-3.10.1",
    url = "https://github.com/skypjack/entt/archive/refs/tags/v3.10.1.tar.gz",
    sha256 = "f7031545130bfc06f5fe6b2f8c87dcbd4c1254fab86657e2788b70dfeea57965",
    patch_args = ["-p1"],
    patches = ["//patches:entt_export_source_files.patch"],
)

git_repository(
    name = "ecsact_entt",
    commit = "cc23d95e2ea5b030434f2ebf7ffd6f78b56b6c8c",
    remote = "git@github.com:seaube/ecsact-entt.git",
    shallow_since = "1651764189 -0700",
)
