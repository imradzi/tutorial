#include <iostream>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>
#include "ulid.h"

void analyzeULIDUniqueness() {
    std::cout << "=== ULID Uniqueness Analysis ===" << std::endl;
    
    // Generate ULIDs rapidly to test uniqueness
    const int TEST_COUNT = 1000000;  // 1 million ULIDs
    std::unordered_set<std::string> unique_ulids;
    std::vector<std::string> all_ulids;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate ULIDs as fast as possible
    for (int i = 0; i < TEST_COUNT; ++i) {
        std::string ulid = generateULID();
        all_ulids.push_back(ulid);
        unique_ulids.insert(ulid);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Generated " << TEST_COUNT << " ULIDs in " << duration.count() << "ms" << std::endl;
    std::cout << "Unique ULIDs: " << unique_ulids.size() << std::endl;
    std::cout << "Duplicates: " << (TEST_COUNT - unique_ulids.size()) << std::endl;
    std::cout << "Uniqueness rate: " << std::fixed << std::setprecision(6) 
              << (100.0 * unique_ulids.size() / TEST_COUNT) << "%" << std::endl;
    
    // Check if ULIDs are lexicographically sorted
    bool is_sorted = true;
    for (size_t i = 1; i < all_ulids.size(); ++i) {
        if (all_ulids[i-1] > all_ulids[i]) {
            is_sorted = false;
            break;
        }
    }
    
    std::cout << "Lexicographically sorted: " << (is_sorted ? "YES" : "NO") << std::endl;
    
    // Show some sample ULIDs
    std::cout << "\nSample ULIDs:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << all_ulids[i] << std::endl;
    }
}

void showCollisionProbabilities() {
    std::cout << "\n=== Collision Probability Analysis ===" << std::endl;
    
    // ULID has 80 bits of randomness
    const double total_random_space = std::pow(2.0, 80);
    
    std::cout << "Total randomness space: " << std::scientific << total_random_space << std::endl;
    std::cout << "That's approximately: " << std::fixed << std::setprecision(0) 
              << total_random_space << " possible values" << std::endl;
    
    // Birthday paradox calculations for different numbers of ULIDs
    std::vector<long long> test_sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000};
    
    std::cout << "\nCollision probabilities (Birthday Paradox):" << std::endl;
    std::cout << "ULIDs Generated | Collision Probability" << std::endl;
    std::cout << "----------------|----------------------" << std::endl;
    
    for (auto n : test_sizes) {
        // Simplified birthday paradox: P(collision) ≈ 1 - e^(-n²/(2*N))
        // where N is the total space (2^80) and n is number of items
        double exponent = -static_cast<double>(n * n) / (2.0 * total_random_space);
        double collision_prob = 1.0 - std::exp(exponent);
        
        std::cout << std::setw(15) << n << " | " 
                  << std::scientific << std::setprecision(2) << collision_prob;
        
        if (collision_prob < 1e-10) {
            std::cout << " (virtually zero)";
        }
        std::cout << std::endl;
    }
}

void simulateMultipleMachines() {
    std::cout << "\n=== Multi-Machine Simulation ===" << std::endl;
    
    // Simulate 3 different machines generating ULIDs simultaneously
    const int ULIDS_PER_MACHINE = 10000;
    const int MACHINE_COUNT = 3;
    
    std::unordered_set<std::string> global_ulids;
    std::vector<std::vector<std::string>> machine_ulids(MACHINE_COUNT);
    
    // Simulate each machine with different random seeds
    std::vector<ULIDGenerator> generators(MACHINE_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate ULIDs on each "machine"
    for (int machine = 0; machine < MACHINE_COUNT; ++machine) {
        std::cout << "Machine " << (machine + 1) << " generating " << ULIDS_PER_MACHINE << " ULIDs..." << std::endl;
        
        for (int i = 0; i < ULIDS_PER_MACHINE; ++i) {
            std::string ulid = generators[machine].generate();
            machine_ulids[machine].push_back(ulid);
            global_ulids.insert(ulid);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    int total_generated = MACHINE_COUNT * ULIDS_PER_MACHINE;
    int unique_count = global_ulids.size();
    
    std::cout << "\nMulti-machine results:" << std::endl;
    std::cout << "Total ULIDs generated: " << total_generated << std::endl;
    std::cout << "Unique ULIDs across all machines: " << unique_count << std::endl;
    std::cout << "Cross-machine duplicates: " << (total_generated - unique_count) << std::endl;
    std::cout << "Generation time: " << duration.count() << "ms" << std::endl;
    
    if (unique_count == total_generated) {
        std::cout << "✅ Perfect uniqueness across all machines!" << std::endl;
    } else {
        std::cout << "⚠️  Found " << (total_generated - unique_count) << " duplicates" << std::endl;
    }
    
    // Show sample ULIDs from each machine
    std::cout << "\nSample ULIDs from each machine:" << std::endl;
    for (int machine = 0; machine < MACHINE_COUNT; ++machine) {
        std::cout << "Machine " << (machine + 1) << ": " << machine_ulids[machine][0] << std::endl;
    }
}

int main() {
    std::cout << "ULID Uniqueness Analysis" << std::endl;
    std::cout << "========================" << std::endl;
    
    analyzeULIDUniqueness();
    showCollisionProbabilities();
    simulateMultipleMachines();
    
    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "• ULIDs have 80 bits of randomness (1.2 x 10^24 possibilities)" << std::endl;
    std::cout << "• Collision probability is extremely low even with millions of ULIDs" << std::endl;
    std::cout << "• Timestamp component (48 bits) provides natural ordering" << std::endl;
    std::cout << "• Each machine uses different random sequences" << std::endl;
    std::cout << "• Clock synchronization improves but isn't required for uniqueness" << std::endl;
    
    return 0;
}