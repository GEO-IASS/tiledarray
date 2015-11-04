if(ENABLE_MONDRIAAN)

  if(ENABLE_MONDRIAAN_PATOH)
    find_package(Mondriaan REQUIRED PaToH)
    set(TILEDARRAY_HAS_PATOH TRUE)
  else()
    find_package(Mondriaan REQUIRED)
    set(TILEDARRAY_HAS_PATOH FALSE)
  endif()
    
  # Set the  build variables
  include_directories(${Mondriaan_INCLUDE_DIRS})
  list(APPEND TiledArray_CONFIG_INCLUDE_DIRS ${Mondriaan_INCLUDE_DIRS})
  set(TiledArray_LIBRARIES ${Mondriaan_LIBRARIES} ${TiledArray_LIBRARIES})
  set(TiledArray_CONFIG_LIBRARIES ${Mondriaan_LIBRARIES} ${TiledArray_CONFIG_LIBRARIES})
  set(TILEDARRAY_HAS_MONDRIAAN 1)
      
endif()