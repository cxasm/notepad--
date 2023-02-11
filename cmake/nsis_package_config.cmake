set(CPACK_GENERATOR NSIS)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
# 维护人员
set(CPACK_PACKAGE_CONTACT "XiaoPb")

set(P4_VERSION_INFO_CL_HIGH "0")
set(P4_VERSION_INFO_CL_LOW "0")
# set(CPACK_PACKAGE_VERSION_PATCH "0")

set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}V${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}V${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")

set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/Resources/ico/txt (9).ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/src/Resources/ico/txt (9).ico")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License")

set (CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}" )

# 设置 安装包属性信息
set (CPACK_NSIS_DEFINES "
        ${CPACK_NSIS_DEFINES}
        VIProductVersion ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${P4_VERSION_INFO_CL_HIGH}.${P4_VERSION_INFO_CL_LOW}
        VIFileVersion ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${P4_VERSION_INFO_CL_HIGH}.${P4_VERSION_INFO_CL_LOW}
        VIAddVersionKey /LANG=0 \\\"ProductName\\\" \\\"${CPACK_PACKAGE_NAME}\\\"
        VIAddVersionKey /LANG=0 \\\"ProductVersion\\\" \\\"v${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_BUILD_TYPE_REVISION}\\\"
        VIAddVersionKey /LANG=0 \\\"Comments\\\" \\\"${CPACK_PACKAGE_DESCRIPTION}\\\"
        VIAddVersionKey /LANG=0 \\\"CompanyName\\\" \\\"${CPACK_PACKAGE_VENDOR}\\\"
        VIAddVersionKey /LANG=0 \\\"LegalCopyright\\\" \\\"${CPACK_PACKAGE_COPYRIGHT}\\\"
        VIAddVersionKey /LANG=0 \\\"FileDescription\\\" \\\"${CPACK_PACKAGE_NAME} Installer\\\"
        VIAddVersionKey /LANG=0 \\\"FileVersion\\\" \\\"v${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_BUILD_TYPE_REVISION}\\\"
        "
)

# 设置 安装时需要的环境变量 
set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        WriteRegStr SHCTX \\\"Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\${PROJECT_NAME}\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\${PROJECT_NAME}\\\\shell\\\" \\\"\\\" \\\"open\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\${PROJECT_NAME}\\\\shell\\\\open\\\\command\\\" \\\"\\\" \\\"$\\\\\\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe$\\\\\\\" $\\\\\\\"%1$\\\\\\\"\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\.txt\\\\OpenWithProgids\\\" \\\"${PROJECT_NAME}\\\" \\\"\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\*\\\\shell\\\\${PROJECT_NAME}\\\" \\\"\\\" \\\"Edit with ${PROJECT_NAME}\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\*\\\\shell\\\\${PROJECT_NAME}\\\" \\\"Icon\\\" \\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe\\\"
        WriteRegStr SHCTX \\\"Software\\\\Classes\\\\*\\\\shell\\\\${PROJECT_NAME}\\\\command\\\" \\\"\\\" \\\"$\\\\\\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe$\\\\\\\" $\\\\\\\"%1$\\\\\\\"\\\"
     ")

# 设置 卸载时需要的环境变量 
set (CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\"
        Delete \\\"$SMPROGRAMS\\\\${PROJECT_NAME}.lnk\\\"
        DeleteRegKey SHCTX \\\"Software\\\\Classes\\\\*\\\\shell\\\\${PROJECT_NAME}\\\"
        DeleteRegKey SHCTX \\\"Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\bin\\\\${PROJECT_NAME}.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe\\\"
        DeleteRegKey SHCTX \\\"Software\\\\${PROJECT_NAME}\\\"
        DeleteRegValue SHCTX \\\"Software\\\\Classes\\\\.txt\\\\OpenWithProgids\\\" \\\"${PROJECT_NAME}\\\"
        DeleteRegKey SHCTX \\\"Software\\\\Classes\\\\${PROJECT_NAME}\\\"
     ")
