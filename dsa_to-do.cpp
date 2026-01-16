#include <iostream>
#include<json.hpp>
#include<fstream>
using namespace std;
using json = nlohmann::json;
vector<User> users;
void registerToJson() {
    json data;
    data["users"] = json::array();
    for (auto& u : users) {
        json j;
        j["fullName"] = u.full_name;
        j["email"] = u.email;
        j["password"] = u.password;
        j["status"] = u.status; 
        j["tasks"] = json::array();
        data["users"].push_back(j);
    }
    ofstream outFile("data.json");
    outFile << data.dump(4);
    outFile.close();
}
void LoadFromJson() {
    ifstream inFile("data.json");
    if (!inFile.is_open()) return;

    json data;
    inFile >> data;
    inFile.close();

    users.clear();

    for (const auto& u : data["users"]) {
        User user;
        user.fullName = u["full_name"];
        user.email = u["email"];
        user.password = u["password"];
        user.status = u["status"];
        users.push_back(user);
    }
}
bool emailExists(const string& email) {
    ifstream in("data.json");
    if (!in.is_open()) {
        return false;
    }
    json data;
    in >> data;
    in.close();

    if (!data.contains("users")) return false;

    for (const auto& j : data["users"]) {
        if (j["email"] == email) {
            return true;
        }
    }
    return false;
}
bool Login(const string& email, const string& password) {
    ifstream in("data.json");

    if (!in.is_open()) {
        cout << "No users found.\n";
        return false;
    }

    json data;
    in >> data;
    in.close();

    if (!data.contains("users")) {
        cout << "No users found.\n";
        return false;
    }

    for (const auto& u : data["users"]) {
        if (u["email"] == email && u["password"] == password) {
            cout << "Login successful.\n";
            return true;
        }
    }

    cout << "Invalid email or password.\n";
    return false;
}

int main() {
    return 0;
}
