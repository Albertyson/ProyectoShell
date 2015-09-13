// archivo: syscalls.cpp
// descripción: Demostración en C++ sobre implementación de comandos 'shell' empleando llamadas de sistema.

 
// <unistd.h> contiene funciones 'wrapper' de las llamadas de sistema como mkdir, rmdir, etc.
#include <unistd.h>
#include <sys/stat.h> 
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>



namespace shell {

using namespace std;

bool es_valido(const std::string& comando);
string leer_comando();
void ejecutar_comando(const string& comando);

}

int main()
{
	using namespace std;
	using namespace shell;

	string comando {};
	// mientras el comando exit no se haya ingresado
	while (comando != "exit") {
		// restablecer comando
		comando = "";
		// mientras el comando leído sea inválido
		while (!es_valido(comando)) {
			// leer comando
			comando = leer_comando();
		}
		// ejecutar comando leído
		ejecutar_comando(comando);
	}

	return 0;
}

namespace shell {

string nombre_comando(const string& comando);
void pwd(const string& comando);
void mkdir(const string& comando);
void rmdir(const string& comando);
//void rmdir_-R(const string& comando);
void rm(const string& comando);
string argumento_comando(const string& comando);

bool es_valido(const string& comando)
{
	vector<string> comandos_validos {"mkdir", "rmdir","rmdir_-R", "rm", "exit","pwd"};

	// si el nombre del comando está en la lista de comandos válidos
	string nombre = nombre_comando(comando);
	vector<string>::iterator busqueda_comando = find(comandos_validos.begin(), comandos_validos.end(), nombre);

	if (busqueda_comando != comandos_validos.end())
		// devolver true
		return true;
	// sino
	else
		return false;
		// devolver false

}

string leer_comando()
{
	string comando;

	cout << "mi_sh>";
	getline(cin, comando);

	return comando;
}

void ejecutar_comando(const string& comando)
{
	string nombre = nombre_comando(comando);
	// si comando es "mkdir"
	if (nombre == "mkdir")
		// ejecutar comando mkdir
		mkdir(comando);
	// si comando es "pwd"
	/*else if (nombre == "pwd")
		// ejecutar comando rmdir
		pwd(comando);*/
	// si comando es "rmdir"
	else if (nombre == "rmdir")
		// ejecutar comando rmdir
		rmdir(comando);
	// si comando es "rmdir -R"
	/*else if (nombre == "rmdir_-R")
		// ejecutar comando rmdir
		rmdir_-R(comando);*/
	// sino si el comando es "rm"
	else if (nombre == "rm")
		// ejecutar comando rm
		rm(comando);
}

string nombre_comando(const string& comando)
{
	// si el comando no es cadena vacía
	if (comando.size() > 0) {
		// devolver el primer token del comando
		char cadena_comando[comando.size()];
		strcpy(cadena_comando, comando.c_str());
		char* nombre = strtok(cadena_comando, " ");
		return string {nombre};
	}
	else
		// devolver cadena vacía
		return "";
}

/*void pwd(const string& comando)
{
	// obtener el nombre del directorio
	string directorio = argumento_comando(comando);
	// eliminar el directorio invocando la llamada del sistema (system call)
	::pwd(directorio.c_str());
}*/

void mkdir(const string& comando)
{
	// obtener el nombre del directorio
	string directorio = argumento_comando(comando);
	// crear el directorio invocando la llamada del sistema (system call)
	::mkdir(directorio.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
}

void rmdir(const string& comando)
{
	// obtener el nombre del directorio
	string directorio = argumento_comando(comando);
	// eliminar el directorio invocando la llamada del sistema (system call)
	::rmdir(directorio.c_str());
}

/*void rmdir_-R(const string& comando)
{
	// obtener el nombre del directorio
	string directorio = argumento_comando(comando);
	// eliminar el directorio invocando la llamada del sistema (system call)
	::rmdir(directorio.c_str());
}*/

void rm(const string& comando)
{
	// obtener el nombre del archivo
	string archivo = argumento_comando(comando);
	// eliminar el archivo invocando la llamada del sistema (system call)
	::remove(archivo.c_str());
}

string argumento_comando(const string& comando)
{
	char cadena_comando[comando.size()];
	strcpy(cadena_comando, comando.c_str());
	// leer el primer token del comando
	char* token = strtok(cadena_comando, " ");

	// mientras no se haya encontrado el fin del comando
	while (token != NULL) {
		// leer el siguient token
		token = strtok(NULL, " ");
		// si el token actual no es una opción
		if (token[0] != '-')
			// devolver el token actual
			return string {token};
	}

	// devolver cadena vacía
	return "";
}

}
