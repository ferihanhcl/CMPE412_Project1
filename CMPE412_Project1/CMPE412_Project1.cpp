#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <iomanip>

using namespace std;

/// Pozisyonları enum olarak tanımla
enum Position {
    Quaestor,
    Aedile,
    Praetor,
    Consul
};

/// Politician yapısı tanımı
struct Politician {
    int age;
    int service_years;
    Position position;
};

const int NUM_YEARS = 200;                      /// Simulation should be run for 200 years

/// Positions Available Annually
const int NUM_QUAESTORS = 20;
const int NUM_AEDILES = 10;
const int NUM_PRAETORS = 8;
const int NUM_CONSULS = 2;

const int MIN_QUAESTOR_AGE = 30;                /// Minimum age 30
const int MIN_AEDILE_AGE = 36;                  /// Minimum age 36   
const int MIN_PRAETOR_AGE = 39;                 /// Minimum age 39
const int MIN_CONSUL_AGE = 42;                  /// Minimum age 42

const int MIN_SERVICE_QUAESTOR_AEDILE = 2;      /// with at least 2 years of service as Quaestor required
const int MIN_SERVICE_AEDILE_PRAETOR = 2;       /// with at least 2 years of service as Aedile required
const int MIN_SERVICE_PRAETOR_CONSUL = 2;       /// with at least 2 years of service as Praetor required
const int REELECTION_INTERVAL = 10;             /// a 10-year interval required for re-election attempts

const int INITIAL_PSI = 100;                    /// Starting PSI

const int UNFILLED_POSITION_PENALTY = -5;       /// Penalty for each unfilled position
const int REELECTION_PENALTY = -10;             /// Additional penalty for re-electing a Consul within 10 years

/// Life Expectancy Distribution:
const int LIFE_EXPECTANCY_MIN = 25;             /// A
const int LIFE_EXPECTANCY_MAX = 80;             /// B
const double MU = 55.0;                         /// μ
const double SIGMA = 10.0;                      /// σ

const double NEW_CANDIDATES_MU = 15.0;          /// μ
const double NEW_CANDIDATES_SIGMA = 5.0;        /// σ


/****************

Generates a random age within a specified age range based on a normal distribution.
    min: Lower bound of the generated age
    max: Upper bound of the generated age
    mu: Mean of the normal distribution
    sigma: Standard deviation of the normal distribution

****************/

int generateLifeExpectancy(int min, int max, double mu, double sigma) {
    random_device rd; /// Create a random device
    mt19937 gen(rd()); /// Create a Mersenne twister random number generator

    /// Create a normal distribution object with the specified mean and standard deviation
    normal_distribution<double> dist(mu, sigma);

    double value;
    /// Loop until the generated age is within the min and max range
    do {
        /// Generate a random age based on the normal distribution
        value = round(dist(gen));
    } while (value < min || value > max); /// Repeat the loop as long as the generated age is outside the min and max range

    /// Return the integer value of the generated age
    return static_cast<int>(value);
}


/// Updates the PSI based on office fill rates and reelection penalties.
int calcPSI(int psi, vector<Politician>& politicians, int num_consuls) {

    int unfilled_penalty = num_consuls * UNFILLED_POSITION_PENALTY;
    int reelection_penalty = 0;

    for (int i = 0; i < politicians.size(); ++i) {
        if (politicians[i].position == Position::Consul && politicians[i].service_years < REELECTION_INTERVAL) {
            reelection_penalty++;
        }
    }

    psi += unfilled_penalty + reelection_penalty * REELECTION_PENALTY;

    /// Return the updated PSI value
    return psi;
}


/// Function to simulate a yearly cycle of the political system.
void simulateYearlyCycle(int& psi, vector<Politician>& politicians) {

    /// Incoming new candidates
    random_device rd;
    mt19937 gen(rd());

    normal_distribution<double> distribution(NEW_CANDIDATES_MU, NEW_CANDIDATES_SIGMA);
    int num_new_candidates = round(distribution(gen));

    for (int i = 0; i < num_new_candidates; ++i) {
        politicians.push_back({ MIN_QUAESTOR_AGE, 0, Position::Quaestor });
    }

    /// Aging and life expectancy control
    for (int i = 0; i < politicians.size();) {
        politicians[i].age++;
        int age = generateLifeExpectancy(LIFE_EXPECTANCY_MIN, LIFE_EXPECTANCY_MAX, MU, SIGMA);
        if (politicians[i].age > age) {
            politicians.erase(politicians.begin() + i);
        }
        else {
            ++i;
        }
    }


    /// Checking the number of current Consuls
    int num_consuls = 0;
    for (int i = 0; i < politicians.size(); ++i) {
        if (politicians[i].position == Position::Consul) {
            num_consuls++;
        }
    }

    for (int i = num_consuls; i < NUM_CONSULS; ++i) {
        politicians.push_back({ MIN_CONSUL_AGE, 0, Position::Consul });
    }

    /// Checking the number of current Praetors
    int num_praetors = 0;
    for (int i = 0; i < politicians.size(); ++i) {
        if (politicians[i].position == Position::Praetor) {
            num_praetors++;
        }
    }

    for (int i = num_praetors; i < NUM_PRAETORS; ++i) {
        politicians.push_back({ MIN_PRAETOR_AGE, 0, Position::Praetor });
    }

    /// Checking the number of current Aediles
    int num_aediles = 0;
    for (int i = 0; i < politicians.size(); ++i) {
        if (politicians[i].position == Position::Aedile) {
            num_aediles++;
        }
    }

    for (int i = num_aediles; i < NUM_AEDILES; ++i) {
        politicians.push_back({ MIN_AEDILE_AGE, 0, Position::Aedile });
    }

    /// Checking the number of current Quaestors
    int num_quaestors = 0;
    for (int i = 0; i < politicians.size(); ++i) {
        if (politicians[i].position == Position::Quaestor) {
            num_quaestors++;
        }
    }

    for (int i = num_quaestors; i < NUM_QUAESTORS; ++i) {
        politicians.push_back({ MIN_QUAESTOR_AGE, 0, Position::Quaestor });
    }

    /// Calculate PSI based on unfilled positions and reelection penalties
    psi = calcPSI(psi, politicians, num_consuls);
}


