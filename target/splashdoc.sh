#! /bin/bash
PROJECT_DIRECTORY="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/.."
FOLDER="${PROJECT_DIRECTORY}/assets/splashdoc"
LOGFILE="${FOLDER}/thesis.log"
echo "" > ${LOGFILE}

{
	cd "${FOLDER}"

	FILENAME="thesis"
	ARGS="--shell-escape"

	pdflatex -halt-on-error ${ARGS} ${FILENAME}.tex
	if [ $? -eq 0 ]
	then
		bibtex ${FILENAME}.aux
		makeindex ${FILENAME}.tex
		pdflatex ${ARGS} ${FILENAME}.tex
		pdflatex ${ARGS} ${FILENAME}.tex
		rm ${FILENAME}.log
	fi
	
	cd -
} >> ${LOGFILE} 2>> ${LOGFILE}
