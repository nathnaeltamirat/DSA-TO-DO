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
    bool isComplete = false;
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
bool updateTaskById();
bool updateSubtaskById();
void displayTasksByFilter();
bool deleteTask();
bool exportStatusToJson();
void updateUsers();
bool deleteSubtask();
bool createSubtask();
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
            task.isComplete = t.value("isComplete", false);
            task.creation_date = t.value("creation_date", "");
            task.deadline_date = t.value("deadline_date", "");
            for (auto &st : t["subtasks"])
            {
                Task subtask;
                subtask.id = st["id"];
                subtask.name = st["name"];
                subtask.isComplete = st.value("isComplete", false);
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
bool taskDone()
{
    if (!curr_user)
        return false;
    int taskId;
    cout << "Enter task id: ";
    cin >> taskId;

    priority_queue<Task> temp;
    bool found = false;

    while (!curr_user->tasks.empty())
    {
        Task t = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (t.id == taskId)
        {
            t.isComplete = true;
            found = true;
        }

        temp.push(t);
    }

    curr_user->tasks = temp;
    if (found)
    {
        updateUsers();
        cout << "task completed" << endl;
    }else{
        cout << "error happen" << endl;
    }

    return found;
}

bool taskNotDone()
{
    if (!curr_user)
        return false;
    int taskId;
    cout << "Enter task id: ";
    cin >> taskId;

    priority_queue<Task> temp;
    bool found = false;

    while (!curr_user->tasks.empty())
    {
        Task t = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (t.id == taskId)
        {
            t.isComplete = false;
            found = true;
        }

        temp.push(t);
    }

    curr_user->tasks = temp;
    if (found)
    {
        updateUsers();
        cout << "task completed" << endl;
    }else{
        cout << "error" << endl;
    }
    return found;
}

bool subtaskDone()
{
    int taskId;
    int subtaskId;
    cout << "Enter task id: ";
    cin >> taskId;
    cin.ignore();
    cout << "Enter subtask id: ";
    cin >> subtaskId;
    cin.ignore();
    if (!curr_user)
        return false;

    priority_queue<Task> temp;
    bool task_found = false, subtask_found = false;

    while (!curr_user->tasks.empty())
    {
        Task t = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (t.id == taskId)
        {
            task_found = true;
            for (auto &s : t.subtasks)
            {
                if (s.id == subtaskId)
                {
                    s.isComplete = true;
                    subtask_found = true;
                    break;
                }
            }
        }

        temp.push(t);
    }

    curr_user->tasks = temp;
    if (task_found && subtask_found)
    {
        updateUsers();
        cout << "subtask completed" << endl;
    }
    else
    {
        cout << "error doing completion" << endl;
    }
    return subtask_found;
}

bool subtaskNotDone()
{
    int taskId;
    int subtaskId;
    cout << "Enter task id: ";
    cin >> taskId;
    cin.ignore();
    cout << "Enter subtask id: ";
    cin >> subtaskId;
    cin.ignore();
    if (!curr_user)
        return false;

    priority_queue<Task> temp;
    bool task_found = false, subtask_found = false;

    while (!curr_user->tasks.empty())
    {
        Task t = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (t.id == taskId)
        {
            task_found = true;
            for (auto &s : t.subtasks)
            {
                if (s.id == subtaskId)
                {
                    s.isComplete = false;
                    subtask_found = true;
                    break;
                }
            }
        }

        temp.push(t);
    }

    curr_user->tasks = temp;
    if (task_found && subtask_found)
    {
        updateUsers();
        cout << "subtask completion removed successfully" << endl;
    }
    else
    {
        cout << "error doing completion" << endl;
    }
    return subtask_found;
}

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
            j["isComplete"] = curr.isComplete;
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
                sub["isComplete"] = sb.isComplete;
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

void displayTasksByFilter()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return;
    }

    vector<Task> list;
    priority_queue<Task> temp = curr_user->tasks;
    while (!temp.empty())
    {
        list.push_back(temp.top());
        temp.pop();
    }

    cout << "Choose filter:\n1. Recently created\n2. Farest created\n3. Nearest deadline\n4. Farest deadline" << endl;
    int fchoice;
    cin >> fchoice;
    cin.ignore();

    auto normalizeDate = [](const string &d) -> string
    {
        if (d.empty())
            return string();
        string s = d;
        for (auto &c : s)
            if (c == '/')
                c = '-';
        string digits;
        for (char c : s)
            if (isdigit((unsigned char)c))
                digits.push_back(c);
        // Expect YYYYMMDD
        if (digits.size() >= 8)
            return digits.substr(0, 8);
        // fallback: pad to something comparable
        while (digits.size() < 8)
            digits.push_back('0');
        return digits;
    };

    if (fchoice == 1)
    {
        sort(list.begin(), list.end(), [&](const Task &a, const Task &b)
             { return normalizeDate(a.creation_date) > normalizeDate(b.creation_date); });
    }
    else if (fchoice == 2)
    {
        sort(list.begin(), list.end(), [&](const Task &a, const Task &b)
             { return normalizeDate(a.creation_date) < normalizeDate(b.creation_date); });
    }
    else if (fchoice == 3)
    {
        sort(list.begin(), list.end(), [&](const Task &a, const Task &b)
             {
                 string da = normalizeDate(a.deadline_date);
                 string db = normalizeDate(b.deadline_date);
                 if (da.empty() && db.empty())
                     return false;
                 if (da.empty())
                     return false; // put empty deadlines at end
                 if (db.empty())
                     return true;
                 return da < db; });
    }
    else if (fchoice == 4)
    {
        sort(list.begin(), list.end(), [&](const Task &a, const Task &b)
             {
                 string da = normalizeDate(a.deadline_date);
                 string db = normalizeDate(b.deadline_date);
                 if (da.empty() && db.empty())
                     return false;
                 if (da.empty())
                     return false; // place tasks with no deadline at end
                 if (db.empty())
                     return true;
                 return da > db; });
    }
    else
    {
        cout << "Invalid choice" << endl;
        return;
    }

    if (list.empty())
    {
        cout << "No tasks found" << endl;
        return;
    }

    for (auto &t : list)
    {
        string isCompleted = t.isComplete ? "Completed" : "Not completed";
        string p = (t.priority == 3) ? "high" : (t.priority == 2) ? "medium"
                                                                  : "low";
        cout << "Task ID: " << t.id << " | Name: " << t.name << " | status: " << isCompleted << " | Priority: " << p
             << " | Created: " << t.creation_date << " | Deadline: " << t.deadline_date << endl;
        if (!t.subtasks.empty())
        {
            cout << "  Subtasks:" << endl;
            for (auto &st : t.subtasks)
            {
                string sisCompleted = st.isComplete ? "Completed" : "Not completed";
                string sp = (st.priority == 3) ? "high" : (st.priority == 2) ? "medium"
                                                                             : "low";
                cout << "    Subtask ID: " << st.id << " | Name: " << st.name << " | status: " << sisCompleted << " | Priority: " << sp
                     << " | Created: " << st.creation_date << " | Deadline: " << st.deadline_date << endl;
            }
        }
    }
    int choice;
    cout << "1.Edit task\n"
            "2.Edit subtask\n"
            "3.Delete task\n"
            "4.Delete subtask\n"
            "5.Mark complete task\n"
            "6.Mark complete subtask\n"
            "7.Mark uncomplete task\n"
            "8.Mark uncomplete subtask\n"
            "9.Add subtask\n"
            "10.Any number to continue\n";
    cin >> choice;
    cin.clear();
    if (choice == 1)
    {
        updateTaskById();
    }
    else if (choice == 2)
    {
        updateSubtaskById();
    }
    else if (choice == 3)
    {
        deleteTask();
    }
    else if (choice == 4)
    {
        deleteSubtask();
    }
    else if (choice == 5)
    {
        taskDone();
    }
    else if (choice == 6)
    {
        subtaskDone();
    }
    else if (choice == 7)
    {
        taskNotDone();
    }
    else if (choice == 8)
    {
        subtaskNotDone();
    }
    else if (choice == 9)
    {
        createSubtask();
    }
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
    if (full_name.empty())
    {
        cout << "Full name cannot be empty.\n";
        return false;
    }
    cout << "Enter your email: ";
    getline(cin, email);
    if (email.empty())
    {
        cout << "Email cannot be empty.\n";
        return false;
    }
    if (email.find('@') == string::npos || email.find('.') == string::npos)
    {
        cout << "Invalid email format.\n";
        return false;
    }
    cout << "Enter your password: ";
    getline(cin, password);
    if (password.empty())
    {
        cout << "Password cannot be empty.\n";
        return false;
    }
    if (password.length() < 6)
    {
        cout << "Password must be at least 6 characters long.\n";
        return false;
    }

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
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    return to_string(year) + "-" +
           (month < 10 ? "0" : "") + to_string(month) + "-" +
           (day < 10 ? "0" : "") + to_string(day);
}

