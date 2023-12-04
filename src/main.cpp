// C++ standard library includes
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// CAF includes
#include "caf/all.hpp"
#include "caf/io/all.hpp"

// Boost includes
CAF_PUSH_WARNINGS
#ifdef CAF_GCC
#  pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
CAF_POP_WARNINGS

// Own includes
#include "int512_serialization.hpp"
#include "is_probable_prime.hpp"
#include "types.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

using boost::multiprecision::gcd;
using boost::multiprecision::int512_t;

#include <mutex>
std::mutex cout_mutex;
std::mutex vector_mutex;
enum class ActorState { Running, Paused };
using namespace caf;
using namespace std::chrono;

namespace {

struct config : actor_system_config {
  string host = "localhost";
  uint16_t port = 0;
  size_t num_workers = 0;
  string mode;
  config() {
    opt_group{custom_options_, "global"}
      .add(host, "host,H", "server host (ignored in server mode)")
      .add(port, "port,p", "port")
      .add(num_workers, "num-workers,w", "number of workers (in worker mode)")
      .add(mode, "mode,m", "one of 'server', 'worker' or 'client'");
  }
};

// -- helper_funktion
// -------------------------------------------------------------------

bool isPrime(int n) {
  // Handle special cases
  if (n <= 1) {
    return false;
  }

  // Check for divisibility up to the square root of n
  for (int i = 2; i <= std::sqrt(n); ++i) {
    if (n % i == 0) {
      // Found a divisor, not a prime number
      return false;
    }
  }

  // No divisors found, it's a prime number
  return true;
}
void printMessage(const std::string& message) {
  // Lock the mutex before using std::cout
  std::lock_guard<std::mutex> lock(cout_mutex);

  // Print the message
  std::cout << message << std::endl;

  // The lock_guard is automatically released when it goes out of scope
}
int generate_Random_Nr(int max, int min) {
  // Seed the random number generator
  std::random_device rd;
  std::mt19937 generator(rd());

  // Define the distribution
  std::uniform_int_distribution<int> distribution(min, max);

  // Generate a random number
  return distribution(generator);
}
int pollard_rho(int n, int zufall) {
  // TODO: Implement me.
  // Set the duration to run the while loop
  const std::chrono::seconds duration(5);

  // Get the start time
  auto start_time = std::chrono::steady_clock::now();

  int x = generate_Random_Nr(n, zufall);
  cout << "generate_Random_Nr = " << x << std::endl;
  int y = x;
  int p = 1;

  while (p == 1 || p == n) {
    x = (x * x + 1) % n;
    y = (y * y + 1) % n;
    y = (y * y + 1) % n;
    p = std::gcd(std::abs(x - y), n);

    // Check if the loop has finished if not exit after duration
    if (std::chrono::steady_clock::now() - start_time >= duration) {
      // printMessage("Loop finished after 5 seconds.");
      return -1; // Exit the loop
    }
  }
  return p;
}

// -- SERVER -------------------------------------------------------------------

void run_server(actor_system& sys, const config& cfg) {
  cout << "server startet" << std::endl;
  if (auto port = sys.middleman().publish_local_groups(cfg.port))
    cout << "published local groups at port " << *port << '\n';
  else
    cerr << "error: " << caf::to_string(port.error()) << '\n';
  cout << "press any key to exit" << std::endl;
  getc(stdin);
}

// -- CLIENT -------------------------------------------------------------------

// Client state, keep track of factors, time, etc.
struct client_state {
  // The joined group.
  group grp;

  // The list of factors.
  std::vector<int> fact_list;

