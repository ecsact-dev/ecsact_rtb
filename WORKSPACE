workspace(name = "ecsact_rtb")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_github_biojppm_c4core",
    commit = "c54693bb85f3fd6ff4a8fdf7f074a5310336d7f5",
    init_submodules = True,
    remote = "https://github.com/zaucy/c4core.git",
    shallow_since = "1652645071 -0700",
)

git_repository(
    name = "com_github_biojppm_rapidyaml",
    commit = "ec6fea4ed700b73f856d1d858df8a31a2a50994e",
    remote = "https://github.com/zaucy/rapidyaml.git",
    shallow_since = "1652645354 -0700",
)

http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "644b7c35adbfbf312d86176660a196e67e7a2c3f20114941a60d0379f2b4771f",
    strip_prefix = "bazel-toolchain-9e71d562023dc7994e747110ee1ca345ad6b4413",
    url = "https://github.com/grailbio/bazel-toolchain/archive/9e71d562023dc7994e747110ee1ca345ad6b4413.zip",
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

http_archive(
    name = "bazelregistry_docopt_cpp",
    sha256 = "a06e705978b1c09427f130a187cb361916c1e7d66b69e91b865ebcd5390a6774",
    strip_prefix = "docopt.cpp-1e38ceb67655f576814ae1c95f138eb8b61453c9",
    url = "https://github.com/bazelregistry/docopt.cpp/archive/1e38ceb67655f576814ae1c95f138eb8b61453c9.zip",
)

http_archive(
    name = "boost",
    sha256 = "c41441a6e9f8038ad626e9f937ddc3675ab896b6c3512eefc6840037b3816d03",
    strip_prefix = "boost-563e8e0de4eac4b48a02d296581dc2454127608e",
    urls = ["https://github.com/bazelboost/boost/archive/563e8e0de4eac4b48a02d296581dc2454127608e.zip"],
)

load("@boost//:index.bzl", "boost_http_archives")

boost_http_archives()

git_repository(
    name = "ecsact",
    commit = "9cb97ce962718b3c92dac91a7ac9dfc61b4bd420",
    remote = "git@github.com:seaube/ecsact.git",
    shallow_since = "1656433721 -0400",
)

http_archive(
    name = "com_github_skypjack_entt",
    patch_args = ["-p1"],
    patches = ["//patches:entt_export_source_files.patch"],
    sha256 = "f7031545130bfc06f5fe6b2f8c87dcbd4c1254fab86657e2788b70dfeea57965",
    strip_prefix = "entt-3.10.1",
    url = "https://github.com/skypjack/entt/archive/refs/tags/v3.10.1.tar.gz",
)

git_repository(
    name = "ecsact_entt",
    commit = "47fdb9612d2f1273592aeb3434c2b3110662c9b7",
    remote = "git@github.com:seaube/ecsact-entt.git",
    shallow_since = "1656435051 -0700",
)

_export_all_build_file_content = """
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "files",
    srcs = glob(["**/*"]),
)
"""

http_archive(
    name = "boost_mp11_files",
    build_file_content = _export_all_build_file_content,
    sha256 = "d3f8ef486f2001c24eb0bc766b838fcce65dbb4edd099f136bf1ac4b51469f7c",
    strip_prefix = "mp11-boost-1.79.0",
    url = "https://github.com/boostorg/mp11/archive/refs/tags/boost-1.79.0.tar.gz",
)
