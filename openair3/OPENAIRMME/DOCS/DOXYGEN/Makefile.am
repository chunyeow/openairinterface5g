if HAVE_DOXYGEN
directory = $(top_srcdir)/DOCS/Documentation

doc_DATA = $(directory)
if HAVE_PDFLATEX
	doc_DATA += oaisim_mme.pdf
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
