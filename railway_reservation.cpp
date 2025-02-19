#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits>
#include <random>
#include <stdexcept>
#include <fstream>

// Custom exceptions
class TrainNotFoundException : public std::runtime_error {
public:
    TrainNotFoundException(int id) : 
        std::runtime_error("Train with ID " + std::to_string(id) + " not found!") {}
};

class SeatNotFoundException : public std::runtime_error {
public:
    SeatNotFoundException(int trainId, int seatNo) : 
        std::runtime_error("Seat number " + std::to_string(seatNo) + 
                         " on train " + std::to_string(trainId) + " is invalid!") {}
};

class NoSeatsAvailableException : public std::runtime_error {
public:
    NoSeatsAvailableException(int trainId) : 
        std::runtime_error("No seats available on train " + std::to_string(trainId) + "!") {}
};

class TicketNotFoundException : public std::runtime_error {
public:
    TicketNotFoundException(const std::string& bookingId) : 
        std::runtime_error("Ticket with booking ID " + bookingId + " not found!") {}
};

class FileIOException : public std::runtime_error {
public:
    FileIOException(const std::string& filename, const std::string& operation) : 
        std::runtime_error("Failed to " + operation + " file: " + filename) {}
};

class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException(const std::string& message) : 
        std::runtime_error("Invalid input: " + message) {}
};

class Train {
private:
    int trainId;
    std::string trainName;
    int totalSeats;
    std::vector<bool> seatAvailability; // true means seat is available

public:
    Train(int id, std::string name, int seats) : 
        trainId(id), trainName(name), totalSeats(seats) {
        // Validate input parameters
        if (id <= 0) throw InvalidInputException("Train ID must be positive");
        if (name.empty()) throw InvalidInputException("Train name cannot be empty");
        if (seats <= 0) throw InvalidInputException("Number of seats must be positive");
        
        seatAvailability.resize(totalSeats, true);
    }

    int getTrainId() const { return trainId; }
    std::string getTrainName() const { return trainName; }
    int getTotalSeats() const { return totalSeats; }
    
    bool isSeatAvailable(int seatNumber) const {
        if (seatNumber < 1 || seatNumber > totalSeats) {
            throw SeatNotFoundException(trainId, seatNumber);
        }
        return seatAvailability[seatNumber - 1];
    }
    
    int getAvailableSeatsCount() const {
        return std::count(seatAvailability.begin(), seatAvailability.end(), true);
    }
    
    int bookNextAvailableSeat() {
        for (int i = 0; i < totalSeats; i++) {
            if (seatAvailability[i]) {
                seatAvailability[i] = false;
                return i + 1; // Seat number (1-based)
            }
        }
        throw NoSeatsAvailableException(trainId);
    }
    
    bool bookSpecificSeat(int seatNumber) {
        if (seatNumber < 1 || seatNumber > totalSeats) {
            throw SeatNotFoundException(trainId, seatNumber);
        }
        
        if (seatAvailability[seatNumber - 1]) {
            seatAvailability[seatNumber - 1] = false;
            return true;
        }
        
        return false; // Seat already booked
    }
    
    bool cancelSeat(int seatNumber) {
        if (seatNumber < 1 || seatNumber > totalSeats) {
            throw SeatNotFoundException(trainId, seatNumber);
        }
        
        if (!seatAvailability[seatNumber - 1]) {
            seatAvailability[seatNumber - 1] = true;
            return true;
        }
        
        return false; // Seat was already available (not booked)
    }
};

class Ticket {
private:
    std::string bookingId;
    int trainId;
    int seatNumber;
    std::string passengerName;
    std::string bookingTime;
    
public:
    Ticket(std::string id, int train, int seat, std::string passenger) :
        bookingId(id), trainId(train), seatNumber(seat), passengerName(passenger) {
        
        // Validate input parameters
        if (id.empty()) throw InvalidInputException("Booking ID cannot be empty");
        if (train <= 0) throw InvalidInputException("Train ID must be positive");
        if (seat <= 0) throw InvalidInputException("Seat number must be positive");
        if (passenger.empty()) throw InvalidInputException("Passenger name cannot be empty");
        
        // Generate current timestamp for booking
        time_t now = time(0);
        tm* ltm = localtime(&now);
        std::stringstream ss;
        ss << std::setfill('0') 
           << std::setw(2) << 1 + ltm->tm_mon << "/"
           << std::setw(2) << ltm->tm_mday << "/"
           << 1900 + ltm->tm_year << " "
           << std::setw(2) << ltm->tm_hour << ":"
           << std::setw(2) << ltm->tm_min << ":"
           << std::setw(2) << ltm->tm_sec;
        bookingTime = ss.str();
    }
    
