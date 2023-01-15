
#include "testbench.h"
#include <factory.h>
#include <tlm/scc/tlm_gp_shared.h>
#undef CHECK
#include <catch2/catch_all.hpp>
#include <unordered_map>

using namespace sc_core;

factory::add<testbench> tb;

TEST_CASE("simple_access", "[CLINT][pin-level]") {
	auto& dut = factory::get<testbench>();
	unsigned int reset_cycles{4};
	unsigned int timeout_cycles{1000};

	dut.rst.write(false);
	sc_start(reset_cycles*dut.clk.read());
	dut.rst.write(true);
	sc_start(dut.clk.read());

	auto thread1 = sc_spawn([&dut](){
		tlm::tlm_generic_payload trans;
		trans.set_command(tlm::TLM_READ_COMMAND);
		trans.set_address(0);
		trans.set_data_length(4);
		trans.set_streaming_width(4);
		uint32_t data = rand();
		trans.set_data_ptr(reinterpret_cast<uint8_t*>(&data));
		SCCDEBUG(__FUNCTION__) << "accessing addr" << trans.get_address() <<", TX: "<<trans;
		sc_core::sc_time d;
		dut.intor->b_transport(trans, d);
		REQUIRE(sc_report_handler::get_count(SC_ERROR) == 0);
		REQUIRE(sc_report_handler::get_count(SC_WARNING) == 0);
		REQUIRE(trans.get_response_status()==tlm::TLM_OK_RESPONSE);
	});

	unsigned cycles{0};
	while(cycles<timeout_cycles && !(thread1.terminated())){
		sc_start(10 * dut.clk.read());
		cycles+=10;
	}
	REQUIRE(cycles<timeout_cycles); // timeout
}
