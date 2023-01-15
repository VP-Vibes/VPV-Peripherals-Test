#ifndef _TESTBENCH_H_
#define _TESTBENCH_H_

#include <minres.h>
#include <sifive.h>
#include <scc.h>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace axi;
using namespace axi::pe;

class testbench : public sc_core::sc_module {
public:

    sc_core::sc_time clk_period{10, sc_core::SC_NS};
    sc_core::sc_time lf_clk_period{100, sc_core::SC_NS};
    sc_core::sc_signal<sc_core::sc_time> clk{"clk", clk_period};
    sc_core::sc_signal<bool> rst{"rst"};
    sc_core::sc_signal<sc_core::sc_time> lfclk_i{"lfclk_i", lf_clk_period,};
    sc_core::sc_signal<bool> mtime_irq{"mtime_int"};
    sc_core::sc_signal<bool> msw_irq{"msw_irq"};

    // initiator side
    tlm_utils::simple_initiator_socket<testbench, 32> intor{"intor"};
    scc::router<> i_router{"i_router", 1, 1};
    vpvper::sifive::clint clint_i{"clint_i"};

public:
    SC_HAS_PROCESS(testbench);
    testbench(): testbench("testbench") {}
    testbench(sc_core::sc_module_name nm)
    : sc_core::sc_module(nm)
    {
        intor(i_router.target[0]);
        //                   target,     index, start, size
        i_router.bind_target(clint_i.socket, 0, 0x0, 0x1000);

    	clint_i.tlclk_i(clk);
    	clint_i.lfclk_i(lfclk_i);
    	clint_i.rst_i(rst);
    	clint_i.mtime_int_o(mtime_irq);
    	clint_i.msip_int_o(msw_irq);

        SC_THREAD(gen_reset);
    }

    void gen_reset() {
        rst = true;
        wait(10_ns);
        rst = false;
    }
};

#endif // _TESTBENCH_H_