    std::string getBookingId() const { return bookingId; }
    int getTrainId() const { return trainId; }
    int getSeatNumber() const { return seatNumber; }
    std::string getPassengerName() const { return passengerName; }
    std::string getBookingTime() const { return bookingTime; }
    
    void displayTicket() const {
        std::cout << "\n========== TICKET DETAILS ==========\n";
        std::cout << "Booking ID: " << bookingId << std::endl;
        std::cout << "Train ID: " << trainId << std::endl;
        std::cout << "Seat Number: " << seatNumber << std::endl;
        std::cout << "Passenger Name: " << passengerName << std::endl;
        std::cout << "Booking Time: " << bookingTime << std::endl;
        std::cout << "===================================\n";
    }
};

class ReservationSystem {
private:
    std::vector<Train> trains;
    std::unordered_map<std::string, Ticket> bookings;
    std::random_device rd;
    std::mt19937 gen;
    
    std::string generateBookingId() {
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<> dis(0, chars.size() - 1);
        
        std::string id = "BK";
        for (int i = 0; i < 8; i++) {
            id += chars[dis(gen)];
        }
        
        // Check if this ID already exists (unlikely but possible)
        if (bookings.find(id) != bookings.end()) {
            return generateBookingId(); // try again
        }
        
        return id;
    }
    
public:
    ReservationSystem() : gen(rd()) {
        try {
            // Initialize with some trains
            trains.push_back(Train(1001, "Express Delhi", 100));
            trains.push_back(Train(1002, "Mumbai Local", 100));
            trains.push_back(Train(1003, "Chennai Mail", 100));
            trains.push_back(Train(1004, "Kolkata Express", 100));
        } catch (const InvalidInputException& e) {
            std::cerr << "Error during initialization: " << e.what() << std::endl;
            // In a real application, you might want to log this and take appropriate action
        }
    }
    
    void displayAllTrains() {
        if (trains.empty()) {
            std::cout << "No trains available in the system.\n";
            return;
        }
        
        std::cout << "\n========== AVAILABLE TRAINS ==========\n";
        std::cout << std::left << std::setw(10) << "Train ID"
                 << std::setw(20) << "Train Name"
                 << std::setw(15) << "Total Seats"
                 << std::setw(15) << "Available Seats" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        for (const auto& train : trains) {
            std::cout << std::left << std::setw(10) << train.getTrainId()
                     << std::setw(20) << train.getTrainName()
                     << std::setw(15) << train.getTotalSeats()
                     << std::setw(15) << train.getAvailableSeatsCount() << std::endl;
        }
        std::cout << "=====================================\n";
    }
    