bool login()
{
    string email;
    string password;

    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your password: ";
    getline(cin, password);
    if (email.empty() || password.empty())
    {
        cout << "Email and password cannot be empty.\n";
        return false;
    }

    if (email.find('@') == string::npos || email.find('.') == string::npos)
    {
        cout << "Invalid email format.\n";
        return false;
    }

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
            cout << "Logged in successfull" << endl;
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
    if (name.empty())
    {
        cout << "Task name cannot be empty.\n";
        return false;
    }

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

    cout << "Enter task deadline_date year-mm-dd: ";
    getline(cin, deadline_date);
    // Empty check
    if (deadline_date.empty())
    {
        cout << "Deadline cannot be empty.\n";
        return false;
    }

    // Format check: yyyy/mm/dd
    if (deadline_date.length() != 10 ||
        deadline_date[4] != '-' ||
        deadline_date[7] != '-')
    {
        cout << "Invalid date format. Use yyyy-mm-dd\n";
        return false;
    }

    // Digit check
    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!isdigit(deadline_date[i]))
        {
            cout << "Date must contain only digits (yyyy/mm/dd)\n";
            return false;
        }
    }

    // Logical range check
    int year = stoi(deadline_date.substr(0, 4));
    int month = stoi(deadline_date.substr(5, 2));
    int day = stoi(deadline_date.substr(8, 2));

    if (month < 1 || month > 12)
    {
        cout << "Invalid month value.\n";
        return false;
    }

    if (day < 1 || day > 31)
    {
        cout << "Invalid day value.\n";
        return false;
    }

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
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task ID.\n";
        return false;
    }
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
            if (name.empty())
            {
                cout << "Subtask name cannot be empty.\n";
                return false;
            }

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

            cout << "Enter subtask deadline (yyyy-mm-dd): ";
            getline(cin, deadline_date);
            // Empty check
            if (deadline_date.empty())
            {
                cout << "Deadline cannot be empty.\n";
                return false;
            }

            // Format check: yyyy/mm/dd
            if (deadline_date.length() != 10 ||
                deadline_date[4] != '-' ||
                deadline_date[7] != '-')
            {
                cout << "Invalid date format. Use yyyy/mm/dd\n";
                return false;
            }

            // Digit check
            for (int i = 0; i < 10; i++)
            {
                if (i == 4 || i == 7)
                    continue;
                if (!isdigit(deadline_date[i]))
                {
                    cout << "Date must contain only digits (yyyy/mm/dd)\n";
                    return false;
                }
            }

            // Logical range check
            int year = stoi(deadline_date.substr(0, 4));
            int month = stoi(deadline_date.substr(5, 2));
            int day = stoi(deadline_date.substr(8, 2));

            if (month < 1 || month > 12)
            {
                cout << "Invalid month value.\n";
                return false;
            }

            if (day < 1 || day > 31)
            {
                cout << "Invalid day value.\n";
                return false;
            }

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
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task ID.\n";
        return false;
    }
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
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task ID.\n";
        return false;
    }
    cin.ignore();

    cout << "Enter Subtask ID to delete: ";
    cin >> subtask_id;
    
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid subtask ID.\n";
        return false;
    }
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
            auto it = remove_if(subs.begin(), subs.end(), [subtask_id](const Task &s)
                                { return s.id == subtask_id; });

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
bool updateProfile()
{
    // Menu
    int option;
    cout << "Choose what to update:\n";
    cout << "1. Full Name\n2. Email\n3. Password\n4. All\n";
    cin >> option;
    // validate Menu
    if (option < 1 || option > 4)
    {
        cout << "Invalid option!\n";
        return false;
    }

    cin.ignore();

    switch (option)
    {

    case 1:
    {
        string updated_name;
        cout << "Enter the new full name:\n";
        getline(cin, updated_name);

        if (updated_name.empty())
        {
            cout << "Name cannot be empty!\n";
            return false;
        }

        curr_user->full_name = updated_name;
        cout << "Full Name updated successfully!\n";
        break;
    }

    case 2:
    {
        string updated_email;
        cout << "Enter the new Email:\n";
        getline(cin, updated_email);

        if (updated_email.find('@') == string::npos ||
            updated_email.find('.') == string::npos)
        {
            cout << "Invalid email format!\n";
            return false;
        }

        curr_user->email = updated_email;
        cout << "Email updated successfully!\n";
        break;
    }

    case 3:
    {
        string updated_password, confirmed_password;

        cout << "Enter the new password:\n";
        getline(cin, updated_password);

        if (updated_password.length() < 6)
        {
            cout << "Password must be at least 6 characters!\n";
            return false;
        }

        cout << "Confirm the new password:\n";
        getline(cin, confirmed_password);

        if (updated_password != confirmed_password)
        {
            cout << "Passwords do not match!\n";
            return false;
        }

        curr_user->password = updated_password;
        cout << "Password updated successfully!\n";
        break;
    }

    case 4:
    {
        string upd_name, upd_email, upd_password, confirm_password;

        cout << "Enter the new full name:\n";
        getline(cin, upd_name);
        if (upd_name.empty())
        {
            cout << "Name cannot be empty!\n";
            return false;
        }

        cout << "Enter the new Email:\n";
        getline(cin, upd_email);
        if (upd_email.find('@') == string::npos ||
            upd_email.find('.') == string::npos)
        {
            cout << "Invalid email format!\n";
            return false;
        }

        cout << "Enter the new Password:\n";
        getline(cin, upd_password);
        if (upd_password.length() < 6)
        {
            cout << "Password must be at least 6 characters!\n";
            return false;
        }

        cout << "Confirm the new password:\n";
        getline(cin, confirm_password);
        if (upd_password != confirm_password)
        {
            cout << "Passwords do not match!\n";
            return false;
        }

        curr_user->full_name = upd_name;
        curr_user->email = upd_email;
        curr_user->password = upd_password;

        cout << "Profile updated successfully!\n";
        break;
    }
    }
    updateUsers();
    return true;
}

void displayTasks()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return;
    }

    priority_queue<Task> temp = curr_user->tasks;
    if (temp.empty())
    {
        cout << "No tasks found" << endl;
        return;
    }

    while (!temp.empty())
    {
        Task t = temp.top();
        temp.pop();
        string isCompleted;
        if (t.isComplete == true)
        {
            isCompleted = "Completed";
        }
        else
        {
            isCompleted = "Not completed";
        }
        string p = (t.priority == 3) ? "high" : (t.priority == 2) ? "medium"
                                                                  : "low";
        cout << "Task ID: " << t.id << " | Name: " << t.name << " | status: " << isCompleted << "  | Priority: " << p
             << " | Created: " << t.creation_date << " | Deadline: " << t.deadline_date << endl;

        if (!t.subtasks.empty())
        {
            cout << "  Subtasks:" << endl;
            for (auto &st : t.subtasks)
            {
                string isCompleted;
                if (st.isComplete == true)
                {
                    isCompleted = "Completed";
                }
                else
                {
                    isCompleted = "Not completed";
                }
                string sp = (st.priority == 3) ? "high" : (st.priority == 2) ? "medium"
                                                                             : "low";
                cout << "    Subtask ID: " << st.id << " | Name: " << st.name << " | status: " << isCompleted << " | Priority: " << sp
                     << " | Created: " << st.creation_date << " | Deadline: " << st.deadline_date << endl;
            }
        }
    }
    int choice;
    cout << "1.Edit task\n"
            "2.Edit subtask\n"
            "3.Delete task\n"
            "4.Delete subtask\n"
            "5.Mark complete task\n"
            "6.Mark complete subtask\n"
            "7.Mark uncomplete task\n"
            "8.Mark uncomplete subtask\n"
            "9.Add subtask\n"
            "10.Any number to continue\n";
    cin >> choice;
    cin.clear();
    if (choice == 1)
    {
        updateTaskById();
    }
    else if (choice == 2)
    {
        updateSubtaskById();
    }
    else if (choice == 3)
    {
        deleteTask();
    }
    else if (choice == 4)
    {
        deleteSubtask();
    }
    else if (choice == 5)
    {
        taskDone();
    }
    else if (choice == 6)
    {
        subtaskDone();
    }
    else if (choice == 7)
    {
        taskNotDone();
    }
    else if (choice == 8)
    {
        subtaskNotDone();
    }
    else if (choice == 9)
    {
        createSubtask();
    }
}

