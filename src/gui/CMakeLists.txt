set(GUI_SOURCES mainwindow.cpp mainwindow.ui)

add_library(atomizer_gui STATIC ${GUI_SOURCES} ${CMAKE_SOURCE_DIR}/include/gui/mainwindow.hpp)
target_link_libraries(atomizer_gui PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)