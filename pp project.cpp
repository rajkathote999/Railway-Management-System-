#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>

using namespace std;

class Train {
protected:
    string trainNumber;
    string trainName;
    string start;
    string destination;
    float price;
    int seat;
    string time;

public:
    Train() : trainNumber(""), trainName(""), start(""), destination(""), price(0.0), seat(0), time("") {}

    void setTrainDetails(const string& tn, const string& name, const string& st, const string& dest,
                         float p, int s, const string& t) {
        trainNumber = tn;
        trainName = name;
        start = st;
        destination = dest;
        price = p;
        seat = s;
        time = t;
    }

    string getTrainNumber() const { return trainNumber; }
    int getAvailableSeats() const { return seat; }

    void displayTrainInfo() const {
        cout << "Train Number: " << trainNumber 
             << "\nTrain Name: " << trainName 
             << "\nStart: " << start 
             << "\nDestination: " << destination 
             << "\nPrice: $" << price 
             << "\nSeats: " << seat 
             << "\nTime: " << time << endl;
    }

    void bookSeat() {
        if (seat > 0) {
            seat--;
            cout << "Seat booked successfully. Seats left: " << seat << endl;
        } else {
            cout << "No seats available!" << endl;
        }
    }

    void addSeat() { seat++; }

    friend ofstream& operator<<(ofstream& os, const Train& train) {
        os << train.trainNumber << '\n'
           << train.trainName << '\n'
           << train.start << '\n'
           << train.destination << '\n'
           << train.price << '\n'
           << train.seat << '\n'
           << train.time << '\n';
        return os;
    }

    friend ifstream& operator>>(ifstream& is, Train& train) {
        getline(is, train.trainNumber);
        getline(is, train.trainName);
        getline(is, train.start);
        getline(is, train.destination);
        is >> train.price >> train.seat;
        is.ignore();  // Ignore leftover newline
        getline(is, train.time);
        return is;
    }
};

class User {
protected:
    string username;
    string password;

public:
    virtual void displayOptions() = 0; // Pure virtual function
    virtual void executeChoice(int choice, vector<Train>& trains, unordered_map<string, vector<string>>& bookings) = 0; // Execute user choice

    void login() {
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
    }
};

class Admin : public User {
public:
    void displayOptions() override {
        cout << "\nAdmin Options:\n1. Add Train\n2. Delete Train\n3. View All Trains\n";
    }

    void executeChoice(int choice, vector<Train>& trains, unordered_map<string, vector<string>>& bookings) override {
        switch (choice) {
        case 1:
            addTrain(trains);
            break;
        case 2:
            deleteTrain(trains);
            break;
        case 3:
            viewAllTrains(trains);
            break;
        default:
            cout << "Invalid choice." << endl;
        }
    }

    void addTrain(vector<Train>& trains) {
        string tn, name, st, dest, time;
        float price;
        int seat;

        cout << "Enter train number: ";
        cin >> tn;
        cin.ignore();

        cout << "Enter train name: ";
        getline(cin, name);

        cout << "Enter start location: ";
        getline(cin, st);

        cout << "Enter destination: ";
        getline(cin, dest);

        cout << "Enter price: ";
        cin >> price;

        cout << "Enter seats available: ";
        cin >> seat;
        cin.ignore();

        cout << "Enter time: ";
        getline(cin, time);

        Train train;
        train.setTrainDetails(tn, name, st, dest, price, seat, time);
        trains.push_back(train);

        cout << "Train added successfully." << endl;
    }

    void deleteTrain(vector<Train>& trains) {
        string tn;
        cout << "Enter train number to delete: ";
        cin >> tn;

        auto it = remove_if(trains.begin(), trains.end(),
                            [&tn](const Train& train) { return train.getTrainNumber() == tn; });

        if (it != trains.end()) {
            trains.erase(it, trains.end());
            cout << "Train deleted successfully." << endl;
        } else {
            cout << "Train not found." << endl;
        }
    }

    void viewAllTrains(const vector<Train>& trains) const {
        if (trains.empty()) {
            cout << "No trains available." << endl;
            return;
        }

        for (const auto& train : trains) {
            train.displayTrainInfo();
            cout << "----------------------" << endl;
        }
    }
};

class Passenger : public User {
public:
    void displayOptions() override {
        cout << "\nPassenger Options:\n1. Book Ticket\n2. Cancel Ticket\n3. View All Trains\n4. View Booked Tickets\n";
    }

    void executeChoice(int choice, vector<Train>& trains, unordered_map<string, vector<string>>& bookings) override {
        switch (choice) {
        case 1:
            bookTicket(trains, bookings);
            break;
        case 2:
            cancelTicket(trains, bookings);
            break;
        case 3:
            viewAllTrains(trains);
            break;
        case 4:
            viewBookedTickets(bookings);
            break;
        default:
            cout << "Invalid choice." << endl;
        }
    }