string toLower(const string &s)
{
    string r = s;
    for (auto &c : r)
        c = tolower((unsigned char)c);
    return r;
}
bool updateTaskById()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return false;
    }

    int task_id;
    cout << "Enter Task ID to update: ";
    cin >> task_id;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task ID.\n";
        return false;
    }
    cin.ignore();

    string new_name;
    string priority_description;
    int new_priority;
    string new_deadline;

    priority_queue<Task> temp;
    bool found = false;

    while (!curr_user->tasks.empty())
    {
        Task curr = curr_user->tasks.top();
        curr_user->tasks.pop();

        if (curr.id == task_id)
        {
            found = true;
            cout << "Enter new name (leave empty to keep current): ";
            getline(cin, new_name);
            if (!new_name.empty())
                curr.name = new_name;

            cout << "Enter new priority (high, medium, low) or leave empty: ";
            getline(cin, priority_description);
            if (!priority_description.empty())
            {
                if (priority_description == "high")
                    new_priority = 3;
                else if (priority_description == "medium")
                    new_priority = 2;
                else if (priority_description == "low")
                    new_priority = 1;
                else
                {
                    cout << "Invalid priority, keeping existing." << endl;
                    new_priority = curr.priority;
                }
                curr.priority = new_priority;
            }

            cout << "Enter new deadline (yyyy-mm-dd) or leave empty: ";
            getline(cin, new_deadline);

                // Format check yyyy/mm/dd
                if (new_deadline.length() != 10 ||
                    new_deadline[4] != '-' ||
                    new_deadline[7] != '-')
                {
                    cout << "Invalid date format. Use yyyy/mm/dd\n";
                    return false;
                }

                // Digit check
                for (int i = 0; i < 10; i++)
                {
                    if (i == 4 || i == 7)
                        continue;
                    if (!isdigit(new_deadline[i]))
                    {
                        cout << "Date must contain only digits.\n";
                        return false;
                    }
                }

                int year = stoi(new_deadline.substr(0, 4));
                int month = stoi(new_deadline.substr(5, 2));
                int day = stoi(new_deadline.substr(8, 2));

                if (month < 1 || month > 12)
                {
                    cout << "Invalid month value.\n";
                    return false;
                }

                if (day < 1 || day > 31)
                {
                    cout << "Invalid day value.\n";
                    return false;
                }

              

            curr.deadline_date = new_deadline;
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
    cout << "Task updated successfully" << endl;
    return true;
}

bool updateSubtaskById()
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
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID input.\n";
        return false;
    }

    cout << "Enter Subtask ID to update: ";
    cin >> subtask_id;
    cin.ignore();
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID input.\n";
        return false;
    }

    string new_name;
    string priority_description;
    int new_priority;
    string new_deadline;

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
            for (auto &st : curr.subtasks)
            {
                if (st.id == subtask_id)
                {
                    subtask_found = true;
                    cout << "Enter new name for subtask (leave empty to keep current): ";
                    getline(cin, new_name);
                    if (!new_name.empty())
                        st.name = new_name;

                    cout << "Enter new priority (high, medium, low) or leave empty: ";
                    getline(cin, priority_description);
                    if (!priority_description.empty())
                    {
                        if (priority_description == "high")
                            new_priority = 3;
                        else if (priority_description == "medium")
                            new_priority = 2;
                        else if (priority_description == "low")
                            new_priority = 1;
                        else
                        {
                            cout << "Invalid priority, keeping existing." << endl;
                            new_priority = st.priority;
                        }
                        st.priority = new_priority;
                    }

                    cout << "Enter new deadline (yyyy-mm-dd) or leave empty: ";
                    getline(cin, new_deadline);
                
                        // Format check yyyy/mm/dd
                        if (new_deadline.length() != 10 ||
                            new_deadline[4] != '-' ||
                            new_deadline[7] != '-')
                        {
                            cout << "Invalid date format. Use yyyy/mm/dd\n";
                            return false;
                        }

                        // Digit check
                        for (int i = 0; i < 10; i++)
                        {
                            if (i == 4 || i == 7)
                                continue;
                            if (!isdigit(new_deadline[i]))
                            {
                                cout << "Date must contain only digits.\n";
                                return false;
                            }
                        }

                        int year = stoi(new_deadline.substr(0, 4));
                        int month = stoi(new_deadline.substr(5, 2));
                        int day = stoi(new_deadline.substr(8, 2));

                        if (month < 1 || month > 12)
                        {
                            cout << "Invalid month value.\n";
                            return false;
                        }

                        if (day < 1 || day > 31)
                        {
                            cout << "Invalid day value.\n";
                            return false;
                        }

    
                    st.deadline_date = new_deadline;
                    break;
                }
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
    cout << "Subtask updated successfully" << endl;
    return true;
}

