#include "../funcionesFS.h"

void consolaFS(){
	puts("Bienvenido a la consola. Ingrese un comando:");
	while(1){
		char *linea = readline(">");
		add_history(linea);
		procesarInput(linea);
	}
}

void procesarInput(char* linea) {
	int cantidad = 0;
	char **palabras = string_split(linea, " ");
	cantidad = cantidadParametros(palabras);
	if (string_equals_ignore_case(*palabras, "format")) {
		consolaFormat(palabras, cantidad);
	} else if (string_equals_ignore_case(*palabras, "rm")) {
		consolaRemove(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "rename")) {
		consolaRename(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "mv")) {
		consolaMove(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "cat")) {
		consolaCat(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "mkdir")) {
		consolaMkdir(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "cpfrom")) {
		consolaCpfrom(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "cpto")) {
		if(verificarRutaArchivo(palabras[1])){
			if(copiarArchivo(palabras) != -1){
				puts("Se copio el archivo, bro.");
				return;
			}
			else
			{
				puts("No se pudo copiar el archivo, bro.");
			}
		}
	} else if (string_equals_ignore_case(*palabras, "cpblock")) {
		procesarCpblock(palabras);
	} else if (string_equals_ignore_case(*palabras, "md5")) {
		consolaMd5(palabras,cantidad);

	} else if (string_equals_ignore_case(*palabras, "ls")) {
		consolaLs(palabras,cantidad);
	} else if (string_equals_ignore_case(*palabras, "info")) {
		consolaInfo(palabras,cantidad);
	} else if(string_equals_ignore_case(*palabras, "exit")){
		printf("Finalizando consola\n");
		liberarPunteroDePunterosAChar(palabras);
		free(palabras);
		free(linea);
		pthread_kill(pthread_self(),SIGKILL);
	} else {
		printf("No existe el comando\n");
	}
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);
	free(linea);

}

void procesarCpblock(char ** palabras){
	if(verificarRutaArchivo(palabras[1])){
		char * rutaLocalArchivo = obtenerRutaLocalDeArchivo(palabras[1]);
		Tnodo *nodo = buscarNodoPorNombre(listaDeNodos, palabras[3]);
		if(nodo != NULL){
			if(obtenerBloqueDisponible(nodo) != -1){
				Tarchivo* tablaArchivo = malloc(sizeof(Tarchivo));
				int nroBloque = atoi(palabras[2]);
				Tbuffer* bloque;
				TbloqueAEnviar* bloqueAEnviar;
				levantarTablaArchivo(tablaArchivo, rutaLocalArchivo);
				free(rutaLocalArchivo);
				if (nroBloque>=cantidadDeBloquesDeUnArchivo(tablaArchivo->tamanioTotal)) {
					puts("Numero de bloque incorrecto");
					liberarTablaDeArchivo(tablaArchivo);
					return;
				}
				if (nodosDisponiblesParaBloqueDeArchivo(tablaArchivo, nroBloque) == 0) {
					puts("No se encontraron los nodos con las copias del bloque");
					liberarTablaDeArchivo(tablaArchivo);
					return;
				}
				pthread_mutex_init(&bloqueMutex, NULL);
				if (pedirBloque(tablaArchivo, nroBloque) == -1) {
					puts("Error al solicitar bloque");
					liberarTablaDeArchivo(tablaArchivo);
					return;
				}
				liberarTablaDeArchivo(tablaArchivo);
				pthread_mutex_lock(&bloqueMutex);
				pthread_mutex_lock(&bloqueMutex);

				bloque = malloc(sizeof(Tbuffer));
				if (copiarBloque(bloqueACopiar, bloque) == -1) {
					puts("Error al copiar bloque recibido");

				}
				liberarEstructuraBuffer(bloqueACopiar);
				bloqueAEnviar = malloc(sizeof(TbloqueAEnviar));
				bloqueAEnviar->contenido = bloque->buffer;
				bloqueAEnviar->tamanio = bloque->tamanio;
				bloqueAEnviar->numeroDeBloque = nroBloque;
				if (enviarBloqueA(bloqueAEnviar, palabras[3]) == -1) {
					puts("Error no se pudo enviar el bloque");
					liberarEstructuraBuffer(bloque);
					return;
				}
				liberarEstructuraBuffer(bloque);
				puts("Perfeeecto, el bloque se copio lo mas bien");
			}
			else{
				puts("El nodo destino esta lleno.");
			}
		}
		else {
			puts("El nodo destino no existe o no esta conectado.");
		}
	}
	else{
		puts("Ruta de archivo incorrecta.");
	}
}

