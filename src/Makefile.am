AUTOMAKE_OPTIONS = gnu

pkglib_LTLIBRARIES = libzm_plugin_openalpr.la

AM_CPPFLAGS = \
	$(libzoneminder_CFLAGS) $(opencv_CFLAGS)

libzm_plugin_openalpr_la_SOURCES = \
	openalpr_plugin.cpp

libzm_plugin_openalpr_la_LDFLAGS = \
	-avoid-version -module -shared -export-dynamic

libzm_plugin_openalpr_la_LIBADD = \
	$(libzoneminder_LIBS) -lopenalpr -ldl

noinst_HEADERS = \
	openalpr_plugin.h
