#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <string>
#include <algorithm>

namespace chatapp {
    using String = std::string;
    template <typename T>
    using Vector = std::vector<T>;
    using Map = std::unordered_map<String, String>;

    // Utility to generate a timestamp
    String getTimestamp() {
        std::time_t now = std::time(0);
        char dt[20];
        std::strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return String(dt);
    }

    // Message class to hold individual messages
    class Message {
    public:
        String sender;
        String content;
        String timestamp;

        Message(const String &sender, const String &content)
                : sender(sender), content(content), timestamp(getTimestamp()) {}
    };

    // Person class to represent individual users
    class Person {
    private:
        String username;
        String password;
        Vector<String> friends;
        Vector<Message> inbox;

    public:
        Person(const String &user, const String &pass)
                : username(user), password(pass) {}

        String getUsername() const { return username; }

        bool checkPassword(const String &pass) const {
            return password == pass; // Password check simplified
        }

        void sendMessage(Person &recipient, const String &content) {
           if(this->isFriend(recipient.username)){
             recipient.inbox.push_back(Message(username, content));
            std::cout << "Message sent to " << recipient.getUsername() << "\n";
           }
           else{
            std::cout   << recipient.getUsername() << " not your freind\n";

           }
        }

        void addFriend(const String &friendName) {
            if (!isFriend(friendName)) {
                friends.push_back(friendName);
                std::cout << friendName << " added as a friend.\n";
            } else {
                std::cout << friendName << " is already your friend.\n";
            }
        }

        bool isFriend(const String &friendName) const {
            return std::find(friends.begin(), friends.end(), friendName) != friends.end();
        }

        void viewInbox() {
            if (inbox.empty()) {
                std::cout << "Inbox is empty.\n";
            } else {
                for (const auto &msg : inbox) {
                    std::cout << "Message from " << msg.sender << ": " << msg.content << " [" << msg.timestamp << "]\n";
                }
                inbox.clear();
            }
        }

        void viewFriends() const {
            std::cout << "Friends: ";
            if (friends.empty()) {
                std::cout << "None\n";
            } else {
                for (const auto &friendName : friends) {
                    std::cout << friendName << " ";
                }
                std::cout << "\n";
            }
        }
    };

    // Global user map for managing users
    Map userPasswords;
    Vector<Person> users;

    void createAccount(const String &username, const String &password) {
        if (userPasswords.find(username) == userPasswords.end()) {
            users.push_back(Person(username, password));
            userPasswords[username] = password; // Store plain password for now
            std::cout << "Account created for " << username << "\n";
        } else {
            std::cout << "Username " << username << " already exists.\n";
        }
    }

    Person* login(const String &username, const String &password) {
        auto it = userPasswords.find(username);
        if (it != userPasswords.end() && it->second == password) {
            for (auto &user : users) {
                if (user.getUsername() == username) {
                    std::cout << username << " logged in successfully!\n";
                    return &user;
                }
            }
        }
        std::cout << "Invalid username or password.\n";
        return nullptr;
    }

    void saveUsersToFile() {
        std::ofstream file("users.txt");
        if (!file.is_open()) return;

        file << users.size() << "\n";
        for (const auto &user : users) {
            file << user.getUsername() << "\n" << user.checkPassword("dummy") << "\n";
        }
    }

    void loadUsersFromFile() {
        std::ifstream file("users.txt");
        if (!file.is_open()) return;

        size_t userCount;
        file >> userCount;
        file.ignore(); // Ignore newline character after userCount

        for (size_t i = 0; i < userCount; ++i) {
            String username, password;
            std::getline(file, username);
            std::getline(file, password);

            users.push_back(Person(username, password));
            userPasswords[username] = password;
        }
    }
}
int main() {
    chatapp::loadUsersFromFile(); // Load existing users from file

    while (true) {  // Outer loop to allow logging in from multiple accounts
        std::cout << "Available usernames:\n";
        for (const auto &user : chatapp::users) {
            std::cout << "- " << user.getUsername() << "\n";
        }

        chatapp::String selectedUsername;
        std::cout << "Select your username: ";
        std::cin >> selectedUsername;
        chatapp::String password;
        std::cout << "Enter your password: ";
        std::cin >> password;

        chatapp::Person *loggedInUser = chatapp::login(selectedUsername, password);
        if (!loggedInUser) continue; // If login fails, start again

        while (true) {  // Inner loop for logged-in user actions
            std::cout << "\n1. View Friends\n";
            std::cout << "2. Send Friend Request\n";
            std::cout << "3. Send Message\n";
            std::cout << "4. View Inbox\n";
            std::cout << "5. Logout\n";
            std::cout << "6. Exit\n";
            std::cout << "Select an option: ";
            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1:
                    loggedInUser->viewFriends();
                    break;
                case 2: {
                    chatapp::String friendUsername;
                    std::cout << "Enter username to add as friend: ";
                    std::cin >> friendUsername;
                    if (chatapp::userPasswords.find(friendUsername) != chatapp::userPasswords.end()) {
                        loggedInUser->addFriend(friendUsername);
                    } else {
                        std::cout << "User does not exist.\n";
                    }
                    break;
                }
                case 3: {
                    chatapp::String recipientUsername;
                    std::cout << "Enter recipient username: ";
                    std::cin >> recipientUsername;
                    if (chatapp::userPasswords.find(recipientUsername) != chatapp::userPasswords.end()) {
                        chatapp::String messageContent;
                        std::cout << "Enter your message: ";
                        std::cin.ignore();
                        std::getline(std::cin, messageContent);
                        loggedInUser->sendMessage(*chatapp::login(recipientUsername, chatapp::userPasswords[recipientUsername]), messageContent);
                    } else {
                        std::cout << "User does not exist.\n";
                    }
                    break;
                }
                case 4:
                    loggedInUser->viewInbox();
                    break;
                case 5:  // Logout action
                    std::cout << "Logging out...\n";
                    goto outer_loop;  // Exit inner loop to outer loop
                case 6:  // Exit the program
                    chatapp::saveUsersToFile();
                    std::cout << "Exiting...\n";
                    return 0;
                default:
                    std::cout << "Invalid option. Please try again.\n";
            }
        }
        outer_loop:
        continue;  // Label for goto to resume outer loop
    }
    return 0;
}
