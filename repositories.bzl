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
        name = "rules_ecsact",
        sha256 = "8cfdc8395502ae8d764611d0b6911d26461dc41511151675ccc019d9ebe723c5",
        strip_prefix = "rules_ecsact-0.1.5",
        url = "https://github.com/ecsact-dev/rules_ecsact/archive/refs/tags/0.1.5.tar.gz",
    )

    http_archive(
        name = "ecsact_rt_entt",
        sha256 = "bd42f68a883773a1621522006b8da853335489ed113fcf53695dbc6754d2ba28",
        strip_prefix = "ecsact_rt_entt-75629b8b6789c0946e27b7a1d510dc160bf1d0bd",
        urls = ["https://github.com/ecsact-dev/ecsact_rt_entt/archive/75629b8b6789c0946e27b7a1d510dc160bf1d0bd.zip"],
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
        sha256 = "f7b5f5cc5b70620934be48090048e9092dbc86ba4e027959d65fd1dd46c36a77",
        strip_prefix = "ecsact_runtime-475cae75c657b2570b1041264670c026de412814",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/475cae75c657b2570b1041264670c026de412814.zip",
    )

    http_archive(
        name = "ecsact_interpret",
        sha256 = "ac3fb03b584ec226bb2d240621e7a539339fbfd6dc2131580109c35f3b11f461",
        strip_prefix = "ecsact_interpret-d014d7aa30ee2a1ac5086773a6c3477451671acd",
        url = "https://github.com/ecsact-dev/ecsact_interpret/archive/d014d7aa30ee2a1ac5086773a6c3477451671acd.zip",
    )

    http_archive(
        name = "ecsact_parse",
        sha256 = "b3f65f9c5ec62d77647c32736738458230469cac937eab82d46a577e52b7fc2a",
        strip_prefix = "ecsact_parse-45a30d62a0c53acb3a329fa9df250c20f0b384d1",
        url = "https://github.com/ecsact-dev/ecsact_parse/archive/45a30d62a0c53acb3a329fa9df250c20f0b384d1.zip",
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "2b2adc4d1a002d7f8fbe24153c9aff5fd7c616de13cf36bd520b7ec4e345965a",
        strip_prefix = "ecsact_lang_cpp-18c6f0df8be535a1586abb7f4e3460b635976a44",
        url = "https://github.com/ecsact-dev/ecsact_lang_cpp/archive/18c6f0df8be535a1586abb7f4e3460b635976a44.zip",
    )

    http_archive(
        name = "ecsact_si_wasm",
        sha256 = "58a56a207e58dd05296d4bf9aafb46f76b68f2a2c78346fd6c42ed426f0d8dd3",
        strip_prefix = "ecsact_si_wasm-18f09a45ae6f5022c2e34e9846057f8bc889fb81",
        urls = ["https://github.com/seaube/ecsact_si_wasm/archive/18f09a45ae6f5022c2e34e9846057f8bc889fb81.zip"],
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
