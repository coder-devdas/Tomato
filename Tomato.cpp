#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<thread>
#include<chrono>
#include<stdexcept>
#include<iomanip>
#include<cstdlib>
#include<ctime>
#include<cmath>

using namespace std;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string CYAN = "\033[36m";
const string YELLOW = "\033[33m";
const string MAGENTA = "\033[35m";

class InsufficientFundsException : public runtime_error
{
public:
    InsufficientFundsException() : runtime_error("Payment Failed: Insufficient funds in wallet.") {}
};

class InvalidMenuException : public runtime_error
{
public:
    InvalidMenuException() : runtime_error("Error: That dish does not exist.") {}
};

enum class Category 
{ 
    STARTER, 
    MAIN_COURSE, 
    DESSERT, 
    BEVERAGE 
};

class MenuItem
{
private:
    string name;
    double price;
    Category category;
    int prepTime;

public:
    MenuItem(const string& new_name, double new_price, Category new_category, int new_prepTime)
    {
        name = new_name;
        price = new_price;
        category = new_category;
        prepTime = new_prepTime;
    }

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getPrepTime() const { return prepTime; }

    string getCategoryString() const
    {
        switch(category)
        {
            case Category::STARTER: return "Starter";
            case Category::MAIN_COURSE: return "Main Course";
            case Category::DESSERT: return "Dessert";
            case Category::BEVERAGE: return "Beverage";
            default: return "Unknown";
        }
    }
};

class Restaurant
{
private:
    string name;
    double rating;
    double locationX;
    double locationY;
    vector<MenuItem> menu;

public:
    Restaurant(const string& new_name, double new_rating, double loc_x, double loc_y)
    {
        name = new_name;
        rating = new_rating;
        locationX = loc_x;
        locationY = loc_y;
    }

    string getName() const { return name; }
    double getRating() const { return rating; }
    double getX() const { return locationX; }
    double getY() const { return locationY; }

    void addMenuItem(const MenuItem& item) 
    { 
        menu.push_back(item); 
    }

    void printMenu() const
    {
        cout << "\n-------------------------------------------------\n";
        cout << left << setw(20) << "Dish Name" << setw(15) << "Category" << setw(10) << "Price" << "\n";
        cout << "-------------------------------------------------\n";
        
        for(const auto& item : menu)
        {
            cout << left << setw(20) << item.getName() 
                 << setw(15) << item.getCategoryString() 
                 << "Rs. " << item.getPrice() << "\n";
        }
        cout << "-------------------------------------------------\n";
    }

    double getItemPrice(const string& target_name) const
    {
        for(const auto& item : menu)
        {
            if(item.getName() == target_name) return item.getPrice();
        }
        throw InvalidMenuException(); 
    }
    
    int getItemPrepTime(const string& target_name) const
    {
        for(const auto& item : menu)
        {
            if(item.getName() == target_name) return item.getPrepTime();
        }
        return 0;
    }
};

class User
{
protected:
    int id;
    string name;

public:
    User(int new_id, const string& new_name)
    {
        id = new_id;
        name = new_name;
    }

    virtual ~User() {}

    int getId() const { return id; }
    string getName() const { return name; }

    virtual void showDashboard() const = 0;
};

class Customer : public User
{
private:
    double walletBalance;
    double locationX;
    double locationY;
    vector<string> orderHistory;

public:
    Customer(int new_id, const string& new_name, double initial_funds, double loc_x, double loc_y) : User(new_id, new_name)
    {
        walletBalance = initial_funds;
        locationX = loc_x;
        locationY = loc_y;
    }

    double getWallet() const { return walletBalance; }
    double getX() const { return locationX; }
    double getY() const { return locationY; }

    void addFunds(double amount)
    {
        walletBalance += amount;
    }

    void deductFunds(double amount)
    {
        if(amount > walletBalance)
        {
            throw InsufficientFundsException();
        }
        walletBalance -= amount;
    }

    void addReceipt(const string& receipt)
    {
        orderHistory.push_back(receipt);
    }

