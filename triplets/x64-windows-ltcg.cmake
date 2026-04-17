# Custom vcpkg triplet for OpenApoc CI: static library linkage so the final
# LTCG link can see dependency IR for whole-program optimization (wrapper
# inlining, small-function inlining, devirtualization across library
# boundaries), with a dynamic CRT to keep the runtime footprint small.
#
# Used by appveyor{,-dev}.yml via VCPKG_DEFAULT_TRIPLET + VCPKG_OVERLAY_TRIPLETS.
# Qt6 is NOT built via vcpkg (USE_SYSTEM_QT=ON with an upstream binary
# distribution), so this triplet only affects the smaller vcpkg deps
# (boost-locale, boost-program_options, SDL2, vorbis, ogg).

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_BUILD_TYPE release)

# Enables -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE for ports that honour the
# CMAKE_INTERPROCEDURAL_OPTIMIZATION knob (most CMake-based ports do).
# Translates to /GL on compile + /LTCG on link for MSVC, giving the ports'
# object files IR contents that the final OpenApoc LTCG link can merge.
set(VCPKG_CMAKE_CONFIGURE_OPTIONS "-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE")
