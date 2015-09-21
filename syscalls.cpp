// archivo: syscalls.cpp
// descripción: Demostración en C++ sobre implementación de comandos 'shell' empleando llamadas de sistema.

 
// <unistd.h> contiene funciones 'wrapper' de las llamadas de sistema como mkdir, rmdir, etc.
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/utsname.h> 
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <regex>
#include <curses.h>
#include <sstream>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#define ENTER 10
#define BACKSPACE 127
#define TAB 9
namespace shell {
	using namespace std;
	bool es_valido(const std::string& comando);
	string leer_comando();
	void ejecutar_comando(const string& comando);
	void autoCompletar(string& comando);
}
using namespace std;
string miSh="Mi_sh>";
bool salir=false;
int main()
{
	using namespace std;
	using namespace shell;
	initscr();
	string comando {};
	miSh="Mi_sh>";
	comando = "";
	char key;
	// mientras el comando exit no se haya ingresado
	while (!salir) {
		erase();
		printw(miSh.c_str());
		key=getch();
		if(key==TAB){
			autoCompletar(comando);
		}else if(key==ENTER){
			if(es_valido(comando)){
				// ejecutar comando leído
				ejecutar_comando(comando);
				comando="";
				miSh+="\nMi_sh>";
			}else{
				comando="";
				miSh+="\nError:El comando no es valido\nMi_sh>";
			}
		}else if(key==BACKSPACE){
			if( comando.length() > 0 ){
				//borrar el ultimo caracter del comando
                comando = comando.substr( 0, comando.length()-1);
                //variable temporal para eliminar la ultima linea y sobreescribirla con el comando si el ultimo caracter
                string linea="Mi_sh>" + comando;
                miSh=miSh.substr(0,miSh.length()-linea.length()-1);
                miSh += "Mi_sh>" + comando;
            }
		}else{//cualquier otra tecla
			comando+=key;
			miSh+=key;
		}
	}
	endwin();
	return 0;
}

namespace shell {
	string nombre_comando(const string& comando);
	string pwd();
	void mkdir(const string& comando);
	void rmdir(const string& comando);
	void rmdir_R(const string& comando);
	void delete_folder_tree(const char* directory_name);
	int path_is_directory(const char* path);
	void rm(const string& comando);
	string argumento_comando(const string& comando);
	string opcion_comando(const string& comando);
	void rmdir_R(const string& comando);
	void delete_folder_tree(const char* directory_name);
	int path_is_directory(const char* path);
	string ls(const string& comando);
	void chmod(const string& comando);
	string uname(const string& comando);
	string salidaDeComando(const string& comando);
	vector<string> comandos_validos {"mkdir", "rmdir", "rm", "exit","pwd","ls","chmod","uname"};

	bool es_valido(const string& comando){
		
		// si el nombre del comando está en la lista de comandos válidos
		string nombre = nombre_comando(comando);
		vector<string>::iterator busqueda_comando = find(comandos_validos.begin(), comandos_validos.end(), nombre);
		if (busqueda_comando != comandos_validos.end())
			return true;
		else
			return false;
	}

