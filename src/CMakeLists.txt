
set(LIBAMIDALA_SOURCES amidala.c
                       lda_model.c 
                       lda_inference.c
                       lda_estimate.c 
                       corpus.c
                       utils.c
)

set(AMIDALA_APP_SOURCES main.c)

set(CMAKE_C_FLAGS "-std=c99 -g")

add_library(amidala SHARED ${LIBAMIDALA_SOURCES})
target_link_libraries(amidala m)

add_executable(amidala_app ${AMIDALA_APP_SOURCES})
target_link_libraries(amidala_app amidala)