void consolaFormat(char**palabras, int cantidad){
	if(cantidad == 0){
		formatearFS();
		puts("FileSystem formateado.");
	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}
void consolaRename(char** palabras, int cantidad) {
	if (cantidad == 2) {
		char ** carpetas = string_split(palabras[1], "/");
		char * yamafs = malloc(10);
		strcpy(yamafs, "yamafs:");
		int cant = contarPunteroDePunteros(carpetas);

		if (string_equals_ignore_case(carpetas[0], yamafs)) {
			puts("Falta la referencia a yamafs:/");
		} else {
			if (cant == 1) {
				puts("No se puede renombrar el root");
			} else {
				if (existeDirectorio(palabras[1]) || verificarRutaArchivo(palabras[1])) {

					renombrarArchivoODirectorio(palabras[1], palabras[2]);
				}
			}
		}
		puts("No se pudo renombrar el archivo o directorio");
		free(yamafs);

	} else {
		puts("Error en la cantidad de parametros");
	}
}

void consolaCat(char ** palabras, int cantidad) {
	if (cantidad == 1) {
		char** carpetas = string_split(palabras[1], "/");

		if(string_equals_ignore_case(carpetas[0], "yamafs:")){
			puts("Falta la referencia a yamafs:/");
		} else if (verificarRutaArchivo(palabras[1])) {
			char * rutaLocal = obtenerRutaLocalDeArchivo(palabras[1]);
			leerArchivoComoTextoPlano(rutaLocal);
		}
		else {
			puts("No existe el directorio o falta la referencia a yamafs");
		}
	}
	else {
		puts("Error en la cantidad de parametros");
	}
}

void consolaMkdir(char**palabras, int cantidad){
	if(cantidad == 1){
		char** carpetas = string_split(palabras[1], "/");
		if(!string_equals_ignore_case(carpetas[0], "yamafs:")){
			puts("Falta la referencia a yamafs:/");
		} else if(existeDirectorio(palabras[1])){
			puts("Existe el directorio");
		}
		else{
			puts("No existe el directorio");
			if(crearDirectorio(palabras[1])>=0){
				persistirTablaDeDirectorios();
			}
		}

		liberarPunteroDePunterosAChar(carpetas);
		free(carpetas);
	}
	else{
		puts("Error en la cantidad de parametros");
	}
}

void consolaCpfrom(char** palabras, int cantidad){
	if(cantidad == 2){
		if(existeDirectorio(palabras[2])){
			puts("Existe el directorio");
			almacenarArchivo(palabras);
		}
		else {
			puts("No existe el directorio");
		}
	}
	else {
		puts("Error en la cantidad de parametros");
	}
}

void consolaMd5(char** palabras, int cantidad){
	if (cantidad == 1){
		char** carpetas = string_split(palabras[1], "/");
		if(!string_equals_ignore_case(carpetas[0], "yamafs:")){
			puts("Falta la referencia a yamafs:/");
		} else{
			getMD5(palabras[1]);
		}
		liberarPunteroDePunterosAChar(carpetas);
		free(carpetas);
	}
	else {
		puts("Error en la cantidad de parametros.");
	}
}

void consolaLs(char**palabras, int cantidad){
	if(cantidad == 1){
		char** carpetas = string_split(palabras[1], "/");
		if(!string_equals_ignore_case(carpetas[0], "yamafs:")){
			puts("Falta la referencia a yamafs");
		} else if(existeDirectorio(palabras[1])){
			puts("Existe el directorio");
			listarArchivos(palabras[1]);
		}
		else{
			puts("No existe el directorio");
		}

		liberarPunteroDePunterosAChar(carpetas);
		free(carpetas);
	}
	else{
		puts("Error en la cantidad de parametros");
	}
}

void consolaInfo(char**palabras, int cantidad){
	if (cantidad == 1){
		char** carpetas = string_split(palabras[1], "/");
		if(!string_equals_ignore_case(carpetas[0], "yamafs:")){
				puts("Falta la referencia a yamafs");
		} else if(verificarRutaArchivo(palabras[1])){
			Tarchivo* tablaArchivo = malloc(sizeof(Tarchivo));
			char * rutaLocal = obtenerRutaLocalDeArchivo(palabras[1]);
			levantarTablaArchivo(tablaArchivo, rutaLocal);
			mostrarTablaArchivo(tablaArchivo);
			liberarTablaDeArchivo(tablaArchivo);
			free(rutaLocal);
		}
		else{
			puts("No existe el directorio o falta la referencia a yamafs:");
		}

		liberarPunteroDePunterosAChar(carpetas);
		free(carpetas);
	}
	else{
		puts("Error en la cantidad de parametros");
	}
}

void consolaRemove (char** palabras, int cantidad){
	if (cantidad == 1){
		char** carpetas = string_split(palabras[1], "/");
		if (!string_equals_ignore_case(carpetas[0], "yamafs:")){
				puts("Falta la referencia a yamafs:/");
		} else if(verificarRutaArchivo(palabras[1])){
			removerArchivo(palabras[1]);
		} else{
			puts("El archivo no existe en la ruta especificada");
		}

		liberarPunteroDePunterosAChar(carpetas);
		free(carpetas);

	} else if (cantidad ==2){
		if (string_equals_ignore_case(palabras[1], "-d")){
			char** carpetas = string_split(palabras[2], "/");
			if(!string_equals_ignore_case(carpetas[0], "yamafs:")){
				puts("Falta la referencia a yamafs:/");
			} else if(existeDirectorio(palabras[2])){
				if(esDirectorioRaiz(palabras[2])){
				puts("No se puede remover el directorio raíz");
			} else if(esDirectorioVacio(palabras[2])){
				removerDirectorio(palabras[2]);
				puts("Ya pude remover el directorio");
			} else{
				puts("El directorio no esta vacío. No se puede remover");
			}
			} else{
				puts("No existe el directorio");
			}

			liberarPunteroDePunterosAChar(carpetas);
			free(carpetas);

		} else if (string_equals_ignore_case(palabras[1], "-b")){
				puts("Voy a eliminar un nodo");
			//removerNodo
		}
		} else{
			puts("Error en la cantidad de parámetros");
		}

}

int getMD5(char* rutaYamafs){
	char* rutaLocal = obtenerRutaLocalDeArchivo(rutaYamafs);
	char * nombreArchivoConExtension = obtenerNombreDeArchivoDeUnaRuta(rutaLocal);
	char * rutaTmp = string_new();
	Tarchivo * archivo = malloc(sizeof(Tarchivo));
	levantarTablaArchivo(archivo,rutaLocal);

	mkdir("/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/metadata/tmp/", 0777);
	string_append(&rutaTmp, "/home/utnso/tp-2017-2c-Los-Ritchines/fileSystem/src/metadata/tmp/");
	string_append(&rutaTmp, nombreArchivoConExtension);
	levantarArchivo(archivo,rutaTmp);
	char* comando = string_duplicate("md5sum ");
	string_append(&comando, rutaTmp);
	system(comando);
	printf("Obtuve el MD5 del archivo");
	remove(rutaTmp);
	free(comando);
	free(rutaTmp);
	free(rutaLocal);

	return 0;
}

void consolaMove(char ** palabras, int cantidad){
	if(cantidad==2){
		char** carpetas1 = string_split(palabras[1], "/");
		char** carpetas2 = string_split(palabras[2], "/");
		if(!string_equals_ignore_case(carpetas1[0], "yamafs:") || !string_equals_ignore_case(carpetas2[0], "yamafs:")){
			puts("Falta la referencia a yamafs:/ en alguna de las rutas");
		} else if(verificarRutaArchivo(palabras[1])){
			moverArchivo(palabras[1], palabras[2]);
		} else{
			puts("No se quiere mover un archivo");
		}

		liberarPunteroDePunterosAChar(carpetas1);
		free(carpetas1);
		liberarPunteroDePunterosAChar(carpetas2);
		free(carpetas2);
	}else{
		puts("Error en la cantidad de parametros");
	}
}
