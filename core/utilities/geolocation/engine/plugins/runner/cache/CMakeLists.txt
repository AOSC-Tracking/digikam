#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

set(cache_SRCS CachePlugin.cpp CacheRunner.cpp)

marble_add_plugin(NAME    CachePlugin
                  SOURCES ${cache_SRCS})
