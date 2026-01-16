#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;
string curr_user_email;
struct Task
{

    int id;
    string name;

    int priority;
    string creation_date;
    string deadline_date;
    vector<Task> subtasks;
    bool operator<(const Task &other) const
    {
        return priority < other.priority;
    }
};

struct User
{
    string full_name;
    string email;
    string password;
    string status;
    priority_queue<Task> tasks;
};

vector<User> users;

vector<User> loadUsers()
{
    users.clear();
    ifstream in("./db/db.json");
    if (!in.is_open() || in.peek() == ifstream::traits_type::eof())
        return users;

    json j;
    in >> j;
    in.close();

    for (auto &u : j)
    {
        User user;
        user.full_name = u["full_name"];
        user.email = u["email"];
        user.password = u["password"];
        user.status = u["status"];

        for (auto &t : u["tasks"])
        {
            Task task;
            task.id = t["id"];
            task.priority = t["priority"];
            task.name = t["name"];

            task.creation_date = t.value("creation_date", "");
            task.deadline_date = t.value("deadline_date", "");
            for (auto &st : t["subtasks"])
            {
                Task subtask;
                subtask.id = st["id"];
                subtask.name = st["name"];
                subtask.priority = st["priority"];
                subtask.creation_date = st.value("creation_date", "");
                subtask.deadline_date = st.value("deadline_date", "");
                task.subtasks.push_back(subtask);
            }
            user.tasks.push(task);
        }
        users.push_back(user);
    }
    return users;
};

void updateUsers()
{
    json r = json::array();
    for (auto &user : users)
    {
        json u;
        u["full_name"] = user.full_name;
        u["email"] = user.email;
        u["password"] = user.password;
        u["status"] = user.status;
        u["tasks"] = json::array();
        priority_queue<Task> temp = user.tasks;

        while (!temp.empty())
        {
            json j;
            Task curr = temp.top();
            j["id"] = curr.id;
            j["name"] = curr.name;
            j["priority"] = curr.priority;
            j["creation_date"] = curr.creation_date;
            j["deadline_date"] = curr.deadline_date;

            j["subtasks"] = json::array();
            for (auto &sb : curr.subtasks)
            {
                json sub;
                sub["id"] = sb.id;
                sub["priority"] = sb.priority;
                sub["name"] = sb.name;

                sub["creation_date"] = sb.creation_date;
                sub["deadline_date"] = sb.deadline_date;
                j["subtasks"].push_back(sub);
            }
            u["tasks"].push_back(j);
            temp.pop();
        }
        r.push_back(u);
    }
    ofstream out("./db/db.json");
    if (out.is_open())
    {
        out << r.dump(4);
        out.close();
    }
    loadUsers();
};
bool emailExist(string &email)
{
    for (auto &u : users)
    {
        if (u.email == email)
        {

            return true;
        }
    }
    return false;
}

bool saveUser(
    const string &full_name,
    const string &email,
    const string &password)
{
    User u;
    u.full_name = full_name;
    u.email = email;
    u.password = password;
    u.status = "user";
    users.push_back(u);
    updateUsers();
    loadUsers();
    return true;
};

bool signup()
{
    string full_name;
    string email;
    string password;
    string confirm_password;
    cout << "Enter your full name: ";
    getline(cin, full_name);
    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your password: ";
    getline(cin, password);
    cout << "Confirm your password: ";
    getline(cin, confirm_password);
    if (password != confirm_password)
    {
        cout << "Password not same" << endl;
        return false;
    }
    else if (emailExist(email))
    {
        cout << "User already Signed in" << endl;
        return false;
    }
    if (saveUser(full_name, email, password))
    {
        curr_user_email = email;
        cout << "Registered Successfully" << endl;
        return true;
    }
    return false;
}

bool login()
{

    string email;
    string password;

    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your password: ";
    getline(cin, password);

    if (!emailExist(email))
    {
        cout << "Email not found" << endl;
        return false;
    }
    for (auto &u : users)
    {
        if (u.email == email & u.password == password)
        {
            curr_user_email = u.email;
            cout << "registred successfull" << endl;
            return true;
        }
    }
    cout << "Invalid crediential" << endl;
    return false;
}

bool createTask()
{
    // TODO: Create Task with email:curr_user_email

    // take input from the user  of priority as medium high or low and change it to number as high medium or low
    // take creation_date and deadline date in specified format
    // generate unique id of length 6 must be unique not only random align it with curr date and other

    //     struct Task
    // {
    //     int id;
    //     int priority;
    //     string creation_date;
    //     string deadline_date;
    //     vector<Task> subtasks;
    //     bool operator<(const Task &other) const
    //     {
    //         return priority < other.priority;
    //     }
    // };

    // struct User
    // {
    //     string full_name;
    //     string email;
    //     string password;
    //     string status;
    //     priority_queue<Task> tasks;
    // };
}
int main()
{
    loadUsers();
    int login_choice;
    cout << "Hey user welcome to AASTU TODO App !!!" << endl;
    cout << "1. Login\n2. Register" << endl;
    cin >> login_choice;
    cin.ignore();
    if (login_choice == 2)
    {
        signup();
    }
    else if (login_choice == 1)
    {
        login();
    }

    return 0;
}
