#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <iomanip>

using namespace std;

//функция вывода меню
void showMenu()
{
	
    cout << "" << endl;
    cout << "---------------MENU --------------" << endl;
    cout << "1-DISK IN SYSTEMS :" << endl;
    cout << "2-FORMAT DISK :" << endl;
    cout << "3-DISK INTEGRITY CHECK :" << endl;
    cout << "4-LABEL ON FILE :" << endl;
    cout << "5-CHECK FILE :" << endl;
    cout << "6-EXIT" << endl;
    cout << "--------------------------------" << endl;
	
}

//структура, хранящие данные о смонтированных дисках в системе
struct mount {

    std::string device;
    std::string destination;
    std::string fstype;
    std::string options;
    int dump;
    int pass;
	
};

//оператор, возвращающий форматированную строку для вывода информации о дисках в системе
std::ostream& operator<<(std::ostream& stream, const mount& mount) {
	
    return stream <<" DISC: "<<mount.device << " || " << " MOUNTED ON: " <<mount.destination << " || " << " OPTIONS: " <<mount.options << " || " << " DUMP:"<<mount.dump;
	
}

//функция сбора данных о смонтированных дисках
int getDiskList()
{

	//файл, хранящий данные о дисках
    std::ifstream mountinfo("/proc/mounts");
	
	//парсинг файла
    while( !mountinfo.eof() ) {
        mount each;
        mountinfo >> each.device >> each.destination >> each.fstype >> each.options >> each.dump >> each.pass;
        if( each.device != "" )
            std::cout<<"--------------------------------------------------------------------------------------" << std::endl;
            std::cout << each << std::endl;
    }
    return 0;	

}

//функция форматирования диска
int formatDisc()
{

    char buf[8192];
    string diskName;
    cout << "Enter a disk name: ";
	
	//ввод имени диска
    cin >> diskName;
	
	//открытие потока чтения
    FILE *f = fopen(diskName.c_str(), "w");

    if(f == NULL) {

       perror("Could not open file for writing");
       return 0;

    }

    long unsigned int written = 0;
	
	//цикл форматирования диска
    while(true) {

        unsigned count = fwrite(buf, sizeof(buf), 1, f);
        written += count;

        if(count != sizeof(buf)) {

            perror("Could not write file");
            break;

        };
    } 

	//закрытие потока чтения
    fclose(f);

    cout << "" << endl;
    cout << "Formatted" << endl;
    cout << "" << endl;
    return 0;

}

//функция перенаправления управления терминалу
std::string exec(const char* cmd) {

	//входной буффер для команды
    char buffer[128];
    std::string result = "";
	
	//открытие терминала
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    try {
		//цикл приема результата
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }

    pclose(pipe);
    return result;

}




//функция хэширования
std::string md5(const std::string &str){

  unsigned char hash[MD5_DIGEST_LENGTH];

  MD5_CTX md5;

  MD5_Init(&md5);

  MD5_Update(&md5, str.c_str(), str.size());

  MD5_Final(hash, &md5);

  std::stringstream ss;

  for(int i = 0; i < MD5_DIGEST_LENGTH; i++){

    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( hash[i] );
  }

  return ss.str();
}

//функция чтения файла
string readFile(const string& path) {

	//поток считывания файла
    ifstream input_file(path);

    if (!input_file.is_open()) {

        cerr << "Could not open the file - '"

             << path << "'" << endl;

        exit(EXIT_FAILURE);
    }
	
	//возврат результата в string
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

//функция записи в файл
int writeFile(string fileName, string hash)
{
	// поток для записи
    std::ofstream out;
// открываем файл для записи	
    out.open("hash.txt");      

    if (out.is_open())
    {
		//запись строки
        out << fileName + " " + hash << std::endl;

    }
	
	//закрытие потока записи
    out.close(); 
}

//построчное считывание файла
string readFileStr(string fileName, string hash)
{

	cout<<fileName<<endl;
	cout<<hash<<endl;
	
	// открытие файл для чтения
    std::ifstream file("hash.txt"); 

	//проверка на открытость
    if (!file.is_open()) 
	{

        std::cerr << "Failed to open the file." << std::endl;
        return " ";

    }

    std::string line;
	
	//построчное чтение файла
    while (std::getline(file, line)) {

		//набор условий для сравнения хэш-значений
    	if(strstr(line.c_str(),fileName.c_str()))
        {
        	if(strstr(line.c_str(),hash.c_str()))
        	{

        		return "NOMODIFY";

			}

        		return "MODIFY"; 
        	}
    }
	//закрытие потока
    file.close(); 

    return "NO";

}

//точка входа в программу
int main()
{

    int option;
    double balance = 500;

    do
    {
		//вывод меню
        showMenu();
        cout << "Option: ";
        cin >> option;
        cout << "\033[2J\033[1;1H";

		//обработка выбора в меню приложения
        switch(option)
        {
			// вывод списка смонтированных дисков
           case 1:
             getDiskList();	
             break;

			//форматирование диска
           case 2:
             formatDisc();
             break;

			//проверка диска на целостность
           case 3:
           {
			   
			string diskName = "sudo badblocks -s /";
			string disk;

			cout << "Enter a disk name: ";
			cin>>disk;
			
			string diskNameFinally = diskName+disk;
			cout << exec(diskNameFinally.c_str());
			cout << " " << endl;
            break;
			
            }
			
			//установка метки отслеживания целостности на файл
            case 4:
            {
				string fileName = "";
				cout<<"Enter a file name:";
				cin>>fileName;

				string file = readFile(fileName);
				writeFile(fileName, md5(file));
				cout<<"The label is installed"<<endl;
				break;

            }
			
			//проверка целостности файла
             case 5:
             {
             	string fileName = "";
				cout<<"Enter a file name:";
				cin>>fileName;
				
				//вызов функции чтения файла
				string file = readFile(fileName);
				
				//вызов функции хэширования
				string hash = md5(file);
				string answ = readFileStr(fileName, hash);

				//набор условий для выбора результата
				if(answ == "NO")
				{
					cout<<"Label does not exist"<<endl;
				}

				else if(answ == "NOMODIFY")
				{
					cout<<"The file has not been changed"<<endl;
				}

				if(answ == "MODIFY")
				{
					cout<<"The file has been modified"<<endl;
				}
						break;

			 }
			//выход из программы
			 case 6:
			 {
             	return 0;
             	break;
			 }

           default:
		   
              cout << "\033[2J\033[1;1H"; 
              cout << "Your choice is invalid " << endl;
              cin.clear();
              cin.ignore();
			}

    } while(option != 6);

    return 0;

}
