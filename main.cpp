#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;
string curr_user_email;
struct Task{
    int id;
    int priority;
    string creation_date;
    string deadline_date;
    vector<Task> subtasks;
    bool operator<(const Task& other){
        return priority < other.priority;
    }
};

struct User{
    string full_name;
    string email;
    string password;
    string status;
    priority_queue<Task> tasks;
};

bool saveUser(
    const string& file_name,
    const string& full_name,
    const string& email,
    const string& password
){
    json users = json::array();

    ifstream in(file_name);
    if(in.is_open()){
        in >>users;
        in.close();
    }

    json new_user;
    new_user["full_name"] = full_name;
    new_user["email"] = email;
    new_user["password"] = password;
    new_user["status"] = "user";
    new_user["tasks"] = json::array();
    users.push_back(new_user);
    ofstream out(file_name);
    out << users.dump(4);
    return true;

};
bool signup(){
        string full_name;
        string email;
        string password;
        string confirm_password;
        cout << "Enter your full name: ";
        getline(cin,full_name);
        cout << "Enter your email: ";
        getline(cin,email);
        cout << "Enter your password: ";
         getline(cin,password);
        cout << "Confirm your password: ";
         getline(cin,confirm_password);
         if(password!=confirm_password){
            return false;
         }
         if(saveUser("./db/db.json",full_name,email,password)){
            curr_user_email = email;
            return true;
         }
         return false;
}
int main(){
    int login_choice;
    cout << "Hey user welcome to AASTU TODO App !!!" << endl;
    cout << "1. Login\n2. Register" << endl;
    cin >> login_choice;
    cin.ignore();
    if(login_choice == 2){
        signup();
    }
    

    return 0;
}
