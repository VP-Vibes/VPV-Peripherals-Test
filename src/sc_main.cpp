/*
 * sc_main.cpp
 *
 *  Created on:
 *      Author:
 */

#include "factory.h"
#include <catch2/catch_session.hpp>
#include <scc/report.h>
#include <scc/trace.h>
#include <scc/tracer.h>
#include <util/ities.h>
#include <cstdlib>

using namespace scc;
using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    auto my_name = util::split(argv[0], '/').back();
    scc::init_logging(LogConfig().logLevel(getenv("SCC_TEST_VERBOSE")?log::DEBUG:log::FATAL).logAsync(false));
    // create tracer if environment variable SCC_TEST_TRACE is defined
    std::unique_ptr<scc::tracer> tracer;
    if(getenv("SCC_TEST_TRACE"))
        tracer=std::make_unique<scc::tracer>(my_name, scc::tracer::file_type::TEXT, true);
    // instantiate design(s)
    factory::get_instance().create();
    // run tests
    int result = Catch::Session().run( argc, argv );
    // destroy design(s)
    sc_stop();
    factory::get_instance().destroy();
    return result;
}
