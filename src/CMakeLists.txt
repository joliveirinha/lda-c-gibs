#general configuration
#set(CMAKE_C_FLAGS "-Wall -g")

# library 
set(LIBAMIDALA_SOURCES corpus.c
                       lda.c 
                       cokus.c
                       distributions.c
                       dirichlet.c
                       utils.c
                       libamidala.c
)

add_library(amidala SHARED ${LIBAMIDALA_SOURCES})
target_link_libraries(amidala m)

# AMIDALA executable
set(AMIDALA_APP_SOURCES amidala.c
)

add_executable(amidala_app ${AMIDALA_APP_SOURCES})
target_link_libraries(amidala_app amidala)

