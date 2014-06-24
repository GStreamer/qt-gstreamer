#!/bin/sh
# This script creates (pre-)release tarballs with their md5sums.
# To use this script:
# 1) Change QTGSTREAMER_VERSION in CMakeLists.txt to the final release version that you are making.
# 2) Commit the result.
# 3) Call this script to create the tarballs.

VERSION=`cat CMakeLists.txt | grep QTGSTREAMER_VERSION | sed -r 's#set\(QTGSTREAMER_VERSION ([0-9\.]+)\)#\1#'`
echo "Making tarballs for version $VERSION..."

git archive --format=tar --prefix=qt-gstreamer-$VERSION/ HEAD | xz > qt-gstreamer-$VERSION.tar.xz
sha256sum qt-gstreamer-$VERSION.tar.xz > qt-gstreamer-$VERSION.tar.xz.sha256sum

echo "Done."
