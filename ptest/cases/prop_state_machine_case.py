from .base import SingleSatCase
from .utils import Enums, TestCaseFailure
import time

# pio run -e fsw_native_leader
# python -m ptest runsim -c ptest/configs/fc_only_native.json -t PropStateMachineCase
class PropStateMachineCase(SingleSatCase):
    def post_boot(self):
        self.mission_state = 'manual'
        self.cycle()

        self.flight_controller.write_state("dcdc.SpikeDock_cmd", True)
        self.flight_controller.write_state(
            "prop.state", Enums.prop_states["disabled"])
        self.cycle()

    # This section is autogenerated by ./pan_generate /Users/athena/FlightSoftware/src/fsw/FCCode/PropController.cpp
    @property
    def state(self):
        return str(self.read_state("prop.state"))

    @state.setter
    def state(self, val):
        self.write_state("prop.state", str(val))

    @property
    def cycles_until_firing(self):
        return str(self.read_state("prop.cycles_until_firing"))

    @cycles_until_firing.setter
    def cycles_until_firing(self, val):
        self.write_state("prop.cycles_until_firing", str(val))

    @property
    def sched_valve1(self):
        return str(self.read_state("prop.sched_valve1"))

    @sched_valve1.setter
    def sched_valve1(self, val):
        self.write_state("prop.sched_valve1", str(val))

    @property
    def sched_valve2(self):
        return str(self.read_state("prop.sched_valve2"))

    @sched_valve2.setter
    def sched_valve2(self, val):
        self.write_state("prop.sched_valve2", str(val))

    @property
    def sched_valve3(self):
        return str(self.read_state("prop.sched_valve3"))

    @sched_valve3.setter
    def sched_valve3(self, val):
        self.write_state("prop.sched_valve3", str(val))

    @property
    def sched_valve4(self):
        return str(self.read_state("prop.sched_valve4"))

    @sched_valve4.setter
    def sched_valve4(self, val):
        self.write_state("prop.sched_valve4", str(val))

    @property
    def sched_intertank1(self):
        return str(self.read_state("prop.sched_intertank1"))

    @sched_intertank1.setter
    def sched_intertank1(self, val):
        self.write_state("prop.sched_intertank1", str(val))

    @property
    def sched_intertank2(self):
        return str(self.read_state("prop.sched_intertank2"))

    @sched_intertank2.setter
    def sched_intertank2(self, val):
        self.write_state("prop.sched_intertank2", str(val))

    @property
    def max_pressurizing_cycles(self):
        return str(self.read_state("prop.max_pressurizing_cycles"))

    @max_pressurizing_cycles.setter
    def max_pressurizing_cycles(self, val):
        self.write_state("prop.max_pressurizing_cycles", str(val))

    @property
    def threshold_firing_pressure(self):
        return str(self.read_state("prop.threshold_firing_pressure"))

    @threshold_firing_pressure.setter
    def threshold_firing_pressure(self, val):
        self.write_state("prop.threshold_firing_pressure", str(val))

    @property
    def ctrl_cycles_per_filling(self):
        return str(self.read_state("prop.ctrl_cycles_per_filling"))

    @ctrl_cycles_per_filling.setter
    def ctrl_cycles_per_filling(self, val):
        self.write_state("prop.ctrl_cycles_per_filling", str(val))

    @property
    def ctrl_cycles_per_cooling(self):
        return str(self.read_state("prop.ctrl_cycles_per_cooling"))

    @ctrl_cycles_per_cooling.setter
    def ctrl_cycles_per_cooling(self, val):
        self.write_state("prop.ctrl_cycles_per_cooling", str(val))

    @property
    def tank1_valve_choice(self):
        return str(self.read_state("prop.tank1.valve_choice"))

    @tank1_valve_choice.setter
    def tank1_valve_choice(self, val):
        self.write_state("prop.tank1.valve_choice", str(val))

    @property
    def tank2_pressure(self):
        return str(self.read_state("prop.tank2.pressure"))

    @tank2_pressure.setter
    def tank2_pressure(self, val):
        self.write_state("prop.tank2.pressure", str(val))

    @property
    def tank2_temp(self):
        return str(self.read_state("prop.tank2.temp"))

    @tank2_temp.setter
    def tank2_temp(self, val):
        self.write_state("prop.tank2.temp", str(val))

    @property
    def tank1_temp(self):
        return str(self.read_state("prop.tank1.temp"))

    @tank1_temp.setter
    def tank1_temp(self, val):
        self.write_state("prop.tank1.temp", str(val))

    @property
    def pressurize_fail(self):
        return str(self.read_state("prop.pressurize_fail"))

    @pressurize_fail.setter
    def pressurize_fail(self, val):
        self.write_state("prop.pressurize_fail", str(val))

    def print_object(self):
        print(f"[TESTCASE] state: { Enums.prop_states[int(self.state)]}")
        print(f"[TESTCASE] cycles_until_firing: {self.cycles_until_firing}")
        print(f"[TESTCASE] sched_valve1: {self.sched_valve1}")
        print(f"[TESTCASE] sched_valve2: {self.sched_valve2}")
        print(f"[TESTCASE] sched_valve3: {self.sched_valve3}")
        print(f"[TESTCASE] sched_valve4: {self.sched_valve4}")
        print(f"[TESTCASE] sched_intertank1: {self.sched_intertank1}")
        print(f"[TESTCASE] sched_intertank2: {self.sched_intertank2}")
        print(f"[TESTCASE] max_pressurizing_cycles: {self.max_pressurizing_cycles}")
        print(f"[TESTCASE] threshold_firing_pressure: {self.threshold_firing_pressure}")
        print(f"[TESTCASE] ctrl_cycles_per_filling: {self.ctrl_cycles_per_filling}")
        print(f"[TESTCASE] ctrl_cycles_per_cooling: {self.ctrl_cycles_per_cooling}")
        print(f"[TESTCASE] tank1_valve_choice: {self.tank1_valve_choice}")
        print(f"[TESTCASE] tank2_pressure: {self.tank2_pressure}")
        print(f"[TESTCASE] tank2_temp: {self.tank2_temp}")
        print(f"[TESTCASE] tank1_temp: {self.tank1_temp}")
        print(f"[TESTCASE] pressurize_fail: {self.pressurize_fail}")

    # End autogenerated section

    @property
    def min_num_cycles(self):
        return (int(self.ctrl_cycles_per_filling) + int(self.ctrl_cycles_per_cooling))*int(self.max_pressurizing_cycles) + 4

    # Note: It takes 1202 cycles to pressurize 20 cycles
    def fake_pressure(self, i):
        # If we are pressurizing and we've pressurized for 100 cycles, then pretend we made it
        if self.state == str(Enums.prop_states["pressurizing"]):
            if i > 100: 
                self.tank2_pressure = 26
    
    def fake_cycles_until_firing(self, i):
        if self.state == str(Enums.prop_states["await_firing"]):
        # If we are in 5th cycle of await_firing, then tell prop that we gonna fire soon
            self.tank2_pressure = 26
            if  i == 5:
                self.cycles_until_firing = 5

    # Step the state machine (maximum of max_cycles) until prop.state changes
    # Return the number of cycles
    def cycle_until_change(self, verbose=False, max_cycles=0):
        if max_cycles == 0:
            max_cycles = self.min_num_cycles
        old_state = self.state
        for i in range(int(max_cycles)):

            self.fake_pressure(i)
            self.fake_cycles_until_firing(i)

            if self.state != old_state:
                return str(i)
            else:
                self.cycle()
                if verbose == True:
                    print("Executing Cycle #: " + str(i))
        return str(-1)

    def test_disabled_to_idle(self):
        print("[TESTCASE] test_disabled_to_idle: manually setting prop to idle")
        if not self.read_state("dcdc.SpikeDock") == "true":
            raise TestCaseFailure("Spike and Hold DCDC is not enabled.")
        self.state = Enums.prop_states["idle"]
        self.cycle()
        if not self.state == str(Enums.prop_states["idle"]):
            raise TestCaseFailure("state != idle")

    # We should immediately transition to await_pressurizing (within 1 cycle)
    def test_idle_to_await_pressurizing(self):
        print("[TESTCASE] test_idle_to_await_pressurizing")
        print("[TESTCASE] state should automatically transition to await pressurizing upon receiving a good schedule")
        if not self.state == str(Enums.prop_states["idle"]):
            raise TestCaseFailure("state != idle")
        self.sched_valve1 = 100
        self.sched_valve2 = 700
        self.sched_valve3 = 800
        self.sched_valve4 = 400
        self.cycles_until_firing = int(self.min_num_cycles) + 2
        self.cycle()
        if not self.state == str(Enums.prop_states["await_pressurizing"]):
            raise TestCaseFailure("state != await_pressurizing")

    def test_await_pressurizing_to_pressurize(self):
        print("[TESTCASE] test_await_pressurizing_to_pressurize")
        if not self.state == str(Enums.prop_states["await_pressurizing"]):
            raise TestCaseFailure("state != await_pressurizing")
        print(f"[TESTCASE] cycles_until_change: {self.cycle_until_change()}")
        if not self.state == str(Enums.prop_states["pressurizing"]):
            raise TestCaseFailure("state != pressurizing")
        return

    def test_pressurize_to_await_firing(self):
        print("[TESTCASE] test_pressurize_to_await_firing")
        if not self.state == str(Enums.prop_states["pressurizing"]):
            raise TestCaseFailure("state != pressurizing")
        # Verbose is True since we probably want to watch the pressure rise
        print(f"[TESTCASE] cycles_until_change: {self.cycle_until_change()}")
        if not self.state == str(Enums.prop_states["await_firing"]):
            raise TestCaseFailure("state != await_firing")
        return
    
    def test_await_firing_to_firing(self):
        print("[TESTCASE] test_await_firing_to_firing")
        if not self.state == str(Enums.prop_states["await_firing"]):
            raise TestCaseFailure("state != await_firing")
        # Expect to be here for a long time
        print(f"[TESTCASE] cycles_until_change: {self.cycle_until_change()}")
        if not self.state == str(Enums.prop_states["firing"]):
            raise TestCaseFailure("state != firing")
        return

    def test_firing_to_idle(self):
        print("[TESTCASE] test_firing_to_idle")

        if not self.flight_controller.is_teensy:
            print("[TESTCASE] Test skipped since we are on a native platform.")
            return

        if not self.state == str(Enums.prop_states["firing"]):
            raise TestCaseFailure("state != firing")
        print(f"[TESTCASE] cycles_until_change: {self.cycle_until_change()}")
        if not self.state == str(Enums.prop_states["idle"]):
            raise TestCaseFailure("state != idle")
        return

    def run(self):
        print("------------------------------------------------")
        self.test_disabled_to_idle()
        print("------------------------------------------------")
        self.test_idle_to_await_pressurizing()
        print("------------------------------------------------")
        self.test_await_pressurizing_to_pressurize()
        print("------------------------------------------------")
        self.test_pressurize_to_await_firing()
        print("------------------------------------------------")
        self.test_await_firing_to_firing()
        print("------------------------------------------------")
        self.test_firing_to_idle()
        print("------------------------------------------------")
        print("[TESTCASE] Propulsion State Machine Test finished.")
        self.finish()
