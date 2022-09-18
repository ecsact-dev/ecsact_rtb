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
        sha256 = "f47dc87a0f948d9f8d4458df32c7b3b2191d94be5ba675798100f4534098a646",
        strip_prefix = "rules_ecsact-0.1.3",
        url = "https://github.com/ecsact-dev/rules_ecsact/archive/refs/tags/0.1.3.tar.gz",
    )

    http_archive(
        name = "ecsact_rt_entt",
        sha256 = "01179cb8499de697a3a05f8c114ab591686cb63b7d2113a286361d5e47d0015f",
        strip_prefix = "ecsact_rt_entt-4c4f7755b0549eb09cc01edd6fb10f05839cc5b4",
        urls = ["https://github.com/ecsact-dev/ecsact_rt_entt/archive/4c4f7755b0549eb09cc01edd6fb10f05839cc5b4.zip"],
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
        sha256 = "5c70d7b3dd8c91edb2f8f7cfccd94f2ac585c618ca7a8fc626322f9d08b6851a",
        strip_prefix = "ecsact_runtime-a55229e28bfda2eb7ea8b4173354f58c01420ec5",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/a55229e28bfda2eb7ea8b4173354f58c01420ec5.zip",
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "7a9998065ab78fcbf4355d82ee13c137c475639592d649cbe607386244c89cf7",
        strip_prefix = "ecsact_lang_cpp-b62b41856d04990adef0f839a754a86e398af121",
        url = "https://github.com/ecsact-dev/ecsact_lang_cpp/archive/b62b41856d04990adef0f839a754a86e398af121.zip",
    )

    http_archive(
        name = "ecsact_parse",
        sha256 = "e75458acc83327d3d7c677e79cb8f69521ecf4bde3ed30589788d4e039ae2a1f",
        strip_prefix = "ecsact_parse-f90dc260554f4e6fca2894198273444d7e992a5b",
        url = "https://github.com/ecsact-dev/ecsact_parse/archive/f90dc260554f4e6fca2894198273444d7e992a5b.zip",
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "7a9998065ab78fcbf4355d82ee13c137c475639592d649cbe607386244c89cf7",
        strip_prefix = "ecsact_lang_cpp-b62b41856d04990adef0f839a754a86e398af121",
        url = "https://github.com/ecsact-dev/ecsact_lang_cpp/archive/b62b41856d04990adef0f839a754a86e398af121.zip",
    )

    http_archive(
        name = "ecsactsi_wasm",
        sha256 = "51f9c51c26732f16147b6f762ca1461d8305bf798c9a7df4ba282fbf6c7d12fb",
        strip_prefix = "ecsactsi-wasm-5eaca7e00ea7858309aaf32a9bc24e0e8c8d0a41",
        urls = ["https://github.com/seaube/ecsactsi-wasm/archive/5eaca7e00ea7858309aaf32a9bc24e0e8c8d0a41.zip"],
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
