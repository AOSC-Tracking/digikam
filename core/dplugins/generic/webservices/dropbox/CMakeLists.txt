#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-FileCopyrightText: 2018      by Maik Qualmann, <metzpinguin at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(dropboxplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dbmpform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbnewalbumdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbtalker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbwindow.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    DropBox
                           SOURCES ${dropboxplugin_SRCS}
)

add_subdirectory(icons)
