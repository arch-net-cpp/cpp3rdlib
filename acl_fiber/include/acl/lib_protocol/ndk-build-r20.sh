#!/bin/sh
rm -f jni
rm -rf obj
ln -s src jni
cd jni

export app_abi="armeabi-v7a x86"
export app_platform=android-16
export ndk_toolchain_version=4.6

ndk-build

#export app_abi=x86
#ndk-build

#~/Library/Android/android-ndk-r9d/ndk-build

cd ..
