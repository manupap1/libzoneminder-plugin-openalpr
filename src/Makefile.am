AUTOMAKE_OPTIONS = gnu

pkglib_LTLIBRARIES = libzm_plugin_openalpr.la

AM_CPPFLAGS = \
	$(LIBZMPLUGINS_CFLAGS) $(OPENCV_CFLAGS)

libzm_plugin_openalpr_la_SOURCES = \
	openalpr_plugin.cpp

libzm_plugin_openalpr_la_LDFLAGS = \
	-avoid-version -module -shared -export-dynamic

libzm_plugin_openalpr_la_LIBADD = \
	$(LIBZMPLUGINS_LIBS) -lopenalpr

noinst_HEADERS = \
	openalpr_plugin.h
