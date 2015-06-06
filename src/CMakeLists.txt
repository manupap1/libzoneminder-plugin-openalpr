# CMakeLists.txt for the libzoneminder-plugin-openalpr library.
# Created by Emmanuel Papin (manupap01@gmail.com)

add_library(zm_plugin_openalpr SHARED openalpr_plugin.cpp)

TARGET_LINK_LIBRARIES(zm_plugin_openalpr ${libzoneminder_LDFLAGS} -lopenalpr -ldl -lboost_program_options -lopencv_core -lopencv_imgproc)

install (TARGETS zm_plugin_openalpr DESTINATION ${zm_pluginslibdir})