    void showDashboard() const override
    {
        cout << "\n===================================\n";
        cout << "   PROFILE: " << name << "\n";
        cout << "===================================\n";
        cout << "Current Balance: Rs. " << fixed << setprecision(2) << walletBalance << "\n";
        cout << "\n--- Recent Orders ---\n";
        
        if(orderHistory.empty())
        {
            cout << "No orders yet.\n";
        }
        else 
        {
            int start = max(0, (int)orderHistory.size() - 5);
            for(int i = start; i < orderHistory.size(); ++i)
            {
                cout << " > " << orderHistory[i] << "\n";
            }
        }
        cout << "===================================\n";
    }
};

class DeliveryDriver : public User
{
private:
    double locationX;
    double locationY;
    double speed;
    bool isAvailable;

public:
    DeliveryDriver(int new_id, const string& new_name, double start_x, double start_y, double start_speed) : User(new_id, new_name)
    {
        locationX = start_x;
        locationY = start_y;
        speed = start_speed;
        isAvailable = true; 
    }

    double getX() const { return locationX; }
    double getY() const { return locationY; }
    double getSpeed() const { return speed; }
    bool getIsAvailable() const { return isAvailable; }

    void setAvailability(bool status)
    {
        isAvailable = status;
    }
    
    void toggleAvailability()
    {
        isAvailable = !isAvailable;
    }
    
    void updateLocation(double new_x, double new_y)
    {
        locationX = new_x;
        locationY = new_y;
    }

    void showDashboard() const override
    {
        cout << "\n===================================\n";
        cout << "   DRIVER PROFILE: " << name << "\n";
        cout << "===================================\n";
        cout << "Status: " << (isAvailable ? "Free" : "Busy") << "\n";
        cout << "Current Location: (X: " << locationX << ", Y: " << locationY << ")\n";
        cout << "===================================\n";
    }
};

class Order
{
public:
    int orderId;
    string customerName;
    string restaurantName;
    string orderType;
    double totalBill;
    string status;
    int assignedDriverId;
    double targetX;
    double targetY;

    Order(int id, string cName, string rName, string type, double bill, double cX, double cY)
    {
        orderId = id;
        customerName = cName;
        restaurantName = rName;
        orderType = type;
        totalBill = bill;
        targetX = cX;
        targetY = cY;
        status = "Pending";
        assignedDriverId = 0;
    }
};

class TomatoApp
{
private:
    Restaurant myRest;
    Customer loggedInUser;
    vector<DeliveryDriver> drivers;
    vector<Order> globalOrders;
    int orderCounter;

    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string MAGENTA = "\033[35m";

    void clearScreen()
    {
        cout << "\033[2J\033[1;1H";
    }