    void checkSeatAvailability(int trainId) {
        try {
            const Train& train = findTrain(trainId);
            int availableSeats = train.getAvailableSeatsCount();
            std::cout << "Train " << trainId << " (" << train.getTrainName() << ") has " 
                      << availableSeats << " seat(s) available out of " 
                      << train.getTotalSeats() << std::endl;
            
            if (availableSeats == 0) {
                std::cout << "Sorry, the train is fully booked.\n";
            }
        } catch (const TrainNotFoundException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    std::string bookTicket(int trainId, const std::string& passengerName) {
        if (passengerName.empty()) {
            std::cerr << "Error: Passenger name cannot be empty.\n";
            return "";
        }
        
        try {
            Train& train = findTrainRef(trainId);
            
            try {
                int seatNumber = train.bookNextAvailableSeat();
                std::string bookingId = generateBookingId();
                
                try {
                    bookings.emplace(bookingId, Ticket(bookingId, trainId, seatNumber, passengerName));
                    
                    std::cout << "Ticket booked successfully!\n";
                    bookings.at(bookingId).displayTicket();
                    
                    return bookingId;
                } catch (const InvalidInputException& e) {
                    // Undo seat booking if ticket creation fails
                    train.cancelSeat(seatNumber);
                    std::cerr << "Error creating ticket: " << e.what() << std::endl;
                    return "";
                }
            } catch (const NoSeatsAvailableException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return "";
            }
        } catch (const TrainNotFoundException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "";
        }
    }
    
    bool cancelTicket(const std::string& bookingId) {
        try {
            const Ticket& ticket = findTicket(bookingId);
            int trainId = ticket.getTrainId();
            int seatNumber = ticket.getSeatNumber();
            
            try {
                Train& train = findTrainRef(trainId);
                
                try {
                    if (train.cancelSeat(seatNumber)) {
                        std::cout << "Ticket with Booking ID " << bookingId << " cancelled successfully!\n";
                        bookings.erase(bookingId);
                        return true;
                    } else {
                        std::cerr << "Failed to cancel seat. This is unexpected.\n";
                        return false;
                    }
                } catch (const SeatNotFoundException& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                    return false;
                }
            } catch (const TrainNotFoundException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return false;
            }
        } catch (const TicketNotFoundException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool checkTicketStatus(const std::string& bookingId) {
        try {
            const Ticket& ticket = findTicket(bookingId);
            std::cout << "Ticket found! Here are the details:\n";
            ticket.displayTicket();
            return true;
        } catch (const TicketNotFoundException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
    
    void loadTrainsFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw FileIOException(filename, "open");
        }
        
        // Clear existing trains
        trains.clear();
        
        std::string line;
        // Skip header line
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            
            try {
                int trainId;
                std::string trainName;
                int totalSeats;
                int availableSeats;
                
                // Parse trainId
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing train ID");
                try {
                    trainId = std::stoi(token);
                } catch (const std::exception&) {
                    throw InvalidInputException("train ID is not a valid number: " + token);
                }
                
                // Parse trainName
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing train name");
                trainName = token;
                
                // Parse totalSeats
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing total seats");
                try {
                    totalSeats = std::stoi(token);
                } catch (const std::exception&) {
                    throw InvalidInputException("total seats is not a valid number: " + token);
                }
                
                // Parse availableSeats
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing available seats");
                try {
                    availableSeats = std::stoi(token);
                } catch (const std::exception&) {
                    throw InvalidInputException("available seats is not a valid number: " + token);
                }
                
                // Create and add the train
                Train train(trainId, trainName, totalSeats);
                
                // If some seats are already booked, mark them as unavailable
                int bookedSeats = totalSeats - availableSeats;
                for (int i = 0; i < bookedSeats && i < totalSeats; i++) {
                    try {
                        train.bookNextAvailableSeat();
                    } catch (const NoSeatsAvailableException&) {
                        std::cerr << "Warning: CSV file has inconsistent seat data for train " << trainId << std::endl;
                        break;
                    }
                }
                
                trains.push_back(train);
                
            } catch (const InvalidInputException& e) {
                std::cerr << "Error parsing CSV line: " << e.what() << std::endl;
                std::cerr << "Line content: " << line << std::endl;
                // Continue to next line
            }
        }
        
        std::cout << "Loaded " << trains.size() << " trains from " << filename << std::endl;
    }
    
    void saveTrainsToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw FileIOException(filename, "open for writing");
        }
        
        // Write header
        file << "trainId,trainName,totalSeats,availableSeats\n";
        
        // Write train data
        for (const auto& train : trains) {
            file << train.getTrainId() << ","
                 << train.getTrainName() << ","
                 << train.getTotalSeats() << ","
                 << train.getAvailableSeatsCount() << "\n";
        }
        
        if (file.fail()) {
            throw FileIOException(filename, "write to");
        }
        
        std::cout << "Saved " << trains.size() << " trains to " << filename << std::endl;
    }
    
    void loadTicketsFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw FileIOException(filename, "open");
        }
        
        // Clear existing bookings
        bookings.clear();
        
        std::string line;
        // Skip header line
        std::getline(file, line);
        
