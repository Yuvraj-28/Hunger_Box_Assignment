# Hunger_Box_Assignment
Railway Reservation System 

## Overview
This is a simple command-line based Railway Reservation System implemented in C++. The system allows users to check seat availability, book tickets, cancel tickets, and check ticket status using a unique booking ID.

## Features Implemented

1. **Train & Seat Management**
   - System maintains a fixed number of trains (4 in this implementation)
   - Each train has a unique Train ID and name
   - Each train has a fixed number of seats (100 per train)
   - System tracks available and booked seats for each train

2. **Ticket Booking**
   - Users can book a single seat per request on a selected train
   - A unique Booking ID is generated for each successful booking
   - System prevents booking when no seats are available

3. **Ticket Cancellation**
   - Users can cancel a ticket using their Booking ID
   - Cancelled seats are made available for new bookings

4. **Ticket Status Check**
   - Users can check their ticket details using the Booking ID
   - System provides full details including passenger name, seat number, and booking time

## Setup and Installation

### Prerequisites
- C++ compiler (GCC, Clang, or MSVC)
- Standard C++ libraries

### Compilation
1. Save the code in a file named `railway_reservation.cpp`
2. Compile the code using one of the following commands:

**For GCC (Linux/macOS):**
```bash
g++ -std=c++11 railway_reservation.cpp -o railway_reservation
```

**For Clang (macOS/Linux):**
```bash
clang++ -std=c++11 railway_reservation.cpp -o railway_reservation
```

**For MSVC (Windows):**
```bash
cl /EHsc railway_reservation.cpp /Fe:railway_reservation.exe
```

### Running the Application
After compilation, run the executable:

**On Linux/macOS:**
```bash
./railway_reservation
```

**On Windows:**
```bash
railway_reservation.exe
```

## Usage Guide

Upon starting the application, you'll see the main menu with these options:

1. **Display All Trains** - Shows a list of all trains with their IDs, names, and seat availability
2. **Check Seat Availability** - Checks if seats are available on a specific train
3. **Book a Ticket** - Books a ticket for a passenger on a specific train
4. **Cancel a Ticket** - Cancels an existing ticket using a Booking ID
5. **Check Ticket Status** - Shows details of a ticket using the Booking ID
0. **Exit** - Exits the application

### Workflow Example:
1. First, display all trains (Option 1) to see available trains
2. Check seat availability (Option 2) by entering a train ID
3. Book a ticket (Option 3) by entering train ID and passenger name
4. Note down the generated Booking ID
5. Check ticket status (Option 5) using the Booking ID
6. Cancel the ticket (Option 4) if needed

## Implementation Details

### Classes
- **Train**: Manages train details and seat availability
- **Ticket**: Stores ticket information including booking ID, passenger details, and timestamp
- **ReservationSystem**: Main class that handles bookings, cancellations, and ticket status

### Data Structures
- Vector of Train objects to store all trains
- Unordered map to store bookings with Booking ID as key

### Features
- Auto-generation of unique booking IDs
- Real-time tracking of seat availability
- Timestamp generation for each booking
- Input validation for various operations