    void simulateLoadingBar(const string& taskName, int durationMs)
    {
        cout << "\n" << CYAN << "[SYSTEM] " << taskName << " " << RESET;
        for(int i = 0; i < 15; ++i)
        {
            cout << GREEN << "█" << RESET;
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(durationMs / 15));
        }
        cout << GREEN << " DONE!" << RESET << "\n";
    }

    void generateQRCode()
    {
        cout << "\nScan QR Code to Pay:\n";
        cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n";
        for(int i = 0; i < 7; i++)
        {
            cout << "█ ";
            for(int j = 0; j < 7; j++)
            {
                if(rand() % 2 == 0) cout << "██";
                else cout << "  ";
            }
            cout << " █\n";
        }
        cout << "▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀\n";
    }

    void restaurantPortal()
    {
        clearScreen();
        cout << YELLOW << "=== RESTAURANT LOGIN ===\n" << RESET;
        cout << "Enter Admin Password: ";
        string pass;
        cin >> pass;

        if(pass != "1234")
        {
            cout << RED << "Wrong Password!\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        bool inRestro = true;
        while(inRestro)
        {
            clearScreen();
            cout << YELLOW << "=== " << myRest.getName() << " DASHBOARD ===\n" << RESET;
            cout << "1. View Pending Orders\n";
            cout << "2. Logout\n";
            cout << CYAN << "Select option: " << RESET;
            
            int choice;
            cin >> choice;
            if(cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

            if(choice == 1)
            {
                cout << "\n--- PENDING ORDERS ---\n";
                bool found = false;
                for(auto& ord : globalOrders)
                {
                    if(ord.status == "Pending")
                    {
                        found = true;
                        cout << "ID: " << ord.orderId << " | Customer: " << ord.customerName 
                             << " | Type: " << ord.orderType << " | Bill: Rs." << ord.totalBill << "\n";
                    }
                }

                if(!found)
                {
                    cout << "No pending orders right now.\n";
                }
                else
                {
                    cout << "\nEnter Order ID to Accept (or 0 to cancel): ";
                    int accId;
                    cin >> accId;

                    if(accId != 0)
                    {
                        DeliveryDriver* assigned = nullptr;
                        for(auto& d : drivers)
                        {
                            if(d.getIsAvailable())
                            {
                                assigned = &d;
                                break;
                            }
                        }

                        if(assigned == nullptr)
                        {
                            cout << RED << "No free drivers available!\n" << RESET;
                        }
                        else
                        {
                            for(auto& ord : globalOrders)
                            {
                                if(ord.orderId == accId)
                                {
                                    ord.status = "Preparing";
                                    ord.assignedDriverId = assigned->getId();
                                    assigned->setAvailability(false);
                                    
                                    simulateLoadingBar("Assigning Driver & Starting Prep...", 800);
                                    cout << GREEN << "Order Accepted! Kitchen is preparing food. Assigned to: " << assigned->getName() << RESET << "\n";
                                    break;
                                }
                            }
                        }
                    }
                }
                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 2)
            {
                inRestro = false;
            }
        }
    }
    
    void deliveryDriverPortal()
    {
        clearScreen();
        cout << YELLOW << "=== DRIVER LOGIN ===\n" << RESET;
        cout << "Enter Driver ID: ";
        int dId;
        cin >> dId;

        DeliveryDriver* currentDriver = nullptr;
        for(auto& d : drivers)
        {
            if(d.getId() == dId)
            {
                currentDriver = &d;
                break;
            }
        }

        if(currentDriver == nullptr)
        {
            cout << RED << "Driver not found!\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        bool inDriver = true;
        while(inDriver)
        {
            clearScreen();
            cout << YELLOW << "=== " << currentDriver->getName() << "'s DASHBOARD ===\n" << RESET;
            cout << "1. View Assigned Deliveries\n";
            cout << "2. Mark Delivery Complete\n";
            cout << "3. Toggle Availability (Current: " << (currentDriver->getIsAvailable() ? GREEN + "Free" : RED + "Busy") << RESET << ")\n";
            cout << "4. Logout\n";
            cout << CYAN << "Select option: " << RESET;
            
            int choice;
            cin >> choice;
            if(cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

            if(choice == 1)
            {
                cout << "\n--- ASSIGNED ORDERS ---\n";
                bool found = false;
                for(auto& ord : globalOrders)
                {
                    if(ord.assignedDriverId == currentDriver->getId() && (ord.status == "Preparing" || ord.status == "Out for Delivery"))
                    {
                        found = true;
                        cout << "ID: " << ord.orderId << " | Customer: " << ord.customerName 
                             << " | Status: " << ord.status << "\n";
                        
                        if(ord.status == "Preparing")
                        {
                            cout << CYAN << "-> Action Required: Pick up from " << ord.restaurantName << " and change status to 'Out for Delivery'.\n" << RESET;
                        }
                    }
                }
                if(!found) cout << "No active deliveries assigned.\n";
                
                cout << "\nEnter Order ID to change status to 'Out for Delivery' (or 0 to cancel): ";
                int pickId;
                cin >> pickId;
                
                if(pickId != 0)
                {
                    for(auto& ord : globalOrders)
                    {
                        if(ord.orderId == pickId && ord.assignedDriverId == currentDriver->getId())
                        {
                            ord.status = "Out for Delivery";
                            simulateLoadingBar("Picking up food...", 800);
                            cout << GREEN << "Order picked up! Drive safe.\n" << RESET;
                            break;
                        }
                    }
                }
                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 2)
            {
                cout << "\nEnter Order ID to mark as Delivered: ";
                int delId;
                cin >> delId;
                
                bool found = false;
                for(auto& ord : globalOrders)
                {
                    if(ord.orderId == delId && ord.assignedDriverId == currentDriver->getId() && ord.status == "Out for Delivery")
                    {
                        found = true;
                        ord.status = "Delivered";
                        currentDriver->setAvailability(true);
                        currentDriver->updateLocation(ord.targetX, ord.targetY);
                        
                        simulateLoadingBar("Completing Delivery...", 1000);
                        cout << GREEN << "Delivery Complete! You are now marked as Free for new orders.\n" << RESET;
                        break;
                    }
                }
                if(!found) cout << RED << "Invalid Order ID or order is not currently 'Out for Delivery'.\n" << RESET;
                
                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 3)
            {
                currentDriver->toggleAvailability();
                cout << "Availability changed.\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
            else if(choice == 4)
            {
                inDriver = false;
            }
        }
    }
    
    void customerPortal()
    {
        bool inCust = true;
        while(inCust)
        {
            clearScreen();
            cout << YELLOW << "=== CUSTOMER PORTAL ===\n" << RESET;
            cout << "1. Place New Order\n";
            cout << "2. Check Order Status\n";
            cout << "3. View Wallet & Profile\n";
            cout << "4. Logout\n";
            cout << CYAN << "Select option: " << RESET;
            
            int choice;
            cin >> choice;
            if(cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

            if(choice == 1)
            {
                myRest.printMenu();
                cout << CYAN << "\nEnter Dish Name: " << RESET;
                string dish;
                cin >> ws; getline(cin, dish);

                try
                {
                    double basePrice = myRest.getItemPrice(dish);
                    
                    cout << CYAN << "Choose Order Type (Normal / Priority): " << RESET;
                    string type;
                    cin >> type;

                    double finalPrice = basePrice;
                    if(type == "Priority" || type == "priority")
                    {
                        double dist = sqrt(pow(loggedInUser.getX() - myRest.getX(), 2) + pow(loggedInUser.getY() - myRest.getY(), 2));
                        finalPrice += (10 * dist); 
                    }

                    cout << YELLOW << "\nInitiating checkout for Rs. " << finalPrice << "...\n" << RESET;
                    simulateLoadingBar("Connecting to Secure Payment Gateway...", 1200);

                    loggedInUser.deductFunds(finalPrice); 

                    cout << GREEN << "\nTotal is: Rs. " << finalPrice << "\n" << RESET;
                    generateQRCode();
                    
                    simulateLoadingBar("Verifying Payment...", 1500);
                    cout << GREEN << "\n[SUCCESS] Payment Approved.\n" << RESET;

                    Order newOrder(orderCounter++, loggedInUser.getName(), myRest.getName(), type, finalPrice, loggedInUser.getX(), loggedInUser.getY());
                    globalOrders.push_back(newOrder);

                    string receipt = "Order #" + to_string(newOrder.orderId) + " (" + dish + ") - Rs. " + to_string(finalPrice);
                    loggedInUser.addReceipt(receipt);

                    cout << "Order placed successfully! Your Order ID is: " << YELLOW << newOrder.orderId << RESET << "\n";
                }
                catch(const InsufficientFundsException& e)
                {
                    cout << RED << "\n[DECLINED] " << e.what() << "\n";
                    cout << "Please add money to your wallet to complete this order." << RESET << "\n";
                }
                catch(const InvalidMenuException& e)
                {
                    cout << RED << "\n[!] " << e.what() << RESET << "\n";
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 2)
            {
                cout << CYAN << "Enter your Order ID: " << RESET;
                int searchId;
                cin >> searchId;

                bool found = false;
                for(const auto& ord : globalOrders)
                {
                    if(ord.orderId == searchId && ord.customerName == loggedInUser.getName())
                    {
                        found = true;
                        cout << "\n--- ORDER DETAILS ---\n";
                        cout << "Customer: " << ord.customerName << "\n";
                        cout << "Type: " << ord.orderType << "\n";
                        cout << "Total: Rs. " << ord.totalBill << "\n";
                        cout << "Status: " << CYAN << ord.status << RESET << "\n";
                        
                        if(ord.status == "Accepted" || ord.status == "Preparing")
                        {
                            simulateLoadingBar("Pinging Kitchen...", 800);
                            cout << GREEN << "\nYour food is currently being prepared!" << RESET << "\n";
                        }
                        else if(ord.status == "Out for Delivery")
                        {
                            simulateLoadingBar("Pinging Driver GPS...", 800);
                            cout << GREEN << "\nYour Zomato Partner is on the way!" << RESET << "\n";
                        }
                    }
                }
                if(!found) cout << RED << "Order not found!\n" << RESET;

                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 3)
            {
                loggedInUser.showDashboard();
                cout << CYAN << "\nAdd funds to wallet? (y/n): " << RESET;
                char ans;
                cin >> ans;
                if(ans == 'y' || ans == 'Y')
                {
                    cout << CYAN << "Enter amount: Rs. " << RESET;
                    double amt;
                    cin >> amt;
                    if(!cin.fail() && amt > 0)
                    {
                        simulateLoadingBar("Processing Bank Transfer...", 1000);
                        loggedInUser.addFunds(amt);
                        cout << GREEN << "[SUCCESS] Funds added.\n" << RESET;
                    }
                }
                cout << "\nPress Enter to continue...";
                cin.ignore(10000, '\n'); cin.get();
            }
            else if(choice == 4)
            {
                inCust = false;
            }
        }
    }

public:
    TomatoApp() : 
        myRest("Hotel Devdas", 4.8, 15.0, 20.0), 
        loggedInUser(1, "Prathamesh", 500.0, 0.0, 0.0)
    {
        orderCounter = 1;
        srand(time(0)); 
        
        drivers.push_back(DeliveryDriver(101, "Ram", 0.0, 0.0, 50.0));
        drivers.push_back(DeliveryDriver(102, "Raju", 5.0, 5.0, 60.0));

        myRest.addMenuItem(MenuItem("Paneer Butter Masala", 100.0, Category::MAIN_COURSE, 15));
        myRest.addMenuItem(MenuItem("Chicken Biryani", 110.0, Category::MAIN_COURSE, 20));
        myRest.addMenuItem(MenuItem("Veg Fried Rice", 120.0, Category::MAIN_COURSE, 15));
        myRest.addMenuItem(MenuItem("Butter Naan", 40.0, Category::MAIN_COURSE, 5));
        myRest.addMenuItem(MenuItem("Gulab Jamun", 50.0, Category::DESSERT, 2));
    }

    void run()
    {
        bool isRunning = true;
        while(isRunning)
        {
            clearScreen();
            cout << RED << "************************************************************\n" << RESET;
            cout << YELLOW << "==================== Welcome to Tomato =====================\n" << RESET;
            cout << RED << "------------------------------------------------------------\n" << RESET;
            cout << "\nYou're a:\n";
            cout << "1. Restaurant Admin\n";
            cout << "2. Customer\n";
            cout << "3. Delivery Driver\n";
            cout << "4. Exit\n";
            cout << CYAN << "\nEnter choice: " << RESET;

            int role;
            cin >> role;

            if(cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

            if(role == 1) restaurantPortal();
            else if(role == 2) customerPortal();
            else if(role == 3) deliveryDriverPortal();
            else if(role == 4)
            {
                cout << YELLOW << "\nThank you for using Tomato!\n" << RESET;
                isRunning = false;
            }
        }
    }
};

int main()
{
    TomatoApp app;
    app.run();
    return 0;
}