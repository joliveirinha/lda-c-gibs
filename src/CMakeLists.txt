
set(LIBAMIDALA_SOURCES lda_model.c lda_inference.c corpus.c)
set(AMIDALA_TEST_SOURCES main.c)

add_library(amidala SHARED ${LIBAMIDALA_SOURCES})
target_link_libraries(amidala m)

add_executable(amidala_test ${AMIDALA_TEST_SOURCES})
target_link_libraries(amidala_test amidala)

