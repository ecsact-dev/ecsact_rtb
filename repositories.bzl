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
        sha256 = "c4fbcde50de216cd6ec52691f450f9953a8dd41168f1811fae5e4d430d66a5e3",
        strip_prefix = "ecsact_rt_entt-d905e9b4b338b3564ed7105155e6ac2c2832dfc8",
        urls = ["https://github.com/ecsact-dev/ecsact_rt_entt/archive/d905e9b4b338b3564ed7105155e6ac2c2832dfc8.zip"],
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
        sha256 = "039e08ed758ec59ac8c318995a3ff707224808da33da1622f4b35ab381c0a31e",
        strip_prefix = "ecsact_runtime-0e319a80db0158e16135ebeca02497406b46be53",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/0e319a80db0158e16135ebeca02497406b46be53.zip",
    )

    http_archive(
        name = "ecsact_parse_runtime_interop",
        sha256 = "c39bdfa6af986214bdfaf0f2793c77b35ac9690bc085cdd0ab90beca845e536c",
        strip_prefix = "ecsact_parse_runtime_interop-0101aaa89179ddd63c81ed8267191ec86943f99c",
        url = "https://github.com/ecsact-dev/ecsact_parse_runtime_interop/archive/0101aaa89179ddd63c81ed8267191ec86943f99c.zip",
    )

    http_archive(
        name = "ecsact_parse",
        sha256 = "44c465e40b7d14546e025cdad631b1c6041e95eb58c8d18e28358f7ce2c4d825",
        strip_prefix = "ecsact_parse-7880d1d89d9f18cf930ed0071c880c253c02af3c",
        url = "https://github.com/ecsact-dev/ecsact_parse/archive/7880d1d89d9f18cf930ed0071c880c253c02af3c.zip",
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "5f11ed3820945d04d391f3a4eedf7e30fad4973f66a95d4dfd306c6df66964a1",
        strip_prefix = "ecsact_lang_cpp-4f94e11d56c0e16acd48cfd2d7c1d4ff3660415a",
        url = "https://github.com/ecsact-dev/ecsact_lang_cpp/archive/4f94e11d56c0e16acd48cfd2d7c1d4ff3660415a.zip",
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
