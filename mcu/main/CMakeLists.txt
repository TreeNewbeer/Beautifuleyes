if(IDF_TARGET STREQUAL "esp32s2")
idf_component_register(SRCS "main.cpp" "TouchElement.cpp" "FrameEncoder.cpp" "SerialPort.cpp" "FrameDecoder.cpp"
                       INCLUDE_DIRS ".")
else()
    message(FATAL_ERROR "This firmware only available on esp32s2 now")
endif()
