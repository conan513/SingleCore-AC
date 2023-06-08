
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/LoginChat.cpp")

AC_ADD_SCRIPT_LOADER("LoginChat" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/login_chat.conf.dist")