    void bookTicket(vector<Train>& trains, unordered_map<string, vector<string>>& bookings) {
        string tn;
        cout << "Enter train number to book: ";
        cin >> tn;

        for (auto& train : trains) {
            if (train.getTrainNumber() == tn) {
                train.bookSeat();
                bookings[username].push_back(tn); // Store booked train number
                return;
            }
        }
        cout << "Train not found." << endl;
    }

    void cancelTicket(vector<Train>& trains, unordered_map<string, vector<string>>& bookings) {
        string tn;
        cout << "Enter train number to cancel: ";
        cin >> tn;

        for (auto& train : trains) {
            if (train.getTrainNumber() == tn) {
                train.addSeat();
                auto& userBookings = bookings[username];
                userBookings.erase(remove(userBookings.begin(), userBookings.end(), tn), userBookings.end()); // Remove train number from bookings
                cout << "Ticket cancelled successfully." << endl;
                return;
            }
        }
        cout << "Train not found." << endl;
    }

    void viewBookedTickets(const unordered_map<string, vector<string>>& bookings) const {
        auto it = bookings.find(username);
        if (it != bookings.end() && !it->second.empty()) {
            cout << "Booked Tickets:\n";
            for (const auto& trainNumber : it->second) {
                cout << trainNumber << endl;
            }
        } else {
            cout << "No booked tickets." << endl;
        }
    }

    void viewAllTrains(const vector<Train>& trains) const {
        if (trains.empty()) {
            cout << "No trains available." << endl;
            return;
        }

        for (const auto& train : trains) {
            train.displayTrainInfo();
            cout << "----------------------" << endl;
        }
    }
};

void saveTrainsToFile(const vector<Train>& trains) {
    ofstream outFile("trains.txt");
    if (!outFile) {
        cerr << "Error opening file for writing!" << endl;
        return;
    }

    for (const auto& train : trains) {
        outFile << train;
    }
    outFile.close();
}

void loadTrainsFromFile(vector<Train>& trains) {
    ifstream inFile("trains.txt");
    if (!inFile) {
        // If file doesn't exist, add predefined trains
        cout << "No train data found. Adding default trains." << endl;

        Train train1, train2, train3, train4, train5, train6;
        train1.setTrainDetails("123", "Shadapti Express", "Pune", "Solapur", 450, 120, "08:00 AM");
        train2.setTrainDetails("234", "Siddheswar Express", "Lonavala", "Hyderabad", 1200, 180, "09:00 AM");
        train3.setTrainDetails("345", "Badlapur Express", "Tirupati", "Indore", 1850, 485, "10:00 AM");
        train4.setTrainDetails("456", "Jabalpur Express", "Pune", "Solapur", 4800, 1490, "11:00 AM");
        train5.setTrainDetails("567", "Chennai Jatpat Express", "Delhi", "Kolkata", 450, 1540, "12:00 PM");
        train6.setTrainDetails("678", "Kanyakumari Entrance Devdardhan", "Jabalpur", "Sri Lanka", 1220, 2344, "01:00 PM");

        trains.push_back(train1);
        trains.push_back(train2);
        trains.push_back(train3);
        trains.push_back(train4);
        trains.push_back(train5);
        trains.push_back(train6);

        return; // Exit the function after adding default trains
    }

    Train train;
    while (inFile >> train) {
        trains.push_back(train);
    }
    inFile.close();

    cout << "Trains loaded from file: " << trains.size() << endl;
}

bool adminLogin() {
    string adminUsername = "admin";
    string adminPassword = "password"; // Hardcoded for simplicity

    string inputUsername, inputPassword;
    cout << "Admin Username: ";
    cin >> inputUsername;
    cout << "Admin Password: ";
    cin >> inputPassword;

    return (inputUsername == adminUsername && inputPassword == adminPassword);
}

int main() {
    vector<Train> trains;
    unordered_map<string, vector<string>> bookings; // Store bookings by username
    loadTrainsFromFile(trains);

    User* user = nullptr; // Pointer to User type
    int choice;

    do {
        cout << "\nSelect User Type:\n1. Admin\n2. Passenger\n3. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
            if (!adminLogin()) {
                cout << "Invalid admin credentials." << endl;
                continue;
            }
            user = new Admin();
        } else if (choice == 2) {
            user = new Passenger();
            user->login(); // Passenger login
        } else if (choice == 3) {
            saveTrainsToFile(trains);
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid choice!" << endl;
            continue;
        }

        int subChoice;
        user->displayOptions();
        cout << "Enter your choice: ";
        cin >> subChoice;
        user->executeChoice(subChoice, trains, bookings);

        delete user; // Free allocated memory for user
    } while (true);

    return 0;
}