        int loadedTickets = 0;
        int errorCount = 0;
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            
            try {
                std::string bookingId;
                int trainId;
                int seatNumber;
                std::string passengerName;
                std::string bookingTime;
                
                // Parse bookingId
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing booking ID");
                bookingId = token;
                
                // Parse trainId
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing train ID");
                try {
                    trainId = std::stoi(token);
                } catch (const std::exception&) {
                    throw InvalidInputException("train ID is not a valid number: " + token);
                }
                
                // Parse seatNumber
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing seat number");
                try {
                    seatNumber = std::stoi(token);
                } catch (const std::exception&) {
                    throw InvalidInputException("seat number is not a valid number: " + token);
                }
                
                // Parse passengerName
                if (!std::getline(ss, token, ',')) throw InvalidInputException("missing passenger name");
                passengerName = token;
                
                // Parse bookingTime (rest of the line)
                std::getline(ss, bookingTime);
                
                // Find the corresponding train
                try {
                    Train& train = findTrainRef(trainId);
                    
                    // Try to book the specific seat
                    try {
                        if (train.bookSpecificSeat(seatNumber)) {
                            // Create a ticket with the loaded data
                            try {
                                bookings.emplace(bookingId, Ticket(bookingId, trainId, seatNumber, passengerName));
                                loadedTickets++;
                            } catch (const InvalidInputException& e) {
                                std::cerr << "Error creating ticket from CSV: " << e.what() << std::endl;
                                std::cerr << "Line content: " << line << std::endl;
                                // Undo the seat booking
                                train.cancelSeat(seatNumber);
                                errorCount++;
                            }
                        } else {
                            std::cerr << "Warning: Seat " << seatNumber << " on train " << trainId 
                                      << " is already booked. Skipping ticket: " << bookingId << std::endl;
                            errorCount++;
                        }
                    } catch (const SeatNotFoundException& e) {
                        std::cerr << "Error booking seat from CSV: " << e.what() << std::endl;
                        std::cerr << "Line content: " << line << std::endl;
                        errorCount++;
                    }
                } catch (const TrainNotFoundException& e) {
                    std::cerr << "Error finding train from CSV: " << e.what() << std::endl;
                    std::cerr << "Line content: " << line << std::endl;
                    errorCount++;
                }
                
            } catch (const InvalidInputException& e) {
                std::cerr << "Error parsing CSV line: " << e.what() << std::endl;
                std::cerr << "Line content: " << line << std::endl;
                errorCount++;
                // Continue to next line
            }
        }
        
        std::cout << "Loaded " << loadedTickets << " tickets from " << filename << std::endl;
        if (errorCount > 0) {
            std::cout << "Warning: " << errorCount << " tickets could not be loaded due to errors." << std::endl;
        }
    }
    
    void saveTicketsToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw FileIOException(filename, "open for writing");
        }
        
        // Write header
        file << "bookingId,trainId,seatNumber,passengerName,bookingTime\n";
        
        // Write ticket data
        for (const auto& pair : bookings) {
            const std::string& id = pair.first;
            const Ticket& ticket = pair.second;        
            file << ticket.getBookingId() << ","
                 << ticket.getTrainId() << ","
                 << ticket.getSeatNumber() << ","
                 << ticket.getPassengerName() << ","
                 << ticket.getBookingTime() << "\n";
        }
        
        if (file.fail()) {
            throw FileIOException(filename, "write to");
        }
        
        std::cout << "Saved " << bookings.size() << " tickets to " << filename << std::endl;
    }
    
private:
    // Helper method to find a train by ID (const version)
    const Train& findTrain(int trainId) const {
        for (const auto& train : trains) {
            if (train.getTrainId() == trainId) {
                return train;
            }
        }
        throw TrainNotFoundException(trainId);
    }
    
    // Helper method to find a train by ID (non-const version for modifications)
    Train& findTrainRef(int trainId) {
        for (auto& train : trains) {
            if (train.getTrainId() == trainId) {
                return train;
            }
        }
        throw TrainNotFoundException(trainId);
    }
    
    // Helper method to find a ticket by booking ID
    const Ticket& findTicket(const std::string& bookingId) const {
        auto it = bookings.find(bookingId);
        if (it == bookings.end()) {
            throw TicketNotFoundException(bookingId);
        }
        return it->second;
    }
};

