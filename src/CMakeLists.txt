add_executable(
    json-dbus-bridge
    json-dbus-bridge.c
    bridge.c
    bridge_request.c
)
target_link_libraries(json-dbus-bridge ${DBUS_LIBRARIES} ${JSON_C_LIBRARIES} ${LIBEVENT_LIBRARIES} ${FCGI_LIBRARY} ${LIBSYSTEMD_LIBRARIES})

install(TARGETS json-dbus-bridge DESTINATION bin COMPONENT bin)
