MYFILE=EPC

all:
	pdflatex $(MYFILE).tex
	pdflatex $(MYFILE).tex
	makeindex $(MYFILE).nlo -s nomencl.ist -o $(MYFILE).nls
	pdflatex $(MYFILE).tex

.PHONY: all
