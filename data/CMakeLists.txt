install(FILES ${CMAKE_SOURCE_DIR}/data/graceful-greeter.desktop DESTINATION /usr/share/xgreeters/)
execute_process(COMMAND gzip -c ${CMAKE_SOURCE_DIR}/data/graceful-greeter.3
        OUTPUT_FILE ${CMAKE_BINARY_DIR}/graceful-greeter.3.gz
        )
install(FILES ${CMAKE_BINARY_DIR}/graceful-greeter.3.gz DESTINATION /usr/share/man/man3)