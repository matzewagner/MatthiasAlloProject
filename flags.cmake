#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#    target_link_libraries("${APP_NAME}" /usr/local/lib/libloris.dylib lo)
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries("${APP_NAME}" loris lo)


