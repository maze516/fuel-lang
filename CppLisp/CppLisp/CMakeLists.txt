cmake_minimum_required (VERSION 2.6)

project (fuel)

set (CMAKE_CXX_STANDARD 11)

#add_compile_definitions(WITH_STATIC_DEBUGGER)
add_definitions(-DWITH_STATIC_DEBUGGER)

include_directories(
    ../CppLispInterpreter
    ../CppLispDebugger
)

add_executable(fuel FuelMain.cpp
../CppLispInterpreter/csobject.h
../CppLispInterpreter/csstring.h
../CppLispInterpreter/cstypes.h
../CppLispInterpreter/Token.h
../CppLispInterpreter/Tokenizer.h
../CppLispInterpreter/Parser.h
../CppLispInterpreter/Utils.h
../CppLispInterpreter/Exception.h
../CppLispInterpreter/Variant.h
../CppLispInterpreter/Scope.h
../CppLispInterpreter/Environment.h
../CppLispInterpreter/Interpreter.h
../CppLispInterpreter/DebuggerInterface.h
../CppLispInterpreter/Lisp.h
../CppLispInterpreter/fuel.h
../CppLispDebugger/Debugger.h

../CppLispInterpreter/csobject.cpp
../CppLispInterpreter/csstring.cpp
../CppLispInterpreter/cstypes.cpp
../CppLispInterpreter/Token.cpp
../CppLispInterpreter/Tokenizer.cpp
../CppLispInterpreter/Parser.cpp
../CppLispInterpreter/Utils.cpp
../CppLispInterpreter/Exception.cpp
../CppLispInterpreter/Variant.cpp
../CppLispInterpreter/Scope.cpp
../CppLispInterpreter/Environment.cpp
../CppLispInterpreter/Interpreter.cpp
../CppLispInterpreter/Lisp.cpp
../CppLispInterpreter/fuel.cpp
../CppLispDebugger/Debugger.cpp
)

set_target_properties(fuel PROPERTIES LINK_FLAGS_RELEASE -s)

target_link_libraries(fuel ${CMAKE_DL_LIBS})

install(TARGETS fuel DESTINATION bin)

if (CMAKE_SYSTEM_NAME MATCHES "Android")
     add_custom_command(TARGET fuel POST_BUILD
            COMMAND "${ANDROID_TOOLCHAIN_PREFIX}strip" -g -S -d --strip-debug --verbose
            fuel
            COMMENT "Strip debug symbols done on final fuel binary.")
endif()
            