	void ejecutar_comando(const string& comando)
	{
		std::size_t found=comando.find(">>");
		if(found!=std::string::npos){// encuentra >>
			char cadena_comando[comando.size()];
			strcpy(cadena_comando, comando.c_str());
			// leer el primer token del comando
			char* token = strtok(cadena_comando, ">>");
			// mientras no se haya encontrado el fin del comando
			if (token != NULL) {
				// leer el siguient token
				token = strtok(NULL, ">>");
				if(token!=NULL){
					//pasar la salida a un archivo, token tiene el nombre del archivo
					std::ofstream outfile;
					outfile.open(token, std::ios_base::app);
					outfile << salidaDeComando(comando); 
				}else{//error falta argumento
					miSh+="Error falta argumento\n";				
				}
			}
			return;
		}
		found=comando.find(">");
		if(found!=std::string::npos){// encuentra >
			//parsear por >
			char cadena_comando[comando.size()];
			strcpy(cadena_comando, comando.c_str());
			// leer el primer token del comando
			char* token = strtok(cadena_comando, ">");
			// mientras no se haya encontrado el fin del comando
			if (token != NULL) {
				// leer el siguient token
				token = strtok(NULL, ">");
				if(token!=NULL){
					//pasar la salida a un archivo, token tiene el nombre del archivo
					int fd;
				    int defout;				 
				    if ((defout = dup(1)) < 0){
				        fprintf(stderr, "Can't dup(2) - (%s)\n", strerror(errno));
				        exit(1);
				    }
				    if ((fd = open(token, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) < 0){
				        fprintf(stderr, "Can't open(2) - (%s)\n", strerror(errno)); exit(1);
				    }
				    if (dup2(fd, 1) < 0) // redireccionar la salida al archivo
				    {
				        fprintf(stderr, "Can't dup2(2) - (%s)\n", strerror(errno)); exit(1);
				    }
				    close(fd);  //Cerrar archivo
				    if (puts(salidaDeComando(comando).c_str()) < 0)
				    {
				        fprintf(stderr, "Can't printf(3) to fd=%d - (%s)\n", fileno(stdout), strerror(errno)); exit(1);
				    }
				    fflush(stdout);// flush el output al archivo
				    // Ahora stdout esta limpio para otro uso
				    if (dup2(defout, 1) < 0) // redireccionar el output de nuevo a stdout
				    {
				        fprintf(stderr, "Can't dup2(2) - (%s)\n", strerror(errno)); exit(1);
				    }
				    close(defout);  // La copia de stdout no se necesita mas
				    return;
				}else{//error falta argumento
					miSh+="Error falta argumento\n";				
				}
			}
			return;
		}
		found=comando.find("|");
		if(found!=std::string::npos){// encuentra |
			//parsear por |
			char cadena_comando[comando.size()];
			strcpy(cadena_comando, comando.c_str());
			// leer el primer token del comando
			char* token = strtok(cadena_comando, "|");
			// mientras no se haya encontrado el fin del comando
			if (token != NULL) {
				// leer el siguient token
				token = strtok(NULL, "|");
				if(token!=NULL){
					//pasar la salida a el siguiente comando
					ejecutar_comando(salidaDeComando(token));
				}else{//error falta argumento
					miSh+="Error falta argumento\n";				
				}
			}
			return;
		}else{//si no encuentra > ni |
			miSh+=salidaDeComando(comando);
		}
		
	}

	string salidaDeComando(const string& comando){
		string nombre = nombre_comando(comando);
		// si comando es "mkdir"
		if (nombre == "mkdir"){
			// ejecutar comando mkdir
			mkdir(comando);
			return "";
		}
		// si comando es "pwd"
		else if (nombre == "pwd"){
			// ejecutar comando rmdir
			return pwd();
		}
		// si comando es "rmdir"
		else if (nombre == "rmdir"){
		    rmdir(comando);
		    return "";
		}else if (nombre =="rmdir -r"){
			rmdir_R(comando);
			return "";
		}
		// sino si el comando es "rm"
		else if (nombre == "rm"){
			// ejecutar comando rm
			rm(comando);
			return "";
		}
		else if (nombre == "ls"){
			// ejecutar comando ls
			return ls(comando);
		}
		else if (nombre == "chmod"){
			chmod(comando);
			return "";
		}
		else if (nombre == "uname"){
			return uname(comando);
		}else if (nombre == "exit"){
			salir=true;
			return "";
		}
		return "";
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

	string pwd()
	{
		string path = getenv("PWD");
		return path;
	}

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
			if(token==NULL)
				return "";
			// si el token actual no es una opción
			if (token[0] != '-')
				// devolver el token actual
				return string {token};
			else
				return string {token};
		}
		// devolver cadena vacía
		return "";
	}

	string opcion_comando(const string& comando)
	{
		
		char cadena_comando[comando.size()];
		strcpy(cadena_comando, comando.c_str());
		// leer el primer token del comando
		char* token = strtok(cadena_comando, " ");

		// mientras no se haya encontrado el fin del comando
		while (token != NULL) {
			// leer el siguient token
			token = strtok(NULL, " ");
			// si el token actual es una opción
			if (token[0] == '-')
				// devolver el token actual
				return string {token};
		}
		// devolver cadena vacía
		return "";

	}


	void rmdir_R(const string& comando)
	{
		// obtener el nombre del directorio
		string directorio = argumento_comando(comando);
		delete_folder_tree(directorio.c_str());
		
	}


	void delete_folder_tree(const char* directory_name){
		 DIR*            dp;
		 struct dirent*  ep;
		 char            p_buf[512] = {0};

		 dp = opendir(directory_name);

		 while ((ep = readdir(dp)) != NULL) {
		 cout<<(p_buf, "%s/%s", directory_name, ep->d_name)<<endl;
		 if (path_is_directory(p_buf))
		     delete_folder_tree(p_buf);
		 else
		     unlink(p_buf);
	     	 }

		closedir(dp);
		rmdir(directory_name);

	}

	int path_is_directory (const char* path) {
	    struct stat s_buf;

	    if (stat(path, &s_buf))
	        return 0;

	    return S_ISDIR(s_buf.st_mode);
	}

