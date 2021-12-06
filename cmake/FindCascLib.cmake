# This file is part of Noggit3, licensed under GNU General Public License (version 3).

# adds target CascLib

find_path (CASCLIB_INCLUDE_DIR CascLib.h CascPort.h)

find_library (_casc_debug_lib NAMES CascLibDAD CascLibDAS CascLibDUD CascLibDUS)
find_library (_casc_release_lib NAMES CascLibRAD CascLibRAS CascLibRUD CascLibRUS)
find_library (_casc_any_lib NAMES Casc Casclib CascLib)

set (CASC_LIBRARIES)
if (_casc_debug_lib AND _casc_release_lib)
  list (APPEND CASC_LIBRARIES debug ${_casc_debug_lib} optimized ${_casc_release_lib})
else()
  list (APPEND CASC_LIBRARIES ${_casc_any_lib})
endif()

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (CascLib DEFAULT_MSG CASC_LIBRARIES CASCLIB_INCLUDE_DIR)

mark_as_advanced (CASC_INCLUDE_DIR _casc_debug_lib _casc_release_lib _casc_any_lib CASC_LIBRARIES)

add_library (CascLib INTERFACE)
target_link_libraries (CascLib INTERFACE ${Casc_LIBRARIES})
set_property  (TARGET CascLib APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${CASCLIB_INCLUDE_DIR})
set_property  (TARGET CascLib APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CASCLIB_INCLUDE_DIR})

#! \note on Windows, Casc tries to auto-link. There is no proper flag to disable that, so abuse this one.
target_compile_definitions (CascLib INTERFACE -D__CASCLIB_SELF__)
