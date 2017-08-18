// Compatibility code require to work with MK v0.3.x
#ifndef COMPAT_03_HPP
#define COMPAT_03_HPP



// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
//#ifndef MEASUREMENT_KIT_COMMON_RUNNER_HPP
//#define MEASUREMENT_KIT_COMMON_RUNNER_HPP

#include <measurement_kit/common.hpp>
#include <measurement_kit/nettests.hpp>

#include <atomic>
#include <string>
#include <thread>

namespace mk {

//class nettests::BaseTest;

class RunnerNg {
  public:
    RunnerNg();
    void run_test(Var<nettests::BaseTest> test, Callback<Var<nettests::BaseTest>> func);
    void run(Callback<Continuation<>> begin);
    void break_loop_();
    bool empty();
    void join_();
    ~RunnerNg();
    static Var<RunnerNg> global();

    // Globally accessible attribute that other classes can use
    Var<Reactor> reactor = Reactor::global();

  private:
    std::atomic<int> active{0};
    std::mutex run_mutex;
    std::atomic<bool> running{false};
    std::thread thread;

    void run_unlocked_(Callback<Continuation<>> begin);
};

} // namespace mk
//#endif



// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
//#ifndef MEASUREMENT_KIT_OONI_SCRIPTABLE_HPP
//#define MEASUREMENT_KIT_OONI_SCRIPTABLE_HPP

// This module wraps OONI's tests offering several convenience
// functionality, e.g., running them in a background thread.

#include <measurement_kit/dns.hpp>
#include <measurement_kit/report.hpp>

namespace mk {
namespace ooni {
namespace scriptable {

/*
    Async functions. The following functions run the requested operation
    in the background thread managed by the RunnerNg instance. The Entry is
    returned serialized as a string, again to help scriptability.
*/

void dns_injection(std::string input, Settings settings,
                   Callback<std::string> callback,
                   Var<RunnerNg> runner = RunnerNg::global(),
                   Var<Logger> = Logger::global());

void http_invalid_request_line(Settings settings, Callback<std::string> cb,
                               Var<RunnerNg> runner = RunnerNg::global(),
                               Var<Logger> logger = Logger::global());

void tcp_connect(std::string input, Settings settings,
                 Callback<std::string> callback,
                 Var<RunnerNg> runner = RunnerNg::global(),
                 Var<Logger> logger = Logger::global());

void web_connectivity(std::string input, Settings settings,
                      Callback<std::string> callback,
                      Var<RunnerNg> runner = RunnerNg::global(),
                      Var<Logger> logger = Logger::global());

} // namespace scriptable
} // namespace mk
} // namespace ooni
//#endif



// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
//#ifndef MEASUREMENT_KIT_NDT_SCRIPTABLE_HPP
//#define MEASUREMENT_KIT_NDT_SCRIPTABLE_HPP

#include <measurement_kit/common.hpp>

namespace mk {
namespace ndt {
namespace scriptable {

void run(Callback<std::string> callback, Settings settings = {},
         Var<RunnerNg> runner = RunnerNg::global(),
         Var<Logger> logger = Logger::global());

} // namespace scriptable
} // namespace ndt
} // namespace mk
//#endif



#endif
