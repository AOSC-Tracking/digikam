#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

set(json_SRCS JsonRunner.cpp JsonPlugin.cpp JsonParser.cpp)

marble_add_plugin(NAME    JsonPlugin
                  SOURCES ${json_SRCS})
