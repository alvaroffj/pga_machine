MPI_COMPILE_FLAGS = $(shell mpicc --showme:compile)
MPI_LINK_FLAGS = $(shell mpicc --showme:link)
CC = 'gcc'
main: main.o sga.h generate.o memory.o statistic.o utility.o initial.o app.o app2.o random.o master.o comunica.o rselect.o operators.o app2_g.o app2_ng.o app_g.o app_ng.o app_ir.o app2_ir.o
	$(CC) $(MPI_COMPILE_FLAGS) main.o generate.o memory.o statistic.o utility.o initial.o app.o app2.o random.o master.o comunica.o rselect.o operators.o app2_g.o app2_ng.o app_g.o app_ng.o app_ir.o app2_ir.o $(MPI_LINK_FLAGS) -lm -o main
main.o: main.c sga.h
	$(CC) -c $(MPI_COMPILE_FLAGS) main.c $(MPI_LINK_FLAGS) -o main.o
random.o: random.c random.h
	$(CC) -c $(MPI_COMPILE_FLAGS) random.c $(MPI_LINK_FLAGS) -o random.o
generate.o: generate.c generate.h
	$(CC) -c $(MPI_COMPILE_FLAGS)  generate.c $(MPI_LINK_FLAGS) -o generate.o
operators.o: operators.c operators.h
	$(CC) -c $(MPI_COMPILE_FLAGS) operators.c $(MPI_LINK_FLAGS) -o operators.o
app.o: app.c app.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app.c $(MPI_LINK_FLAGS) -o app.o
rselect.o: rselect.c rselect.h
	$(CC) -c $(MPI_COMPILE_FLAGS) rselect.c $(MPI_LINK_FLAGS) -o rselect.o
memory.o: memory.c memory.h
	$(CC) -c $(MPI_COMPILE_FLAGS) memory.c $(MPI_LINK_FLAGS) -o memory.o
utility.o: utility.c
	$(CC) -c $(MPI_COMPILE_FLAGS) utility.c $(MPI_LINK_FLAGS) -o utility.o
statistic.o: statistic.c statistic.h
	$(CC) -c $(MPI_COMPILE_FLAGS) statistic.c $(MPI_LINK_FLAGS) -o statistic.o
initial.o: initial.c initial.h
	$(CC) -c $(MPI_COMPILE_FLAGS) initial.c $(MPI_LINK_FLAGS) -o initial.o
app2.o: app2.c app2.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app2.c $(MPI_LINK_FLAGS) -o app2.o
master.o: master.c master.h
	$(CC) -c $(MPI_COMPILE_FLAGS) master.c $(MPI_LINK_FLAGS) -o master.o
comunica.o: comunica.c comunica.h
	$(CC) -c $(MPI_COMPILE_FLAGS) comunica.c $(MPI_LINK_FLAGS) -o comunica.o
app2_g.o: app2_g.c app2_g.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app2_g.c $(MPI_LINK_FLAGS) -o app2_g.o
app2_ir.o: app2_ir.c app2_ir.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app2_ir.c $(MPI_LINK_FLAGS) -o app2_ir.o
app2_ng.o: app2_ng.c app2_ng.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app2_ng.c $(MPI_LINK_FLAGS) -o app2_ng.o
app_g.o: app_g.c app_g.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app_g.c $(MPI_LINK_FLAGS) -o app_g.o
app_ir.o: app_ir.c app_ir.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app_ir.c $(MPI_LINK_FLAGS) -o app_ir.o
app_ng.o: app_ng.c app_ng.h
	$(CC) -c $(MPI_COMPILE_FLAGS) app_ng.c $(MPI_LINK_FLAGS) -o app_ng.o