	string ls(const string& comando)
	{
		// obtener los nombres de los archivos presentes
		//ver archivos
		DIR *dp;
		struct dirent *ep;
		dp = opendir ("./");
		if (dp != NULL){
			string files="";
			while (ep = readdir (dp)){
				files+=ep->d_name;
				files+="\n";
			}
			(void) closedir (dp);
			return files;
		}else{
			return "Couldn't open the directory";
		}
	}

	void chmod(const string& comando){
		//obtener los permisos para el archivo
		string permisos=argumento_comando(comando);
		//validar si los permisos estan bien escritos
		regex rxPermisos("(((u|g)|(o|a))+(MAS|MENOS)((r|w)|(x))+)");
		if(regex_match(permisos,rxPermisos)){
			//Los permisos han sido bien escritos
			//Obtener el nombre del archivo
			string parserEspecial="chmod "+permisos+" ";
			char copiaComando[comando.size()];
			strcpy(copiaComando, comando.c_str());
			char copiaParser[parserEspecial.size()];
			strcpy(copiaParser, parserEspecial.c_str());
			char* nombreArchivo = strtok(copiaComando, copiaParser);
			//Cambiar permisos
			string path=pwd()+"/"+string{nombreArchivo};
			std::size_t found=permisos.find("u");
			if(found!=std::string::npos){//si encuentra el caracter u:usuario
				found=permisos.find("r");
				if(found!=std::string::npos){//si encuentra el caracter r:lectura
					::chmod(path.c_str(),S_IRUSR);
				}
				found=permisos.find("w");
				if(found!=std::string::npos){//si encuentra el caracter w:escritura
					::chmod(path.c_str(),S_IWUSR);
				}
				found=permisos.find("x");
				if(found!=std::string::npos){//si encuentra el caracter x:ejecucion
					::chmod(path.c_str(),S_IXUSR);
				}
			}
			found=permisos.find("g");
			if(found!=std::string::npos){//si encuentra el caracter g:group
				found=permisos.find("r");
				if(found!=std::string::npos){//si encuentra el caracter r:lectura
					::chmod(path.c_str(),S_IRGRP);
				}
				found=permisos.find("w");
				if(found!=std::string::npos){//si encuentra el caracter w:escritura
					::chmod(path.c_str(),S_IWGRP);
				}
				found=permisos.find("x");
				if(found!=std::string::npos){//si encuentra el caracter x:ejecucion
					::chmod(path.c_str(),S_IXGRP);
				}
			}
			found=permisos.find("o");
			if(found!=std::string::npos){//si encuentra el caracter o:other
				found=permisos.find("r");
				if(found!=std::string::npos){//si encuentra el caracter r:lectura
					::chmod(path.c_str(),S_IROTH);
				}
				found=permisos.find("w");
				if(found!=std::string::npos){//si encuentra el caracter w:escritura
					::chmod(path.c_str(),S_IWOTH);
				}
				found=permisos.find("x");
				if(found!=std::string::npos){//si encuentra el caracter x:ejecucion
					::chmod(path.c_str(),S_IXOTH);
				}
			}
		}else{
			miSh+="\nError: Los permisos no son correctos\nComo usar el comando: chmod <u|g|o|a><MAS|MENOS><r|w|x>\n\tu:Usuario\n\tg:Grupo\n\to:Otros\n\ta:Todos\n\tMAS:Agregar\n\tMENOS:Quitar\n\tr:Lectura\n\tw:Escritura\n\tx:Ejecucion\n\tPulse una tecla para continuar...\n";
			getch();
		}
	}
	string uname(const string& comando){
		struct utsname sysinfo;
		uname(&sysinfo);
		string flag=argumento_comando(comando);
		string retVal=sysinfo.sysname;
		if(flag.length()>0){
			if(flag=="-a"){
				stringstream ss;
				ss<<sysinfo.sysname<<" "<<sysinfo.nodename<<" "<<sysinfo.release<<" "<<sysinfo.version<<" "<<sysinfo.machine;
				retVal=ss.str();
			}else if(flag=="-r"){
				retVal=sysinfo.release;
			}else if(flag=="-m"){
				retVal=sysinfo.machine;
			}else if(flag=="-s"){
				retVal=sysinfo.sysname;
			}
		}
		return retVal;
	}
	void autoCompletar(string& comando){
		for(int i=0;i<comandos_validos.size();i++){
			string comandoActual=comandos_validos.at(i);
			/*std::size_t found=comandoActual.find(comando.c_str());
			if(found!=std::string::npos){
				miSh = miSh.substr(0, miSh.length()-comando.length());
				comando=comandos_validos.at(i).c_str();
				miSh+=comando;
				break;
			}*/
			if(comando.substr(0,comando.length())==comandoActual.substr(0,comando.length())){
				miSh = miSh.substr(0, miSh.length()-comando.length());
				comando=comandos_validos.at(i).c_str();
				miSh+=comando;
				break;
			}
		}
	}

}
