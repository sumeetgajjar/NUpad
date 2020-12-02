add_custom_target(webUI ALL
        COMMAND yarn install --frozen-lockfile
        COMMAND yarn build
        COMMAND mv build ${CMAKE_BINARY_DIR}/webUI
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )