SET (GRACEFUL_GREETER_RESOURCES ${CMAKE_SOURCE_DIR}/app/resource/graceful-resources.qrc)

#add_custom_target(GRACEFUL_GREETER_RES
#        rcc -binary ${GRACEFUL_GREETER_RESOURCES} -o ${CMAKE_BINARY_DIR}/graceful-greeter-res.rcc
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/app/resource)

# 使用rcc处理.qrc文件
#QT5_ADD_RESOURCES(ALL_RESOURCES_RCC ${GRACEFUL_GREETER_RESOURCES})