bool searchTaskByName()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in" << endl;
        return false;
    }

    string query;
    cout << "Enter task name to search: ";
    getline(cin, query);
    if (query.empty())
    {
        cout << "Empty query" << endl;
        return false;
    }

    string ql = toLower(query);
    priority_queue<Task> temp = curr_user->tasks;
    bool found = false;

    while (!temp.empty())
    {
        Task t = temp.top();
        temp.pop();

        string tl = toLower(t.name);
        if (tl.find(ql) != string::npos)
        {
            found = true;
            string p = (t.priority == 3) ? "high" : (t.priority == 2) ? "medium"
                                                                      : "low";
            string isCompleted;
            if (t.isComplete == true)
            {
                isCompleted = "Completed";
            }
            else
            {
                isCompleted = "Not completed";
            }
            cout << "Task ID: " << t.id << " | Name: " << t.name << " | status: " << isCompleted << " | Priority: " << p
                 << " | Created: " << t.creation_date << " | Deadline: " << t.deadline_date << endl;

            if (!t.subtasks.empty())
            {
                cout << "  Subtasks:" << endl;
                for (auto &st : t.subtasks)
                {
                    string sp = (st.priority == 3) ? "high" : (st.priority == 2) ? "medium"
                                                                                 : "low";
                    string sisCompleted;
                    if (st.isComplete == true)
                    {
                        sisCompleted = "Completed";
                    }
                    else
                    {
                        sisCompleted = "Not completed";
                    }
                    cout << "    Subtask ID: " << st.id << " | Name: " << st.name << " | status: " << sisCompleted << " | Priority: " << sp
                         << " | Created: " << st.creation_date << " | Deadline: " << st.deadline_date << endl;
                }
            }
        }
    }
    if (found)
    {
        int choice;
        cout << "1.Edit task\n"
                "2.Edit subtask\n"
                "3.Delete task\n"
                "4.Delete subtask\n"
                "5.Mark complete task\n"
                "6.Mark complete subtask\n"
                "7.Mark uncomplete task\n"
                "8.Mark uncomplete subtask\n"
                "9.Add subtask\n"
                "10.Any number to continue\n";
        cin >> choice;
        cin.clear();
        if (choice == 1)
        {
            updateTaskById();
        }
        else if (choice == 2)
        {
            updateSubtaskById();
        }
        else if (choice == 3)
        {
            deleteTask();
        }
        else if (choice == 4)
        {
            deleteSubtask();
        }
        else if (choice == 5)
        {
            taskDone();
        }
        else if (choice == 6)
        {
            subtaskDone();
        }
        else if (choice == 7)
        {
            taskNotDone();
        }
        else if (choice == 8)
        {
            subtaskNotDone();
        }
        else if (choice == 9)
        {
            createSubtask();
        }
        else
        {
            return false;
        }
    }

    if (!found)
        cout << "No matching tasks found" << endl;

    return found;
}

