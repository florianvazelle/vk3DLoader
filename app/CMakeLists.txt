#
# Define vk3DLoader
#

set(VK3DLOADER "vk3DLoader")

add_executable(${VK3DLOADER} shadow.cpp)

target_link_libraries(${VK3DLOADER} PRIVATE ${PROJECT_NAME})

# target_set_warnings(${VK3DLOADER} ENABLE ALL AS_ERROR ALL DISABLE Annoying)

#
# Define vkLavaMpm
#

set(VKLAVAMPM "vkLavaMpm")

add_executable(${VKLAVAMPM} particle.cpp)

target_link_libraries(${VKLAVAMPM} PRIVATE ${PROJECT_NAME})

# target_set_warnings(${VKLAVAMPM} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