/// Function to count the age distribution of politicians in an office
void countAgeDistribution(const vector<int>& politicians, map<int, int>& age_distribution) {
    for (int age : politicians) {
        age_distribution[age]++;
    }
}

int main() {

    /// Initialize random number generator
    srand(time(NULL));

    int psi = INITIAL_PSI;
    vector<Politician> politicians;

    /// Run the simulation for 200 years
    for (int year = 1; year <= NUM_YEARS; ++year) {
        simulateYearlyCycle(psi, politicians);
    }

    cout << "PSI: " << psi << endl << endl;

    cout << "Annual Fill Rate:" << endl;

    int NUM_QUAESTORS = 0, NUM_AEDILES = 0, NUM_PRAETORS = 0, NUM_CONSULS = 0;

    for (int i = 0; i < politicians.size(); ++i) {
        const Politician& politician = politicians[i];
        if (politician.position == Position::Quaestor) {
            NUM_QUAESTORS++;
        }
        else if (politician.position == Position::Aedile) {
            NUM_AEDILES++;
        }
        else if (politician.position == Position::Praetor) {
            NUM_PRAETORS++;
        }
        else if (politician.position == Position::Consul) {
            NUM_CONSULS++;
        }
    }

    double SIZE = politicians.size();
    cout << "Quaestor: " << setprecision(5) << (NUM_QUAESTORS / SIZE) * 100.0 << " %" << endl;
    cout << "Aedile: " << setprecision(5) << (NUM_AEDILES / SIZE) * 100.0 << " %" << endl;
    cout << "Praetor: " << setprecision(5) << (NUM_PRAETORS / SIZE) * 100.0 << " %" << endl;
    cout << "Consul: " << setprecision(5) << (NUM_CONSULS / SIZE) * 100.0 << " %" << endl << endl;


    cout << "Age Distribution:" << endl;

    /// Calculate age distribution for each office
    map<int, int> age_distribution_quaestor;
    map<int, int> age_distribution_aedile;
    map<int, int> age_distribution_praetor;
    map<int, int> age_distribution_consul;

    vector<int> ages_quaestor, ages_aedile, ages_praetor, ages_consul;

    for (int i = 0; i < politicians.size(); ++i) {
        const Politician& politician = politicians[i];
        if (politician.position == Position::Quaestor) {
            ages_quaestor.push_back(politician.age);
        }
        else if (politician.position == Position::Aedile) {
            ages_aedile.push_back(politician.age);
        }
        else if (politician.position == Position::Praetor) {
            ages_praetor.push_back(politician.age);
        }
        else if (politician.position == Position::Consul) {
            ages_consul.push_back(politician.age);
        }
    }

    /// Count age distribution for each office
    countAgeDistribution(ages_quaestor, age_distribution_quaestor);
    countAgeDistribution(ages_aedile, age_distribution_aedile);
    countAgeDistribution(ages_praetor, age_distribution_praetor);
    countAgeDistribution(ages_consul, age_distribution_consul);

    /// Output age distribution for each office
    cout << "Quaestor:" << endl;
    for (auto& entry : age_distribution_quaestor) {
        cout << entry.first << " years old, " << entry.second << " people" << endl;
    }
    cout << endl;

    cout << "Aedile:" << endl;
    for (auto& entry : age_distribution_aedile) {
        cout << entry.first << " years old, " << entry.second << " people" << endl;
    }
    cout << endl;

    cout << "Praetor:" << endl;
    for (auto& entry : age_distribution_praetor) {
        cout << entry.first << " years old, " << entry.second << " people" << endl;
    }
    cout << endl;

    cout << "Consul:" << endl;
    for (auto& entry : age_distribution_consul) {
        cout << entry.first << " years old, " << entry.second << " people" << endl;
    }
    cout << endl;

    return 0;
}
