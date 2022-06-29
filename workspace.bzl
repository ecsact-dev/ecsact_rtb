load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@boost//:index.bzl", "boost_http_archives")

def ecsact_rtb_workspace():
    bazel_skylib_workspace()
    boost_http_archives()
