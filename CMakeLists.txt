cmake_minimum_required(VERSION 3.16)

project(AbstractCircuitEmulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appAbstractCircuitEmulator
    main.cpp
)

qt_add_qml_module(appAbstractCircuitEmulator
    URI AbstractCircuitEmulator
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES Model/simulator.h Model/simulator.cpp
        SOURCES Model/circuitwrapper.h Model/circuitwrapper.cpp
        SOURCES Model/LC3/lc3assembler.h Model/LC3/lc3assembler.cpp
        SOURCES Model/LC3/lc3binarycodegen.h Model/LC3/lc3binarycodegen.cpp
        SOURCES Model/LC3/lc3circuit.h Model/LC3/lc3circuit.cpp
        SOURCES Model/LC3/lc3module.cpp
        SOURCES Model/GlobalDefinitions/circuittype.h
        SOURCES Model/GlobalDefinitions/typedef.h
        SOURCES ModelView/console.h ModelView/console.cpp
        SOURCES Controller/assembleclicked.h Controller/assembleclicked.cpp
        SOURCES Controller/runclicked.h Controller/runclicked.cpp
        SOURCES Controller/filereader.h Controller/filereader.cpp
        SOURCES ModelView/consoleglobal.h
        SOURCES Controller/globalfilereader.h
        SOURCES Controller/globalfilereader.cpp
        SOURCES ModelView/consoleglobal.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appAbstractCircuitEmulator PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appAbstractCircuitEmulator
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appAbstractCircuitEmulator
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appAbstractCircuitEmulator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
