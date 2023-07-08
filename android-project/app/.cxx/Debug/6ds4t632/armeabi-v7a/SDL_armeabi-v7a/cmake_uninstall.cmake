if (NOT EXISTS "/home/germansuh/DungeonAdventure/android-project/app/.cxx/Debug/6ds4t632/armeabi-v7a/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/home/germansuh/DungeonAdventure/android-project/app/.cxx/Debug/6ds4t632/armeabi-v7a/install_manifest.txt\"")
endif()

file(READ "/home/germansuh/DungeonAdventure/android-project/app/.cxx/Debug/6ds4t632/armeabi-v7a/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND /home/germansuh/Android/Sdk/cmake/3.22.1/bin/cmake -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach()
