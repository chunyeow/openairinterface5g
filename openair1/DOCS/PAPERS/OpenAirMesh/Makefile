all: dvi pdf

dvi:  
	latex OpenAirMesh.tex
	bibtex OpenAirMesh
	latex OpenAirMesh.tex
	latex OpenAirMesh.tex

pdf:
	dvips -Ppdf -ta4 OpenAirMesh.dvi
	ps2pdf -dCompatibilityLevel=1.4 -dSubsetFonts=true -dEmbedAllFonts=true -dPDFSETTINGS=/printer OpenAirMesh.ps

clean:
	rm -f OpenAirMesh.aux
	rm -f OpenAirMesh.bbl
	rm -f OpenAirMesh.blg
	rm -f OpenAirMesh.dvi
	rm -f OpenAirMesh.log
	rm -f OpenAirMesh.toc
	rm -f OpenAirMesh.ps

superclean: clean
	rm -f *.bak
	rm -f *.backup
	rm -f *~
	rm -f OpenAirMesh.pdf
