# CMakeLists.txt for the libzoneminder-plugin-openalpr data files.
# Created by Emmanuel Papin (manupap01@gmail.com)

install (FILES openalpr.conf DESTINATION ${zm_pluginsconfdir})
install (FILES config.php DESTINATION "${zm_pluginswebdir}/${CMAKE_PROJECT_NAME}")
install (FILES en_gb.php fr_fr.php DESTINATION "${zm_pluginswebdir}/${CMAKE_PROJECT_NAME}/lang")
