#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

/// Physics

// Define an enumerator of energy type to have better semantics in storing the user choice.
enum class EnergyType {
    ELECTRON_VOLT,
    JOULES
};

// Rydberg constant in electron-volts.
const double RYDBERG_CONSTANT = 13.60569300984;

// Electron-volt to joules constant.
const double EV_TO_JOULES = 1.6e-19;

// Caclulates the energy of an electron transition defined by the values Z, nInitial and nFinal using the Bohr model.
double calculateBohrEnergy(unsigned int Z, unsigned int nInitial, unsigned int nFinal, EnergyType eType) {
    double energy = RYDBERG_CONSTANT * std::pow((double)Z, 2.0) * (1.0 / std::pow((double)nFinal, 2.0) - 1.0 / std::pow((double)nInitial, 2.0));
    if (eType == EnergyType::JOULES) {
        energy *= EV_TO_JOULES;
    }
    return energy;
}

/// Input Handling

// Gets an integer from the user within the defined range.
int getint(int min = INT_MIN, int max = INT_MAX) {
    // By making this type the next size up from standard integers, can avoid edge cases where values in
    // possibleInteger seem reasonable even though no value was necessarily obtained from the stringstream.
    long long possibleInteger;
    std::string line;
    // Grab a line from the cin buffer.
    while (std::getline(std::cin, line)) {
        possibleInteger = std::numeric_limits<long long>::min();

        // Move string into a stringstream.
        std::stringstream input(line);

        // Grab as much integer as possible from the front of the stringstream's buffer.        
        input >> possibleInteger;
        // Apply range bounds checks to resulting maybe-integer.
        if (possibleInteger >= (long long)min && possibleInteger <= (long long)max) {
            // Make sure the stringstream's buffer is clear; "123abc" does not an integer make.
            std::string emptyTest;
            input >> emptyTest;
            if (emptyTest.empty()) {
                // All's good, break out of while loop and return the integer.
                break;
            }
        }

        std::cout << "Sorry, the value you inputted was not valid." << std::endl;
        std::cout << "Input an integer between " << min << " and " << max << ":\n";
    }
    // Can only get here via the if block, but returning inside the if block makes VS throw a warning, I don't like warnings.
    return possibleInteger;
}

// Compares two strings in a case-insensitive manner.
bool icompare(const std::string& a, const std::string& b) {
    // Simple shortcut, two strings must be the same length to be the same.
    if (a.length() != b.length()) return false;
    
    // Not-so-efficient algorithm that equates each pair of characters with the same index in the string objects' char arrays one at a time.
    return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) {
        // Case insensitive so simply force characters to their lower-case form.
        return std::tolower(a) == std::tolower(b);
    });
}

// Gets a user choice between two sets of valid responses. Returns 1 where the user response was in the first set and -1 where the response was in the second set.
int getBetweenTwoStringSetOptions(std::string message, const std::vector<std::string>& firstStringSet, const std::vector<std::string>& secondStringSet) {
    int response;
    std::string line;
    // Grab a line from the cin buffer.
    while (std::getline(std::cin, line)) {
        // Iterate over valid true strings and compare them with the grabbed line in a case-insensitive manner.
        const auto& itTrue = std::find_if(firstStringSet.begin(), firstStringSet.end(), [&line](const std::string& a) {
            return icompare(a, line);
        });
        // Iterate over valid false strings and compare them with the grabbed line in a case-insensitive manner.
        const auto& itFalse = std::find_if(secondStringSet.begin(), secondStringSet.end(), [&line](const std::string& a) {
            return icompare(a, line);
        });

        // If either iterator is not pointing at the end of their respective string arrays, then the grabbed line must have matched one of the valid strings in one of those arrays.
        if (itTrue != firstStringSet.end()) {
            response = 1;
            break;
        } else if (itFalse != secondStringSet.end()) {
            response = -1;
            break;
        }

        std::cout << "Sorry, the value you inputted was not valid." << std::endl;
        std::cout << message;
    }
    // Can only get here via the if block, but returning inside the if block makes VS throw a warning, I don't like warnings.
    return response;
}

// Gets an energy type preference from the user.
EnergyType getEnergyType() {
    static const std::vector<std::string> validEV = {
        "e",
        "ev",
        "electron volt",
        "electronvolt",
        "electron-volt",
        "electron volts",
        "electronvolts",
        "electron-volts"
    };
    static const std::vector<std::string> validJoules = {
        "j",
        "joule",
        "joules"
    };

    // getBetweenTwoStringSetOptions returns 1 where the user response was in the first set, and -1 where the user response was in the second set.
    if (getBetweenTwoStringSetOptions("Electron-volts or joules? ['e', 'J']:\n", validEV, validJoules) == 1) {
        return EnergyType::ELECTRON_VOLT;
    } else {
        return EnergyType::JOULES;
    }
}

// Gets a bool from the user.
bool getBool() {
    static const std::vector<std::string> validTrues = {
        "yes",
        "y",
        "true",
        "1"
    };
    static const std::vector<std::string> validFalses = {
        "no",
        "n",
        "false",
        "0"
    };

    return getBetweenTwoStringSetOptions("Yay, or nay? [y/n]:\n", validTrues, validFalses) == 1;
}

/// Main Program

int main(int argc, char* argv[]) {
    std::cout << "Welcome to Matt's glorious electron transition energy calculator!\n";

    bool shouldContinue;
    do {
        shouldContinue = true;

        // Acquire system definition.
        std::cout << "\nPlease specify a value for the atomic number of the system under consideration.\n";
        unsigned int Z = (unsigned int)getint(1);
        std::cout << "\nPlease specify a value for the initial principal quantum number of the electron under consideration.\n";
        unsigned int nInitial = (unsigned int)getint(1);
        std::cout << "\nPlease specify a value for the final principal quantum number of the electron under consideration.\n";
        unsigned int nFinal = (unsigned int)getint(1);

        // If initial principal quantum number is less than the final, then let's start again.
        if (nInitial < nFinal) {
            std::cout << "\nThe initial principal quantum number must be greater than the final principal quantum number!\n";
            std::cout << "Let's start again!\n";
            continue;
        }

        // Acquire energy unit preference.
        std::cout << "\nDo you want the results in electron-volts or joules?\n";
        EnergyType eType = getEnergyType();

        // Caclulate energy.
        double energy = calculateBohrEnergy(Z, nInitial, nFinal, eType);

        // Format energy calculation output for user's pleasure.
        std::cout << "\nFor a (" << Z << ", " << nInitial << ", " << nFinal << ") transition the energy was calculated to be: \n";
        std::cout << "    E = " << std::setprecision(3) << energy;
        if (eType == EnergyType::ELECTRON_VOLT) {
            std::cout << "eV\n";
        } else {
            std::cout << "J\n";
        }

        std::cout << "\nDo you wish to continue? [y/n]:\n";
        shouldContinue = getBool();
    } while (shouldContinue);

    return 0;
}
