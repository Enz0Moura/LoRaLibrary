cmake_minimum_required(VERSION 3.28)

project(LoRaLibrary)

set(ARDUINO_CLI arduino-cli)
set(ARDUINO_CONFIG ${CMAKE_SOURCE_DIR}/arduino-cli.yaml)

set(FQBN arduino:avr:uno)
set(PORT /dev/ttyACM0)

set(LOCAL_LIB_DIR ${CMAKE_SOURCE_DIR}/.arduino/libraries)
set(LOCAL_LIB_LINK ${LOCAL_LIB_DIR}/LoRaLibrary)


set(MOVING_TERMINAL_SKETCH ${CMAKE_SOURCE_DIR}/examples/moving_terminal)
set(RECEIVER_SKETCH ${CMAKE_SOURCE_DIR}/examples/receiver)


add_custom_target(prepare_library
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LOCAL_LIB_LINK}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LOCAL_LIB_LINK}/src

    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/libs/locino_arduino/
            ${LOCAL_LIB_LINK}/

    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/libs/locino_protocol/include
            ${LOCAL_LIB_LINK}/src/

    
)

add_custom_target(arduino_install_deps
    COMMAND ${ARDUINO_CLI} --config-file ${ARDUINO_CONFIG} core update-index
    COMMAND ${ARDUINO_CLI} --config-file ${ARDUINO_CONFIG} core install arduino:avr
    COMMAND ${ARDUINO_CLI} --config-file ${ARDUINO_CONFIG} lib install RadioHead
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS prepare_library
)

add_custom_target(compile_moving_terminal
    COMMAND ${ARDUINO_CLI}
            --config-file ${ARDUINO_CONFIG}
            compile
            --fqbn ${FQBN}
            ${MOVING_TERMINAL_SKETCH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS arduino_install_deps
)

add_custom_target(compile_receiver
    COMMAND ${ARDUINO_CLI}
            --config-file ${ARDUINO_CONFIG}
            compile
            --fqbn ${FQBN}
            ${RECEIVER_SKETCH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS arduino_install_deps
)

add_custom_target(upload_moving_terminal
    COMMAND ${ARDUINO_CLI}
            --config-file ${ARDUINO_CONFIG}
            upload
            -p ${PORT}
            --fqbn ${FQBN}
            ${MOVING_TERMINAL_SKETCH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS compile_moving_terminal
)

add_custom_target(upload_receiver
    COMMAND ${ARDUINO_CLI}
            --config-file ${ARDUINO_CONFIG}
            upload
            -p ${PORT}
            --fqbn ${FQBN}
            ${RECEIVER_SKETCH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS compile_receiver
)