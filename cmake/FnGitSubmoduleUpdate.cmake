function(git_submodule_update PATH)
    find_package(Git REQUIRED)
    # Old versions of git aren't able to run init+update
    # in one go (via 'git submodule update --init'), we need
    # to call one command for each operation:
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule init -- ${PATH}
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update -- ${PATH}
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction(git_submodule_update)
