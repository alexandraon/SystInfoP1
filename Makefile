final:
		gcc philosophers.c -o philosophers
		gcc consom_Prod.c -o conso_prod
		gcc read_write.c -o read_write
		sh script_cons.bash
		sh script_read_write.bash
		sh script_phil.bash
clean:
	rm philosophers
	rm conso_prod
	rm read_write
	rm file_cons.csv
	rm file_phil.csv
	rm file_read.csv