  // The list of tasks.
  std::vector<int> tasklist;
};

behavior client(stateful_actor<client_state>* self, caf::group grp) {
  // Join group and save it to send messages later.
  self->join(grp);
  self->state.grp = grp;
  self->state.fact_list = {};
  self->state.tasklist = {};

  // TODO: Implement me.

  int n = 37797063;
  self->state.tasklist.push_back(n);
  self->send(grp, "worker_atom_v", n, 3, n);
  cout << "behavior client started " << std::endl;

  /* auto start_time = high_resolution_clock::now();

  auto end_time = high_resolution_clock::now();
  auto elapsed_time = duration_cast<duration<double>>(end_time - start_time);
  aout(self) << "Elapsed time: " << elapsed_time.count() << " seconds" <<
  std::endl;*/

  return {// Handle messages
          [=](const std::string& message, int p, int ndurchP, int urN) {
            try {
              if (message == "client_atom_v") {
                auto foundtask = std::find(self->state.tasklist.begin(),
                                           self->state.tasklist.end(), urN);
                if (foundtask != self->state.tasklist.end()) {
                  self->state.tasklist.erase(foundtask);

                  cout << "the factorial of " << urN << " = " << p << " und  "
                       << ndurchP << std::endl;
                  if (isPrime(p)) {
                    cout << "++++++++++++++ Primzahl " << p
                         << " +++++++++++++++" << std::endl;
                    //<< std::endl;

                    self->state.fact_list.push_back(p);

                  } else {
                    self->state.tasklist.push_back(p);
                    self->send(grp, "worker_atom_v", p, 3, n);
                  }

                  if (isPrime(ndurchP)) {
                    cout << "++++++++++++++ Primzahl " << ndurchP
                         << " +++++++++++++++" << std::endl;
                    self->state.fact_list.push_back(ndurchP);

                  } else {
                    self->state.tasklist.push_back(ndurchP);
                    self->send(self->state.grp, "worker_atom_v", ndurchP, 3, n);
                  }
                }
              } else {
                // cout << "Task ist nicht in der Liste: " << urN << std::endl;
              }
              if (self->state.tasklist.empty()) {
                cout << "Liste ist leer" << std::endl;
                for (auto i : self->state.fact_list) {
                  cout << i << std::endl;
                }
              }
            } catch (const std::exception& e) {
              std::cerr << e.what() << '\n';
            }
          }};
  return {};
}

void run_client(actor_system& sys, const config& cfg) {
  cout << "starting client" << std::endl;
  if (auto eg = sys.middleman().remote_group("vslab", cfg.host, cfg.port)) {
    auto grp = *eg;
    sys.spawn(client, grp);
  } else {
    cerr << "error: " << caf::to_string(eg.error()) << '\n';
  }
}

// -- WORKER -------------------------------------------------------------------

// State specific to each worker.
struct worker_state {
  // The joined group.
  group grp;
};

behavior worker(stateful_actor<worker_state>* self, caf::group grp) {
  // Join group and save it to send messages later.
  self->join(grp);
  self->state.grp = grp;

  printMessage("behavior worker has been started");

  return {
    // Handle messages

    [=](const std::string& message, int n, int zufall, int z) {
      if (message == "worker_atom_v") {
        // calculate cpu time abd do pollard rho algorithm
        auto cpu_time_start = high_resolution_clock::now();

        int result = pollard_rho(n, zufall);

        auto cpu_time_end = high_resolution_clock::now();
        auto cpu_time_diff
          = duration_cast<duration<double>>(cpu_time_end - cpu_time_start);

        if (result != -1) {
          self->send(grp, "client_atom_v", result, n / result, n);
          std::string result_str
            = "Cpu time diff: " + std::to_string(cpu_time_diff.count())
              + " seconds" + "/n" + "Result: " + std::to_string(result)
              + "Worker sent result to client : ";
          printMessage(result_str);
        }
      }
    },
  };

  // TODO: Implement me.
  // - Calculate rho.
  // - Check for new messages in between.

  return {};
}

void run_worker(actor_system& sys, const config& cfg) {
  cout << "starting worker" << std::endl;
  if (auto eg = sys.middleman().remote_group("vslab", cfg.host, cfg.port)) {
    auto grp = *eg;
    size_t number_of_workers = cfg.num_workers;
    // TODO: Spawn workers, e.g:
    for (size_t i = 0; i < number_of_workers; i++) {
      sys.spawn(worker, grp);
    }
  } else {
    cerr << "error: " << caf::to_string(eg.error()) << '\n';
  }
  sys.await_all_actors_done();
}

// -- MAIN ---------------------------------------------------------------------

// dispatches to run_* function depending on selected mode
void caf_main(actor_system& sys, const config& cfg) {
  cout << "caf_main startet" << std::endl;
  // Check serialization implementation. You can delete this.
  auto check_roundtrip = [&](int512_t a) {
    byte_buffer buf;
    binary_serializer sink{sys, buf};
    assert(sink.apply(a));
    binary_deserializer source{sys, buf};
    int512_t a_copy;
    assert(source.apply(a_copy));
    assert(a == a_copy);
  };
  check_roundtrip(1234912948123);
  check_roundtrip(-124);

  int512_t n = 1;
  for (int512_t i = 2; i <= 50; ++i)
    n *= i;
  check_roundtrip(n);
  n *= -1;
  check_roundtrip(n);

  // Dispatch to function based on mode.
  using map_t = unordered_map<string, void (*)(actor_system&, const config&)>;
  map_t modes{
    {"server", run_server},
    {"worker", run_worker},
    {"client", run_client},
  };
  auto i = modes.find(cfg.mode);
  if (i != modes.end())
    (i->second)(sys, cfg);
  else
    cerr << "*** invalid mode specified" << endl;
}

} // namespace

CAF_MAIN(io::middleman, id_block::vslab)

/*
  auto sender_ptr  = self->current_sender();
  auto sender = actor_cast<actor>(sender_ptr);
  actor_id sender_id = sender->id();
  std::cout << "client self id = " << self->id() << " received message from
  worker: " << message << "sender id = "<< sender_id << std::endl;
*/
