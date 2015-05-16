#!/bin/sh
pushd `dirname $0` > /dev/null
rm -f -r \
 C\#/*.csproj.user \
 C++/*.aps \
 C++/*.vcxproj.user \
 MP3epoc.sdf \
 MP3epoc.*.suo \
 MP3epoc.xcodeproj/project.xcworkspace \
 MP3epoc.xcodeproj/xcuserdata \
 TestResults \
 Unit\ Tests\ C\#/*.csproj.user \
 Unit\ Tests\ C++/*.aps \
 Unit\ Tests\ C++/*.vcxproj.user \
 Visual\ Studio\ Build
find . -name .DS_Store -type f -delete
popd > /dev/null
