"""
"""

load("@boost//:index.bzl", "boost_http_archives")
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@ecsact_parse//:repositories.bzl", "ecsact_parse_dependencies")

def ecsact_rtb_workspace():
    bazel_skylib_workspace()
    boost_http_archives()
    ecsact_parse_dependencies()