bool exportStatusToJson()
{
    if (curr_user == nullptr)
    {
        cout << "No user logged in\n";
        return false;
    }

    int total_tasks = 0;
    int completed_tasks = 0;
    int uncompleted_tasks = 0;

    int total_subtasks = 0;
    int completed_subtasks = 0;
    int uncompleted_subtasks = 0;

    priority_queue<Task> temp = curr_user->tasks;

    while (!temp.empty())
    {
        Task t = temp.top();
        temp.pop();

        total_tasks++;

        if (t.isComplete)
            completed_tasks++;
        else
            uncompleted_tasks++;

        for (auto &st : t.subtasks)
        {
            total_subtasks++;

            if (st.isComplete)
                completed_subtasks++;
            else
                uncompleted_subtasks++;
        }
    }

    // Create JSON object
    json output;
    output["user_email"] = curr_user->email;
    output["total_tasks"] = total_tasks;
    output["completed_tasks"] = completed_tasks;
    output["uncompleted_tasks"] = uncompleted_tasks;
    output["total_subtasks"] = total_subtasks;
    output["completed_subtasks"] = completed_subtasks;
    output["uncompleted_subtasks"] = uncompleted_subtasks;

    // Write to output.json
    ofstream out("./db/output.json");
    if (!out.is_open())
    {
        cout << "Failed to open output.json\n";
        return false;
    }

    out << output.dump(4);
    out.close();

    cout << "Status exported successfully to output.json\n";
    return true;
}

