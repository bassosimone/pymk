// Compatibility code require to work with MK v0.3.x
#include "compat-0.3.hpp"



// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include <measurement_kit/nettests.hpp>
#include <measurement_kit/ooni.hpp>
#include <measurement_kit/report.hpp>

namespace mk {
namespace ooni {
namespace scriptable {

using namespace mk::report;

// Convenience macro used to implement all callbacks below
#define XX                                                                     \
    [=](Var<Entry> entry) {                                                    \
        complete([=]() {                                                       \
            if (!entry) {                                                      \
                callback("{}");                                                \
                return;                                                        \
            }                                                                  \
            callback(entry->dump(4));                                          \
        });                                                                    \
    }

void dns_injection(std::string input, Settings settings,
                   Callback<std::string> callback, Var<RunnerNg> runner,
                   Var<Logger> logger) {
    runner->run([=](Continuation<> complete) {
        ooni::dns_injection(input, settings, XX, runner->reactor, logger);
    });
}

void http_invalid_request_line(
        Settings settings, Callback<std::string> callback,
        Var<RunnerNg> runner, Var<Logger> logger) {
    runner->run([=](Continuation<> complete) {
        ooni::http_invalid_request_line(settings, XX, runner->reactor, logger);
    });
}

void tcp_connect(std::string input, Settings settings,
                 Callback<std::string> callback,
                 Var<RunnerNg> runner, Var<Logger> logger) {
    runner->run([=](Continuation<> complete) {
        ooni::tcp_connect(input, settings, XX, runner->reactor, logger);
    });
}

void web_connectivity(std::string input, Settings settings,
                      Callback<std::string> callback,
                      Var<RunnerNg> runner,
                      Var<Logger> logger) {
    runner->run([=](Continuation<> complete) {
        ooni::web_connectivity(input, settings, XX, runner->reactor, logger);
    });
}

void meek_fronted_requests(std::string input, Settings settings,
                      Callback<std::string> callback,
                      Var<RunnerNg> runner,
                      Var<Logger> logger) {
    runner->run([=](Continuation<> complete) {
        ooni::meek_fronted_requests(input, settings, XX, runner->reactor, logger);
    });
}

void dns_query(std::string input, Callback<std::string> callback,
               Var<RunnerNg> runner, Var<Logger> logger) {
    Var<Entry> entry(new Entry);
    Settings settings;
    runner->run([=](Continuation<> complete) {
        ooni::templates::dns_query(entry, "A", "IN", "google.com", "8.8.8.8",
            [=](Error err, Var<dns::Message> message) {
                complete([=]() {
                    if (!!err) {
                        callback("{}");
                        return;
                    }
                    callback(entry->dump(4));
                });
        }, settings, runner->reactor, logger);
    });
}

void http_request(std::string input, Callback<std::string> callback,
                  Var<RunnerNg> runner, Var<Logger> logger) {
    Var<Entry> entry(new Entry);
    Settings settings;
    settings["http/url"] = input;
    http::Headers headers;
    std::string body = "";
    runner->run([=](Continuation<> complete) {
        ooni::templates::http_request(entry, settings, headers, body,
            [=](Error err, Var<http::Response> response){
                complete([=]() {
                    if (!!err) {
                        callback("{}");
                        return;
                    }
                    callback(entry->dump(4));
                });
        }, runner->reactor, logger);
    });
}

// similar to above tcp_connect(), but this one lets you send
// a payload and receive a response
void tcp_connect2(std::string host_port, std::string payload,
		  Callback<std::string> callback,
		  Var<RunnerNg> runner, Var<Logger> logger) {
    Var<Entry> entry(new Entry);
    Settings options;
    (*entry)["connection"] = nullptr;

    ErrorOr<net::Endpoint> maybe_epnt = net::parse_endpoint(host_port, 80);
    if (!maybe_epnt) {
        (*entry)["connection"] = maybe_epnt.as_error().reason;
        callback(entry->dump(4));
        return;
    }
    options["host"] = maybe_epnt->hostname;
    options["port"] = maybe_epnt->port;
    logger->debug("tcp_connect2: parsed host:%s, port:%s", options["host"].c_str(),
		options["port"].c_str());
    templates::tcp_connect(options, [=](Error err, Var<net::Transport> txp) {
        logger->debug("tcp_connect2: connected to endpoint");
        if (!!err) {
            (*entry)["connection"] = err.reason;
            callback(entry->dump(4));
            return;
        }
        Var<std::string> received_data(new std::string);
        txp->on_data([=](net::Buffer data) {
            logger->debug("tcp_connect2: on_data: %s",
                          data.peek().c_str());
            *received_data += data.read();
        });
        txp->write(payload);

        // We assume to have received all the data after a timeout
        // of 5 seconds. XXX hardcoded
        runner->reactor->call_later(5, [=]() {
            (*entry)["connection"] = "success";
            (*entry)["sent"] = payload;
            (*entry)["received"] = *received_data;
            txp->close([=]() { callback(entry->dump(4)); });
	});
    }, runner->reactor, logger);

}

} // namespace scriptable
} // namespace mk
} // namespace ooni




// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include <measurement_kit/ndt.hpp>

namespace mk {
namespace ndt {
namespace scriptable {

using namespace mk::report;

void run(Callback<std::string> callback, Settings settings,
         Var<RunnerNg> runner, Var<Logger> logger) {
    Var<Entry> entry(new Entry);
    ndt::run(entry, [=](Error error) {
        if (error) {
            callback("{}");
            return;
        }
        callback(entry->dump(4));
    }, settings, runner->reactor, logger);
}

} // namespace scriptable
} // namespace mk
} // namespace ndt



// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include <measurement_kit/common.hpp>

#include <cassert>
#include <future>

namespace mk {

RunnerNg::RunnerNg() {}

void RunnerNg::run(Callback<Continuation<>> kickoff) {
    // Lock mutex to make sure that a single thread at a time can call
    // us, so to avoid issues with concurrent invocations.
    std::lock_guard<std::mutex> lock(run_mutex);
    run_unlocked_(kickoff);
}

void RunnerNg::run_unlocked_(Callback<Continuation<>> kickoff) {
    assert(active >= 0);
    if (active == 0 and running) {
        break_loop_();  // Just in case but in theory not needed
        join_();        // Should set `running = false`
        assert(not running);
    }
    if (not running) {
        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();
        // WARNING: below we're passing `this` to the thread, which means that
        // the destructor MUST wait the thread. Otherwise, when the thread dies
        // many strange things could happen (I have seen SIGABRT).
        debug("runner: starting reactor in background...");
        thread = std::thread([&promise, this]() {
            reactor->loop_with_initial_event([&promise]() {
                debug("runner: starting reactor in background... ok");
                promise.set_value(true);
            });
        });
        future.wait();
        running = true;
    }
    active += 1;
    int task_id = active;
    debug("runner: scheduling %d", task_id);
    reactor->call_soon([=]() {
        debug("runner: starting %d", task_id);
        kickoff([=](Callback<> end) {
            debug("runner: ending %d", task_id);
            // For robustness, delay the final callback to the beginning of
            // next I/O cycle to prevent possible user after frees. This
            // could happen because, in our current position on the stack,
            // we have been called by `NetTest` code that may use `this`
            // after calling the callback. But this would be a problem
            // because `test` is most likely to be destroyed after `fn()`
            // returns. This, when unwinding the stack, the use after free
            // would happen.
            reactor->call_soon([=]() {
                debug("runner: callbacking %d", task_id);
                active -= 1;
                assert(active >= 0);
                if (active == 0) {
                    // Interrupt the event loop. The thread will be joined
                    // by the destructor or by next `run` invocation.
                    break_loop_();
                }
                end();
            });
        });
    });
}

void RunnerNg::run_test(Var<nettests::BaseTest> test, Callback<Var<nettests::BaseTest>> fn) {
    run([=](Continuation<> complete) {
        test->run();
//        test->begin([=](Error) {
//            // TODO: do not ignore the error
//            test->end([=](Error) {
//                // TODO: do not ignore the error
//                complete([=]() {
//                    fn(test);
//                });
//            });
//        });
    });
}

void RunnerNg::break_loop_() { reactor->break_loop(); }

bool RunnerNg::empty() { return active == 0; }

void RunnerNg::join_() {
    if (running) {
        thread.join();
        running = false;
    }
}

RunnerNg::~RunnerNg() {
    // WARNING: This MUST be here to make sure we break the loop before we
    // stop the thread. Not doing that leads to undefined behavior.
    break_loop_();
    join_();
}

/*static*/ Var<RunnerNg> RunnerNg::global() {
    static Var<RunnerNg> singleton(new RunnerNg);
    return singleton;
}

} // namespace mk
