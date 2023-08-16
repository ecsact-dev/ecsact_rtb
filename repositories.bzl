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
        sha256 = "0ac010f232d3089200c5e545bcbd6480cf68b705de6930d8ff7cdb0a29f5b47b",
        strip_prefix = "entt-3.11.1",
        url = "https://github.com/skypjack/entt/archive/refs/tags/v3.11.1.tar.gz",
    )

    http_archive(
        name = "ecsact_rt_entt",
        sha256 = "",
        strip_prefix = "ecsact_rt_entt-0.2.0",
        urls = ["https://github.com/ecsact-dev/ecsact_rt_entt/archive/refs/tags/0.2.0.tar.gz"],
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
        sha256 = "e846a1ea81ca5d0ba9d39a4cd55209eb74c22bd85bc37a411eb5f6582b95040d",
        strip_prefix = "ecsact_runtime-0.1.0",
        urls = ["https://github.com/ecsact-dev/ecsact_runtime/archive/refs/tags/0.1.0.tar.gz"],
    )

    http_archive(
        name = "ecsact_interpret",
        sha256 = "c4d1aa8bd9194aed5676c6188d4ced6dfefbf5af51cad8486c5267d1892fa680",
        strip_prefix = "ecsact_interpret-0.1.0",
        urls = ["https://github.com/ecsact-dev/ecsact_interpret/archive/refs/tags/0.1.0.tar.gz"],
    )

    http_archive(
        name = "ecsact_parse",
        sha256 = "c4d1c36e8e3914c5505b4114e84b6eeb13075b29ac754c14e835800661dabd9a",
        strip_prefix = "ecsact_parse-0.1.0",
        urls = ["https://github.com/ecsact-dev/ecsact_parse/archive/refs/tags/0.1.0.tar.gz"],
    )

    http_archive(
        name = "ecsact_lang_cpp",
        sha256 = "28f94434bc2033df014eaff44fbae92a2a528d9d883090220ae8323261854e68",
        strip_prefix = "ecsact_lang_cpp-0.1.0",
        urls = ["https://github.com/ecsact-dev/ecsact_lang_cpp/archive/refs/tags/0.1.0.tar.gz"],
    )

    http_archive(
        name = "ecsact_si_wasm",
        sha256 = "e208a94d4f4a9c09f32b8a9ea91a4f799492e11c7c852b0329b4a3595a45cee6",
        strip_prefix = "ecsact_si_wasm-0.1.0",
        urls = ["https://github.com/ecsact-dev/ecsact_si_wasm/archive/refs/tags/0.1.0.tar.gz"],
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
