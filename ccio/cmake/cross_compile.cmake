if(PLATFORM STREQUAL "ios") 
    message("Cross compiling for iOS")
    set(TARGET "arm64-apple-ios12.0")

    set(SDK "iPhoneOS")

    set(ARCH "arm64")

    set(VER "17.0")

    set(CMAKE_SYSTEM_PROCESSOR aarch64)

    set(CMAKE_SYSTEM_NAME iOS)

    set(CMAKE_FIND_ROOT_PATH "/Applications/Xcode.app/Contents/Developer/Platforms/${SDK}.platform/Developer/SDKs/${SDK}.sdk")

    set(TOOLCHAIN "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain")

    set(CMAKE_OSX_SYSROOT ${CMAKE_FIND_ROOT_PATH})

    set(CMAKE_OSX_ARCHITECTURES ${ARCH})

    set(CMAKE_IPHONEOS_DEPLOYMENT_TARGET 12.0)

    set(CMAKE_C_COMPILER ${TOOLCHAIN}/usr/bin/cc)

    set(CMAKE_C_FLAGS "-target ${TARGET} -g")

    set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/usr/bin/c++)

    set(CMAKE_CXX_FLAGS "-target ${TARGET} -g")

    set(CMAKE_OBJCXX_COMPILER ${TOOLCHAIN}/usr/bin/c++)

    set(CMAKE_OBJCXX_FLAGS "-target ${TARGET} -fobjc-arc -fobjc-weak -g")

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
else()

endif()