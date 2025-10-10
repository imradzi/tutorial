# CSV Reader Programs

This directory contains C++ programs to read Malaysian postcode and state data from CSV files into arrays/vectors.

## Files

### CSV Data Files
- `postcodes.csv` - Contains Malaysian postal codes with format: "postcode","area","city","state_code"
- `states.csv` - Contains Malaysian states with format: "state_code","state_name"

### C++ Source Files

1. **`csv_reader_simple.cpp`** - Uses `std::vector` containers (recommended)
   - Modern C++ approach using STL containers
   - Automatic memory management
   - Dynamic sizing

2. **`csv_reader_arrays.cpp`** - Uses traditional C-style arrays
   - Uses `new`/`delete` for dynamic allocation
   - Manual memory management
   - Fixed maximum size (60,000 postcodes, 20 states)

3. **`csv_data.h`** - Header file with data structures and function declarations

## Building

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build specific programs
make csv_reader_simple    # Vector-based version
make csv_reader_arrays    # Array-based version
```

## Running

```bash
# Run the vector-based version (recommended)
./build/csv_reader_simple

# Run the array-based version
./build/csv_reader_arrays
```

## Program Features

Both programs provide:
- Loading CSV data into memory structures
- Display first 5 postcode entries
- Display all Malaysian states
- Sample search for Perlis (PLS) state
- Interactive search by state code

### Interactive Usage
When prompted, enter a state code (e.g., `PLS`, `JHR`, `KUL`) to search for postcodes in that state.

### Available State Codes
- `JHR` - Johor
- `KDH` - Kedah  
- `KTN` - Kelantan
- `KUL` - Kuala Lumpur
- `LBN` - Labuan
- `MLK` - Melaka
- `NSN` - Negeri Sembilan
- `PHG` - Pahang
- `PJY` - Putrajaya
- `PLS` - Perlis
- `PNG` - Pulau Pinang
- `PRK` - Perak
- `SBH` - Sabah
- `SGR` - Selangor
- `SRW` - Sarawak
- `TRG` - Terengganu

## Data Statistics
- **Total Postcodes**: 56,234 entries
- **Total States**: 16 entries

## Key Differences Between Versions

### Vector Version (`csv_reader_simple.cpp`)
```cpp
std::vector<PostcodeEntry> postcodes;
std::vector<StateEntry> states;
// Automatic memory management
// Dynamic sizing
```

### Array Version (`csv_reader_arrays.cpp`) 
```cpp
PostcodeEntry* postcodes = new PostcodeEntry[MAX_POSTCODES];
StateEntry* states = new StateEntry[MAX_STATES];
// Manual memory management
// Fixed maximum size
// Requires delete[] cleanup
```

## Example Output
```
Successfully loaded 56234 postcode entries.
Successfully loaded 16 state entries.

=== Data loaded successfully ===
Total postcodes: 56234
Total states: 16

=== First 5 Postcode Entries ===
1. Postcode: 01000, Area: Alor Redeh, City: Kangar, State: PLS
2. Postcode: 01000, Area: Alor Tok Terang, City: Kangar, State: PLS
...
```