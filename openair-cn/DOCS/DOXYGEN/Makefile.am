if HAVE_DOXYGEN
directory = $(top_srcdir)/DOCS/Documentation

if HAVE_PDFLATEX
	doc_DATA = $(directory)oaisim_mme.pdf
else
	doc_DATA = $(directory)
endif
$(directory): doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -f doxyfile.stamp
	rm -rf $(top_srcdir)/DOCS/Documentation/html
	rm -rf $(top_srcdir)/DOCS/Documentation/latex
endif
