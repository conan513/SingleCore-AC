CU_SET_PATH("CMAKE_MOD_ACCOUNT_MOUNTS_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/mod_account_mount.cpp")

AC_ADD_SCRIPT_LOADER("AccountMounts" "${CMAKE_CURRENT_LIST_DIR}/src/loader_account_mount.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/mod_account_mount.conf.dist")
