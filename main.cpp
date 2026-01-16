#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <random>
#include "json.hpp"
#include <ctime>
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
User *curr_user = nullptr;

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
        for (auto &u : users)
        {
            if (u.email == curr_user_email)
            {
                curr_user = &u;
            }
        }
        cout << "Registered Successfully" << endl;
        return true;
    }
    return false;
}
string getCurrentDate()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    int year = 1900 +  ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    return to_string(year) + "-" +
    (month < 10 ? "0": "") + to_string(month) + "-" +
    (day < 10 ? "0": "") + to_string(day);
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
        if (u.email == email && u.password == password)
        {
            curr_user_email = u.email;
            for (auto &u : users)
            {
                if (u.email == curr_user_email)
                {
                    curr_user = &u;
                }
            }
            cout << "registred successfull" << endl;
            return true;
        }
    }
    cout << "Invalid crediential" << endl;
    return false;
}
int generateRandomDigit()
{
    static random_device rd;     // seed source (non-deterministic)
    static mt19937_64 gen(rd()); // Mersenne Twister engine
    static uniform_int_distribution<int> dist(100000, 999999);

    return dist(gen);
}

bool createTask()
{
    Task new_task;
    int id;
    string name;
    int priority;
    string priority_description;
    string creation_date;
    string deadline_date;
    cout << "Enter task name: ";
    getline(cin, name);
    cout << "Enter the priority only choose high,medium,low:";
    getline(cin, priority_description);
    if (priority_description == "high")
    {
        priority = 3;
    }
    else if (priority_description == "medium")
    {
        priority = 2;
    }
    else if (priority_description == "low")
    {
        priority = 1;
    }
    else
    {
        cout << "Invalid  priority level " << endl;
        return false;
    }

    cout << "Enter task deadline_date year/mm/dd: ";
    getline(cin, deadline_date);

    new_task.id = generateRandomDigit();
    new_task.creation_date = getCurrentDate();
    new_task.priority = priority;
    new_task.name = name;
    new_task.deadline_date = deadline_date;
    curr_user->tasks.push(new_task);
    updateUsers();
    cout << "tasks updated successfully" << endl;

    return true;
}

bool createSubtask()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return false;
    }

    int task_id;
    cout << "Enter parent task ID: ";
    cin >> task_id;
    cin.ignore();

    string name;
    string priority_description;
    int priority;
    string deadline_date;

    priority_queue<Task> temp;
    bool found = false;

    while (!curr_user->tasks.empty())
    {
        Task curr = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (curr.id == task_id)
        {
            found = true;

            cout << "Enter subtask name: ";
            getline(cin, name);

            cout << "Enter priority (high, medium, low): ";
            getline(cin, priority_description);

            if (priority_description == "high")
                priority = 3;
            else if (priority_description == "medium")
                priority = 2;
            else if (priority_description == "low")
                priority = 1;
            else
            {
                cout << "Invalid priority level" << endl;
                return false;
            }

            cout << "Enter subtask deadline (yyyy/mm/dd): ";
            getline(cin, deadline_date);

            Task subtask;
            subtask.id = generateRandomDigit();
            subtask.name = name;
            subtask.priority = priority;
            subtask.creation_date = getCurrentDate();
            subtask.deadline_date = deadline_date;

            curr.subtasks.push_back(subtask);
        }

        temp.push(curr);
    }

    curr_user->tasks = temp;

    if (!found)
    {
        cout << "Task ID not found" << endl;
        return false;
    }

    updateUsers();
    cout << "Subtask added successfully" << endl;
    return true;
}

bool deleteTask()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return false;
    }

    int task_id;
    cout << "Enter the Task ID to delete: ";
    cin >> task_id;
    cin.ignore();

    priority_queue<Task> temp;
    bool found = false;

    while (!curr_user->tasks.empty())
    {
        Task curr = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (curr.id == task_id)
        {
            found = true;
            // Skip pushing this task → it is deleted
        }
        else
        {
            temp.push(curr);
        }
    }

    curr_user->tasks = temp;

    if (!found)
    {
        cout << "Task ID not found" << endl;
        return false;
    }

    updateUsers();
    cout << "Task deleted successfully" << endl;
    return true;
}

bool deleteSubtask()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return false;
    }

    int task_id, subtask_id;
    cout << "Enter parent Task ID: ";
    cin >> task_id;
    cin.ignore();

    cout << "Enter Subtask ID to delete: ";
    cin >> subtask_id;
    cin.ignore();

    priority_queue<Task> temp;
    bool task_found = false;
    bool subtask_found = false;

    while (!curr_user->tasks.empty())
    {
        Task curr = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (curr.id == task_id)
        {
            task_found = true;

            // Remove subtask using erase-remove_if
            auto &subs = curr.subtasks;
            auto it = remove_if(subs.begin(), subs.end(), [subtask_id](const Task &s) {
                return s.id == subtask_id;
            });

            if (it != subs.end())
            {
                subs.erase(it, subs.end());
                subtask_found = true;
            }
        }

        temp.push(curr);
    }

    curr_user->tasks = temp;

    if (!task_found)
    {
        cout << "Parent Task ID not found" << endl;
        return false;
    }
    if (!subtask_found)
    {
        cout << "Subtask ID not found" << endl;
        return false;
    }

    updateUsers();
    cout << "Subtask deleted successfully" << endl;
    return true;
}

int main()
{
    loadUsers();
    int login_choice;
    cout << "Hey user welcome to AASTU TODO App !!!" << endl;
    cout << "1.Login\n2.Register" << endl;
    cin >> login_choice;
    cin.ignore();
    if (login_choice == 2)
    {
        signup();
    }
    else if (login_choice == 1)
    {
        if (login())
        {
            int choice;
            cout << "1.create task" << endl;
            cout << "2. create Subtask" << endl;
            cin >> choice;
            cin.ignore();

            if (choice == 1)
            {
                createTask();
            }else if (choice == 2){
                createSubtask();
            }
        }
   
    }

    return 0;
}
