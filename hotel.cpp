#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib> 
#include <cstdio>  

using namespace std;

class Room {
protected:
    int roomNo;
    float baseRent;
    bool isBooked;

public:

    Room(int rno, float rent) : roomNo(rno), baseRent(rent), isBooked(false) {}

    virtual ~Room() {}

    float getRent() const { return baseRent; }
    float getRent(int days) const { return baseRent * days; }

    int getRoomNo() const { return roomNo; }
    bool getStatus() const { return isBooked; }
    void book() { isBooked = true; }
    void unbook() { isBooked = false; }

    virtual void showType() const {
        cout << " Room Type: Basic";
    }

    friend ostream& operator<<(ostream& os, const Room& r);
};

ostream& operator<<(ostream& os, const Room& r) {
    os << "\n Room No: " << r.roomNo;
    os << "\n Rent/Day: " << r.baseRent;
    os << "\n Status: " << (r.isBooked ? "Booked" : "Available");
    r.showType(); 
    os << "\n----------------------------";
    return os;
}

class DeluxeRoom : public Room {
private:
    float amenityCharge;

public:
    
    DeluxeRoom(int rno) : Room(rno, 3500.0), amenityCharge(500.0) {}

    void showType() const override {
        cout << " Room Type: Deluxe (Extra Amenities)";
    }

    float getRent() const {
        return baseRent + amenityCharge;
    }

    float getRent(int days) const {
        return (baseRent + amenityCharge) * days;
    }
};

class Customer {
private:
    int roomNumber;
    char name[30];
    char phone[15];
    int days;
    float advancePayment;

public:
    
    Customer() : roomNumber(0), days(0), advancePayment(0.0) {
        strcpy(name, "");
        strcpy(phone, "");
    }

    Customer(const Customer& other) : 
        roomNumber(other.roomNumber), 
        days(other.days), 
        advancePayment(other.advancePayment) {
        strcpy(this->name, other.name);
        strcpy(this->phone, other.phone);
    }

    void getdata() {
        cout << " Enter Customer Name: "; cin >> name;
        cout << " Enter Phone No: "; cin >> phone;
        cout << " Enter Check-in Days: "; cin >> days;
        cout << " Enter Advance Payment: "; cin >> advancePayment;
    }

    void showdata() const {
        cout << "\n Room Number: " << roomNumber;
        cout << "\n Customer Name: " << name;
        cout << "\n Phone No: " << phone;
        cout << "\n Staying for " << days << " days";
        cout << "\n Advance Paid: " << advancePayment;
    }

    int getRoomNumber() const { return roomNumber; }
    void setRoomNumber(int rno) { roomNumber = rno; }
    int getDays() const { return days; }
    float getAdvance() const { return advancePayment; }
    const char* getName() const { return name; }
};

template <typename T>
T add(T a, T b) {
    return a + b;
}

class HotelManager {
private:
    Room* rooms[5]; 
    const int MAX_ROOMS = 5;
    const char* FILE_NAME = "customer_records.dat";

    void loadRooms() {
        
        rooms[0] = new DeluxeRoom(101);
        rooms[1] = new Room(102, 2000.0);
        rooms[2] = new DeluxeRoom(103);
        rooms[3] = new Room(104, 2000.0);
        rooms[4] = new DeluxeRoom(105);
        
        ifstream fin(FILE_NAME, ios::binary);
        Customer cust;
        while (fin.read((char*)&cust, sizeof(cust))) {
            for (int i = 0; i < MAX_ROOMS; ++i) {
                if (rooms[i]->getRoomNo() == cust.getRoomNumber()) {
                    rooms[i]->book();
                    break;
                }
            }
        }
        fin.close();
    }

    void cleanup() {
        for (int i = 0; i < MAX_ROOMS; ++i) {
            delete rooms[i]; 
        }
    }

public:
    
    HotelManager() {
        loadRooms();
    }
    
    ~HotelManager() {
        cleanup();
    }

    void roomCheckIn() {
        int roomIndex = -1;
        cout << "\n--- Customer Check-In ---";
        
        for (int i = 0; i < MAX_ROOMS; ++i) {
            if (!rooms[i]->getStatus()) {
                cout << *rooms[i]; 
                roomIndex = i;
                break;
            }
        }

        if (roomIndex == -1) {
            cout << "\nSorry! All rooms are currently booked.";
            return;
        }

        cout << "\n\n Assigning Room No: " << rooms[roomIndex]->getRoomNo();
        Customer cust;
        cust.setRoomNumber(rooms[roomIndex]->getRoomNo());
        cust.getdata();
        
        ofstream fout(FILE_NAME, ios::binary | ios::app);
        fout.write((char*)&cust, sizeof(cust));
        fout.close();

        rooms[roomIndex]->book();
        cout << "\nRoom " << rooms[roomIndex]->getRoomNo() << " Successfully Booked!\n";
    }

    void showAllRooms() {
        cout << "\n--- All Room Details ---";
        for (int i = 0; i < MAX_ROOMS; ++i) {
            cout << *rooms[i];
        }
    }

    void checkOut(int rno) {
        
        ifstream fin(FILE_NAME, ios::binary);
        ofstream fout("temp.dat", ios::binary);
        Customer cust;
        bool found = false;
        int roomIndex = -1;

        for (int i = 0; i < MAX_ROOMS; ++i) {
            if (rooms[i]->getRoomNo() == rno) {
                roomIndex = i;
                break;
            }
        }

        if (roomIndex == -1) {
            cout << "\nInvalid Room Number!";
            fin.close(); fout.close(); remove("temp.dat"); return;
        }

        while (fin.read((char*)&cust, sizeof(cust))) {
            if (cust.getRoomNumber() != rno) {
                fout.write((char*)&cust, sizeof(cust));
            } else {
                found = true;
            
                float totalCharge = rooms[roomIndex]->getRent(cust.getDays());
                cout << "\n--- Customer Check-Out and Billing ---";
                cust.showdata();
                
                float finalBill = add(totalCharge, -cust.getAdvance()); 
                
                cout << "\n Total Room Charge: " << totalCharge;
                cout << "\n Net Payable Amount: " << finalBill;
                cout << "\n\nCheck-out Successful!";
            }
        }

        fin.close(); fout.close();

        if (found) {
            remove(FILE_NAME);
            rename("temp.dat", FILE_NAME);
            rooms[roomIndex]->unbook();
        } else {
            cout << "\nCustomer record for Room " << rno << " not found.";
            remove("temp.dat");
        }
    }

    void mainMenu() {
        int choice, rno;
        while (true) {
            system("cls"); 
            cout << "\n\t\t\t\t";
            cout << "\n\t\t\t\tHotel Management System";
            cout << "\n\t\t\t\t";
            cout << "\n\n\t1. Room Check-In";
            cout << "\n\t2. Show All Rooms";
            cout << "\n\t3. Customer Check-Out / Billing";
            cout << "\n\t4. Exit Program";
            cout << "\n\n\tEnter your Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: roomCheckIn(); break;
                case 2: showAllRooms(); break;
                case 3:
                    cout << "\n Enter Room Number for Check-Out: ";
                    cin >> rno;
                    checkOut(rno);
                    break;
                case 4:
                    cout << "\nThank you for using the system!\n";
                    exit(0);
                default:
                    cout << "\nInvalid choice. Please try again.";
            }
            cout << "\n\nPress Enter to return to the menu...";
            cin.ignore();
            cin.get();
        }
    }
};

int main() {
    HotelManager mgr;
    mgr.mainMenu();
    return 0;
}