bool authenticateAdmin()
{
    string email, password;

    cout << "\n--- Admin Authentication --\n";
    cout << "Admin Email: ";
    getline(cin, email);
    cout << "Admin Password: ";
    getline(cin, password);

    for (auto &u : users)
    {
        if (u.email == email && u.password == password)
        {
            if (u.status == "admin")
            {
                curr_user = &u;
                return true;
            }
            else
            {
                cout << "Access denied: Not an admin.\n";
                return false;
            }
        }
    }

    cout << "Invalid admin credentials.\n";
    return false;
}

bool addUserAdmin()
{
    string full_name, email, password;

    cout << "Enter new user's full name: ";
    getline(cin, full_name);
    cout << "Enter new user's email: ";
    getline(cin, email);
    if (email.find('@') == string::npos || email.find('.') == string::npos)
    {
        cout << "Invalid email format.\n";
        return false;
    }
    if (emailExist(email))
    {
        cout << "User already exists.\n";
        return false;
    }
    cout << "Enter new user's password: ";
    getline(cin, password);
    User newUser;
    newUser.full_name = full_name;
    newUser.email = email;
    newUser.password = password;
    newUser.status = "user";
    users.push_back(newUser);
    updateUsers();

    cout << "User added successfully \n";
    return true;
}
bool deleteUserAdmin()
{
    if (curr_user == nullptr || curr_user->status != "admin")
    {
        cout << "Access denied.\n";
        return false;
    }
    string email;
    cout << "Enter user email to delete: ";
    getline(cin, email);
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        if (it->email == email)
        {
            if (it->status == "admin")
            {
                cout << "Cannot delete an admin user.\n";
                return false;
            }
            users.erase(it);
            updateUsers();
            cout << "User deleted successfully.\n";
            return true;
        }
    }

    cout << "User not found.\n";
    return false;
}

void viewAllUsers()
{
    cout << "\n-- All Users ---\n";
    bool found = false;
    for (const auto &u : users)
    {
        if (u.status == "user")
        {
            cout << "Name  : " << u.full_name << endl;
            cout << "Email : " << u.email << endl;
            found = true;
        }
    }
}
void adminDashboard()
{
    int choice;
    cout << "\n===== ADMIN DASHBOARD =====\n";
    cout << "1. Add User\n";
    cout << "2. Delete User\n";
    cout << "3.View All Users\n";
    cout << "4. Exit admin panel other number\n";
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore();

    while (true)
    {
        if (choice == 1)
        {
            addUserAdmin();
        }
        else if (choice == 2)
        {
            deleteUserAdmin();
        }
        else if (choice == 3)
        {
            viewAllUsers();
        }
        else
        {
            break;
        }
    }
}

int main()
{
    loadUsers();
    int login_choice;
    while (true)
    {
        cout << "Hey user welcome to AASTU TODO App !!!" << endl;
        cout << "1.Login\n2.Register\n3.AdminLogin\nq.Quit(any number only)" << endl;
        bool success = false;
        cin >> login_choice;
        cin.ignore();
        if (login_choice == 1)
        {
            if (login())
            {
                success = true;
            }
        }
        else if (login_choice == 2)
        {
            if (signup())
            {
                success = true;
            }
        }
        else if (login_choice == 3)
        {
            if (authenticateAdmin())
            {
                adminDashboard();
            }
        }
        else
        {
            break;
        }
        if (success)
        {
            while (true)
            {
                int choice;
                cout << "1.create task" << endl;
                cout << "2. show tasks" << endl;
                cout << "3. search task by name" << endl;
                cout << "4. show tasks by filter" << endl;
                cout << "5. export task" << endl;
                cout << "q. exit/logout (key number)" << endl;
                cin >> choice;
                cin.ignore();
                if (choice == 1)
                {
                    createTask();
                }
                else if (choice == 2)
                {
                    displayTasks();
                }
                else if (choice == 3)
                {
                    searchTaskByName();
                }
                else if (choice == 4)
                {
                    displayTasksByFilter();
                }
                else if (choice == 5)
                {
                    exportStatusToJson();
                }
                else
                {
                    break;
                }
            }
        }
    }

    return 0;
}
