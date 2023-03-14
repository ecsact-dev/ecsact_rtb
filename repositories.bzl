"""
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", _http_archive = "http_archive", _http_file = "http_file")
load("@bazel_tools//tools/build_defs/repo:git.bzl", _git_repository = "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

_EXPORT_ALL_BUILD_FILE_CONTENT = """
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "files",
    srcs = glob(["**/*"]),
)
"""

_NLOHMANN_JSON_BUILD_FILE = """
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "json",
    visibility = ["//visibility:public"],
    includes = ["include"],
    hdrs = glob(["include/**/*.hpp"]),
    strip_include_prefix = "include",
)
"""

def http_archive(name, **kwargs):
    maybe(_http_archive, name = name, **kwargs)

def http_file(name, **kwargs):
    maybe(_http_file, name = name, **kwargs)

def git_repository(name, **kwargs):
    maybe(_git_repository, name = name, **kwargs)

def ecsact_rtb_repositories():
    """Dependencies for ecsact_rtb
    """
    http_file(
        name = "vswhere",
        downloaded_file_path = "vswhere.exe",
        executable = True,
        sha256 = "261d281ce9c95c3e46900e67ace8c3518dd7b2596774f63c92e2bb1fbce71d4c",
        urls = ["https://github.com/microsoft/vswhere/releases/download/3.0.3/vswhere.exe"],
    )

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

    http_archive(
        name = "bazelregistry_docopt_cpp",
        sha256 = "a06e705978b1c09427f130a187cb361916c1e7d66b69e91b865ebcd5390a6774",
        strip_prefix = "docopt.cpp-1e38ceb67655f576814ae1c95f138eb8b61453c9",
        url = "https://github.com/bazelregistry/docopt.cpp/archive/1e38ceb67655f576814ae1c95f138eb8b61453c9.zip",
    )

    http_archive(
        name = "boost",
        sha256 = "4a79c389add7e3d54d0e12c83098d471d24536ba2d6b8593d3a95f151f25eebb",
        strip_prefix = "boost-da62319c330d81ef032517cbe13b6f35d97387cb",
        urls = ["https://github.com/bazelboost/boost/archive/da62319c330d81ef032517cbe13b6f35d97387cb.zip"],
    )

    http_archive(
        name = "com_github_skypjack_entt",
        patch_args = ["-p1"],
        patches = ["@ecsact_rtb//patches:entt_export_source_files.patch"],
        sha256 = "f7031545130bfc06f5fe6b2f8c87dcbd4c1254fab86657e2788b70dfeea57965",
        strip_prefix = "entt-3.10.1",
        url = "https://github.com/skypjack/entt/archive/refs/tags/v3.10.1.tar.gz",
    )

    http_archive(
        name = "ecsact_rt_entt",
        sha256 = "873d520a0a8641a2a98c114b48f31f1f1b0d264ca67489d74e9bbcaf542c2875",
        strip_prefix = "ecsact_rt_entt-fbf0ddd5ae82eaafb8bd5587388a016f37314584",
        urls = ["https://github.com/ecsact-dev/ecsact_rt_entt/archive/fbf0ddd5ae82eaafb8bd5587388a016f37314584.zip"],
    )

    http_archive(
        name = "boost_mp11_files",
        build_file_content = _EXPORT_ALL_BUILD_FILE_CONTENT,
        sha256 = "d3f8ef486f2001c24eb0bc766b838fcce65dbb4edd099f136bf1ac4b51469f7c",
        strip_prefix = "mp11-boost-1.79.0",
        url = "https://github.com/boostorg/mp11/archive/refs/tags/boost-1.79.0.tar.gz",
    )

    http_archive(
        name = "ecsact_runtime",
        sha256 = "ba5b8f1649fc86b9c803ab852d0612e2d100a64e2a06a13b612e111bf8d18735",
        strip_prefix = "ecsact_runtime-c6cdc81000bbee028a8e7a2bc95fc35711db1b75",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/c6cdc81000bbee028a8e7a2bc95fc35711db1b75.zip",
    )

    http_archive(
        name = "ecsact_interpret",
        sha256 = "25bb97427e4b213de4a4d50f367c027667fa915ff8cceb78ba2316ba62b5a22f",
        strip_prefix = "ecsact_interpret-da2e08e4159ea3fafc81b5f57a648d626338ea4d",
        url = "https://github.com/ecsact-dev/ecsact_interpret/archive/da2e08e4159ea3fafc81b5f57a648d626338ea4d.zip",
    )

    http_archive(
        name = "ecsact_parse",
        sha256 = "be9f96ac3ce4f6f394c3521f6e15837109dad36c47a768759ae4a3b773f86512",
        strip_prefix = "ecsact_parse-2a0b219ff1486ac910ab830d4b29cdcb745c286f",
        url = "https://github.com/ecsact-dev/ecsact_parse/archive/2a0b219ff1486ac910ab830d4b29cdcb745c286f.zip",
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "2fa208f81a8422f8bce9ed7200fa726622b77af7c8e08fdae4fa723d2e8fc163",
        strip_prefix = "ecsact_lang_cpp-3b70f5d276a4b29c709c687295d36ab804042e1a",
        url = "https://github.com/ecsact-dev/ecsact_lang_cpp/archive/3b70f5d276a4b29c709c687295d36ab804042e1a.zip",
    )

    http_archive(
        name = "ecsact_si_wasm",
        sha256 = "4153154fd80e2cd48ddd8e0d0c208691f134b33d5c67dc6fc081b3f77149b2c2",
        strip_prefix = "ecsact_si_wasm-2f8922cd9a72d54b7924742153a6113a88cc0867",
        urls = ["https://github.com/seaube/ecsact_si_wasm/archive/2f8922cd9a72d54b7924742153a6113a88cc0867.zip"],
    )

    http_archive(
        name = "nlohmann_json",
        url = "https://github.com/nlohmann/json/releases/download/v3.10.5/include.zip",
        sha256 = "b94997df68856753b72f0d7a3703b7d484d4745c567f3584ef97c96c25a5798e",
        build_file_content = _NLOHMANN_JSON_BUILD_FILE,
    )

    http_archive(
        name = "magic_enum",
        sha256 = "5e7680e877dd4cf68d9d0c0e3c2a683b432a9ba84fc1993c4da3de70db894c3c",
        strip_prefix = "magic_enum-0.8.0",
        urls = ["https://github.com/Neargye/magic_enum/archive/refs/tags/v0.8.0.tar.gz"],
    )
