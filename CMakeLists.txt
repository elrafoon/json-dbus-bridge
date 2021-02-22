cmake_minimum_required(VERSION 3.10)
project(json-debug-bridge)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)
find_package(PkgConfig REQUIRED)

pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(JSON_C REQUIRED json-c)
pkg_check_modules(LIBEVENT REQUIRED libevent)
pkg_check_modules(LIBSYSTEMD REQUIRED libsystemd)
find_package(fcgi REQUIRED)

include_directories(${DBUS_INCLUDE_DIRS} ${JSON_C_INCLUDE_DIRS} ${LIBEVENT_INCLUDE_DIRS} ${FCGI_INCLUDE_DIR} ${LIBSYSTEMD_INCLUDE_DIRS})

if(LIBSYSTEMD_FOUND)
    add_compile_definitions(CONFIG_SYSTEMD=1)
endif()

add_subdirectory(src)
add_subdirectory(nginx)
add_subdirectory(systemd)

###############################################
## PACKAGING
###############################################
#set(CPACK_COMPONENTS_ALL devel toolchain_linux toolchain_windows)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "json-dbus-bridge")
set(CPACK_PACKAGE_VENDOR "Software-Industry, s.r.o.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "JSON-DBUS HTTP Bridge")
set(CPACK_PACKAGE_CONTACT "Stanislav Ravas <ravas@tind.sk>")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME "json-dbus-bridge-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_BUILD_TYPE}")
set(CPACK_DEBIAN_PACKAGE_SECTION "httpd")

set(CPACK_DEBIAN_BIN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_BIN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/debian/prerm;${CMAKE_SOURCE_DIR}/debian/postinst")
#set(CPACK_DEBIAN_BIN_PACKAGE_DEPENDS "ra-core-conf")

#set(CPACK_DEBIAN_BIN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_NGINX_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/nginx/debian/prerm;${CMAKE_SOURCE_DIR}/nginx/debian/postinst")
set(CPACK_DEBIAN_NGINX_PACKAGE_DEPENDS "json-dbus-bridge-bin")
set(CPACK_DEBIAN_NGINX_PACKAGE_ARCHITECTURE "all")

set(CPACK_DEB_COMPONENT_INSTALL ON)
#set(CPACK_RPM_COMPONENT_INSTALL ON)
#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

include(CPack)