// Function to safely get integer input from user
int getIntInput() {
    int value;
    std::string input;
    
    std::getline(std::cin, input);
    
    try {
        // Handle empty input
        if (input.empty()) {
            throw InvalidInputException("input is empty");
        }
        
        // Check if input contains only digits
        for (char c : input) {
            if (!std::isdigit(c)) {
                throw InvalidInputException("input contains non-numeric characters");
            }
        }
        
        value = std::stoi(input);
        
        // Check if value is in reasonable range
        if (value < 0 || value > 10000) {
            throw InvalidInputException("input is out of reasonable range (0-10000)");
        }
        
        return value;
        
    } catch (const InvalidInputException& e) {
        std::cerr << "Invalid input: " << e.what() << std::endl;
        std::cerr << "Please enter a valid number." << std::endl;
        return -1; // Indicate error
    } catch (const std::out_of_range&) {
        std::cerr << "Invalid input: number is too large" << std::endl;
        return -1; // Indicate error
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return -1; // Indicate error
    }
}

void displayMainMenu() {
    std::cout << "\n====== RAILWAY RESERVATION SYSTEM ======\n";
    std::cout << "1. Display All Trains\n";
    std::cout << "2. Check Seat Availability\n";
    std::cout << "3. Book a Ticket\n";
    std::cout << "4. Cancel a Ticket\n";
    std::cout << "5. Check Ticket Status\n";
    std::cout << "0. Exit\n";
    std::cout << "========================================\n";
    std::cout << "Enter your choice: ";
}

int main() {
    ReservationSystem reservationSystem;
    int choice;
    
    std::cout << "Welcome to Railway Reservation System!\n";
    
    try {
        // Try to load data from CSV files
        try {
            reservationSystem.loadTrainsFromCSV("trains.csv");
        } catch (const FileIOException& e) {
            std::cout << "Note: " << e.what() << ". Using default trains." << std::endl;
        }
        
        try {
            reservationSystem.loadTicketsFromCSV("tickets.csv");
        } catch (const FileIOException& e) {
            std::cout << "Note: " << e.what() << ". Starting with no existing bookings." << std::endl;
        }
        
        do {
            displayMainMenu();
            choice = getIntInput();
            
            // Handle invalid input
            if (choice < 0) {
                std::cout << "Please try again.\n";
                continue;
            }
            
            switch (choice) {
                case 1: {
                    reservationSystem.displayAllTrains();
                    break;
                }
                case 2: {
                    std::cout << "Enter Train ID: ";
                    int trainId = getIntInput();
                    if (trainId > 0) {
                        reservationSystem.checkSeatAvailability(trainId);
                    }
                    break;
                }
                case 3: {
                    std::cout << "Enter Train ID: ";
                    int trainId = getIntInput();
                    if (trainId <= 0) {
                        break;
                    }
                    
                    std::string passengerName;
                    std::cout << "Enter Passenger Name: ";
                    std::getline(std::cin, passengerName);
                    
                    if (passengerName.empty()) {
                        std::cerr << "Error: Passenger name cannot be empty.\n";
                        break;
                    }
                    
                    std::string result = reservationSystem.bookTicket(trainId, passengerName);
                    if (result.empty()) {
                        std::cout << "Ticket booking failed.\n";
                    }
                    break;
                }
                case 4: {
                    std::string bookingId;
                    std::cout << "Enter Booking ID: ";
                    std::getline(std::cin, bookingId);
                    
                    if (bookingId.empty()) {
                        std::cerr << "Error: Booking ID cannot be empty.\n";
                        break;
                    }
                    
                    reservationSystem.cancelTicket(bookingId);
                    break;
                }
                case 5: {
                    std::string bookingId;
                    std::cout << "Enter Booking ID: ";
                    std::getline(std::cin, bookingId);
                    
                    if (bookingId.empty()) {
                        std::cerr << "Error: Booking ID cannot be empty.\n";
                        break;
                    }
                    
                    reservationSystem.checkTicketStatus(bookingId);
                    break;
                }
                case 0:
                    // Save data to CSV files before exiting
                    try {
                        reservationSystem.saveTrainsToCSV("trains.csv");
                    } catch (const FileIOException& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                        std::cerr << "Train data was not saved." << std::endl;
                    }
                    
                    try {
                        reservationSystem.saveTicketsToCSV("tickets.csv");
                    } catch (const FileIOException& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                        std::cerr << "Ticket data was not saved." << std::endl;
                    }
                    
                    std::cout << "Thank you for using Railway Reservation System. Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice. Please enter a number between 0 and 5.\n";
            }
            
        } while (choice != 0);
        
    } catch (const std::exception& e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
        std::cerr << "The application will now exit." << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown critical error occurred. The application will now exit." << std::endl;
        return 1;
    }
    
    return 0;
}
