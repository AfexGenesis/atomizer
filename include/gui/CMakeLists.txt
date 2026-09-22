set(GUI_HEADERS mainwindow.hpp)

add_library(atomizer_gui STATIC ${GUI_HEADERS})
target_link_libraries(atomizer_gui PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)