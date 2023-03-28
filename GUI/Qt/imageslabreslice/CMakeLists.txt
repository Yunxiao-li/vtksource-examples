cmake_minimum_required(VERSION 3.8...3.12 FATAL_ERROR)
project(vtkslabreslice)

set(VTK_DIR /home/jackie/works/code/opensource/vtk/vtk9.2-install/debug/lib/cmake/vtk-9.2)
find_package(VTK
  COMPONENTS
    CommonCore
    GUISupportQt
    IOImage
    ImagingColor
    ImagingGeneral
    InteractionImage
    InteractionStyle
    InteractionWidgets)
if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()

set(QT_DIR /usr/local/qt/5.15.2/gcc_64/lib)
find_package(Qt5 COMPONENTS Widgets)
if (NOT TARGET Qt5::Widgets)
  message("Skipping example: ${Qt5_NOT_FOUND_MESSAGE}")
  return ()
endif ()

# Set your files and resources here
set(Srcs
  myVTKImageSlabReslice.cpp)

set(Hdrs
  )

set(UIs
  )

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# CMAKE_AUTOMOC in ON so the MocHdrs will be automatically wrapped.
add_executable(vtkslabreslice
  ${Srcs} ${Hdrs} ${UIs})
target_link_libraries(vtkslabreslice
  PRIVATE
    ${VTK_LIBRARIES}
    )
#vtk_module_autoinit(
#  TARGETS QtVTKRenderWindows
#  MODULES ${VTK_LIBRARIES})
