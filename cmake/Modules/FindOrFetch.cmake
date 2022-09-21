include(FetchContent)
find_package(PkgConfig REQUIRED)

# Common functions for managing dependencies
# ##################################################################################################

macro(find_or_fetch_package package repo tag)
    # Set the package name
    string(TOUPPER ${package} PACKAGE_NAME)

    if(NOT ${PACKAGE_NAME}_FETCH)
        # Try finding the package
        find_package(${package} QUIET)

    endif()

    # If the find_package fails or if it's been set in the cmake cache to be forcefully fetched
    if(NOT ${package}_FOUND)
        if(NOT ${PACKAGE_NAME}_FETCH)
            message(STATUS "${package} not found. Fetching it.")
        else()
            message(STATUS "${package} is being fetched.")
        endif()
        # Fetch the package from source
        fetchcontent_declare(
            ${package}
            GIT_REPOSITORY ${repo}
            GIT_TAG ${tag}
        )
        # Signal that we fetch this package
        set(${PACKAGE_NAME}_FETCH TRUE)

        # Set the variables specified in the remaining macro arguments
        set(SET_VARS "${ARGN}")
        list(LENGTH SET_VARS SET_VARS_LEN)
        if(SET_VARS_LEN)
            math(EXPR SET_VARS_LEN "${SET_VARS_LEN}-1")
            foreach(VAR_NAME_INDEX RANGE 0 ${SET_VARS_LEN} 2)
                math(EXPR VAR_VALUE_INDEX "${VAR_NAME_INDEX}+1")

                list(GET SET_VARS ${VAR_NAME_INDEX} VAR_NAME)
                list(GET SET_VARS ${VAR_VALUE_INDEX} VAR_VALUE)
                set(${VAR_NAME} ${VAR_VALUE})
            endforeach()
        endif()

        message(STATUS "Begin ${package} output --")

        # Hacky solution to be sure to include the fetched package in the build
        fetchcontent_getproperties(${package})
        if(NOT ${package}_POPULATED)
            fetchcontent_populate(${package})
            add_subdirectory(${${package}_SOURCE_DIR} ${${package}_BINARY_DIR} EXCLUDE_FROM_ALL)
            set(${package}_DIR ${${package}_BINARY_DIR})
        endif()

        message(STATUS "End ${package} output --")
    else()
        message(STATUS "Found ${package}.")
    endif()
endmacro()
