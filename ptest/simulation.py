# Nathan Zimmerberg, Tanishq Aggarwal
# 9.14.2019
# simulation.py
# Class to run a simulation and communicate with the flight computers.

import time, timeit, traceback
import math
import platform
import threading
import os
if "CI" not in os.environ:
    import matlab.engine
import datetime
import os
import json
from .gpstime import GPSTime
from .cases.base import SingleSatOnlyCase

class Simulation(object):
    """
    Full mission simulation, including both spacecraft.
    """
    def __init__(self, is_interactive, devices, seed, testcase, print_log=True):
        """
        Initializes self

        Args:
            devices: Connected Teensy devices that are controllable
            seed(int or None) random number generator seed or None
            print_log: If true, prints logging messages to the console rather than
                       just to a file.
        """
        self.is_interactive = is_interactive
        self.devices = devices
        self.seed = seed
        self.testcase = testcase
        self.log = ""
        self.print_log = print_log

    @property
    def is_single_sat_sim(self):
        return isinstance(self.testcase, SingleSatOnlyCase)

    def start(self):
        '''
        Start the MATLAB simulation. This function is blocking until the simulation begins.
        '''

        self.sim_duration = self.testcase.sim_duration
        self.sim_time = 0

        if self.is_single_sat_sim:
            self.flight_controller = self.devices['FlightController']
        else:
            self.flight_controller_leader = self.devices['FlightControllerLeader']
            self.flight_controller_follower = self.devices['FlightControllerFollower']

        self.add_to_log("Running testcase initialization...")
        self.testcase.setup_case(self)

        if self.testcase.sim_duration > 0:
            self.add_to_log("Configuring simulation (please be patient)...")
            start_time = timeit.default_timer()
            self.running = True
            self.configure_sim()
            elapsed_time = timeit.default_timer() - start_time
            self.add_to_log("Configuring simulation took %0.2fs." % elapsed_time)

            self.add_to_log("Starting simulation loop...")
            if self.is_interactive:
                self.sim_thread = threading.Thread(name="Python-MATLAB Simulation Interface",
                                            target=self.run)
                self.sim_thread.start()
            else:
                self.run()
        else:
            self.add_to_log("Not running simulation since the testcase doesn't require it.")
            self.running = False
            self.testcase.run_case()

    def add_to_log(self, msg):
        if self.print_log:
            print(msg)
        self.log += f"[{datetime.datetime.now()}] {msg}\n"

    def configure_sim(self):
        self.eng = matlab.engine.start_matlab()
        path = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "../lib/common/psim/MATLAB")
        self.eng.addpath(path, nargout=0)

        if ((platform.system() == 'Darwin' and not os.path.exists("geograv_wrapper.mexmaci64"))
            or (platform.system() == 'Linux' and not os.path.exists("geograv_wrapper.mexa64"))
            or (platform.system() == 'Windows' and not os.path.exists("geograv_wrapper.mexw64"))
        ):
            self.eng.install(nargout=0)
        self.eng.config(nargout=0)
        # self.eng.generate_mex_code(nargout=0)
        self.eng.eval("global const", nargout=0)

        self.main_state = self.eng.initialize_main_state(self.seed, self.testcase.sim_initial_state, nargout=1)
        self.computer_state_follower, self.computer_state_leader = self.eng.initialize_computer_states(self.testcase.sim_initial_state, nargout=2)
        self.main_state_trajectory = []

        self.eng.workspace['const']['dt'] = self.flight_controller.smart_read("pan.cc_ms") * 1e6  # Control cycle time in nanoseconds.
        self.dt = self.eng.workspace['const']['dt'] * 1e-9 # In seconds.

    def run(self):
        """
        Runs the simulation for the time interval specified in start().
        """

        if self.sim_duration != float("inf"):
            num_steps = int(self.sim_duration / self.dt) 
        else:
            num_steps = float("inf")
        sample_rate = int(10.0 / self.dt) # Sample once every ten seconds
        step = 0
        
        start_time = time.time()
        while step < num_steps and self.running:
            # Step 2. Update dynamics
            print("MAINSTATE: ")
            print(self.main_state['follower']["actuators"]["magrod_real_moment_body"])
            print(self.main_state['follower']["actuators"]["wheel_commanded_ramp"])

            main_state_promise = self.eng.main_state_update(self.main_state, nargout=1, background=True)
            
            self.sim_time = self.main_state['follower']['dynamics']['time']
            print(f"simtime: {self.sim_time}")
            # print(f"Time: {self.main_state['leader']['dynamics']['time']}")
            # Step 1. Get sensor readings from simulation
            self.sensor_readings_follower = self.eng.sensor_reading(self.main_state['follower'],self.main_state['leader'], nargout=1)
            self.sensor_readings_leader = self.eng.sensor_reading(self.main_state['leader'],self.main_state['follower'], nargout=1)

            print(f"mag_body: {self.sensor_readings_follower['magnetometer_body']}")
            print(f"gyro_body: {self.sensor_readings_follower['gyro_body']}")
            print(f"Time to gps lock: {self.main_state['follower']['sensors']['gps_time_till_lock']}")

            

            # Step 3. Simulate flight computers
            # Step 3.1. Use MATLAB simulation as a base
            self.computer_state_follower, self.actuator_commands_follower = \
                self.eng.update_FC_state(self.computer_state_follower,self.sensor_readings_follower, nargout=2)
            self.computer_state_leader, self.actuator_commands_leader = \
                self.eng.update_FC_state(self.computer_state_leader,self.sensor_readings_leader, nargout=2)

            print("follower commands: ")
            # print(self.actuator_commands_follower)
            print("end commands")
            # Step 3.2. Send sim inputs, read sim outputs from Flight Computer
            self.interact_fc()
            # Step 3.3. Allow test case to do its own meddling with the flight computer.
            try:
                self.testcase.run_case()
            except Exception as e:
                traceback.print_exc()
                self.running = False

        # Step 3.4. Step the flight computer forward.
            if self.is_single_sat_sim:
                self.flight_controller.write_state("cycle.start", "true")
            else:
                self.flight_controller_follower.write_state("cycle.start", "true")
                self.flight_controller_leader.write_state("cycle.start", "true")

            
            # Step 5. Command actuators in simulation
            self.main_state = main_state_promise.result()
            self.main_state['follower'] = self.eng.actuator_command(self.actuator_commands_follower,self.main_state['follower'], nargout=1)
            self.main_state['leader'] = self.eng.actuator_command(self.actuator_commands_leader,self.main_state['leader'], nargout=1)

            # 4 comes after 5 to overwrite lmao
            # Step 4??? Read the actuators from the FC lmao
            # print("old guy: ")
            # print(self.main_state['follower']["actuators"]["magrod_real_moment_body"])
            # print(type(self.main_state['follower']["actuators"]["magrod_real_moment_body"]))

            mag_cmd = self.flight_controller.smart_read("adcs_cmd.mtr_cmd")
            yf = 1 # yeet factor
            # mag_cmd = [1000,1000,1000]
            mag_cmd = [[x*yf] for x in mag_cmd] #take transpose
            mag_cmd = matlab.double(mag_cmd) #mutate into matlab
            print(f"mag_cmd: {mag_cmd}")
            self.main_state['follower']["actuators"]["magrod_real_moment_body"] = mag_cmd
            # print("new guy: ")
            # print(self.main_state['follower']["actuators"]["magrod_real_moment_body"])
            
            torq_cmd = self.flight_controller.smart_read("adcs_cmd.rwa_torque_cmd")
            tf = 0
            torq_cmd = [[tf*x] for x in torq_cmd]
            torq_cmd = matlab.double(torq_cmd)
            # self.main_state['follower']['actuators']['wheel_enable'] = 
            self.main_state['follower']["actuators"]["wheel_commanded_ramp"] = torq_cmd

            # Step 6. Store trajectory
            if step % sample_rate == 0:
                self.main_state_trajectory.append(
                    json.loads(self.eng.jsonencode(self.main_state, nargout=1)))

            step += 1
            time.sleep(self.dt - ((time.time() - start_time) % self.dt))

        self.running = False
        self.add_to_log("Simulation ended.")
        self.eng.quit()

    def interact_fc(self):
        if self.is_single_sat_sim:
            self.interact_fc_onesat(self.flight_controller, self.sensor_readings_follower)
        else:
            self.interact_fc_onesat(self.flight_controller_follower, self.sensor_readings_follower)
            self.interact_fc_onesat(self.flight_controller_leader, self.sensor_readings_leader)

    def read_act_fc(self, fc):
        # im just gonna assume one sat sorry lol
        fc.read_state("adcs_cmd.mtr_cmd")


    def interact_fc_onesat(self, fc, sensor_readings):
        """
        Exchange simulation state variables with the one of the flight controllers.
        """
        # Step 3.2.2 Send inputs to Flight Controller
        self.write_adcs_estimator_inputs(fc, sensor_readings)
        # Step 3.2.3 Read outputs from previous control cycle
        self.read_adcs_estimator_outputs(fc)
        
        fc.read_state("adcs.state")
        # fc.read_state("adcs_cmd.mtr_cmd")

    def write_adcs_estimator_inputs(self, flight_controller, sensor_readings):
        """Write the inputs required for ADCS state estimation."""

        # Convert mission time to GPS time
        current_gps_time = GPSTime(self.sim_time)
        print(f"gpstim: {current_gps_time}")
        # Clean up sensor readings to be in a format usable by Flight Software
        position_ecef = ",".join(["%.9f" % x[0] for x in sensor_readings["position_ecef"]])
        velocity_ecef = ",".join(["%.9f" % x[0] for x in sensor_readings["velocity_ecef"]])
        sat2sun_body = ",".join(["%.9f" % x[0] for x in sensor_readings["sat2sun_body"]])
        magnetometer_body = ",".join(["%.9f" % x[0] for x in sensor_readings["magnetometer_body"]])
        w_body = ",".join(["%.9f" % x[0] for x in sensor_readings["gyro_body"]])

        # Send values to flight software
        flight_controller.write_state("orbit.time", current_gps_time.to_s())
        flight_controller.write_state("orbit.pos", position_ecef)
        flight_controller.write_state("orbit.vel", velocity_ecef)
        flight_controller.write_state("adcs_monitor.ssa_vec", sat2sun_body)
        flight_controller.write_state("adcs_monitor.mag1_vec", magnetometer_body)
        flight_controller.write_state("adcs_monitor.gyr_vec", w_body)

    def read_adcs_estimator_outputs(self, flight_controller):
        """
        Read and store estimates from the ADCS estimator onboard flight software.

        The estimates are automatically stored in the Flight Controller telemetry log
        by calling read_state.
        """

        flight_controller.read_state("adcs_monitor.mag2_vec")
        flight_controller.read_state("attitude_estimator.q_body_eci")
        flight_controller.read_state("attitude_estimator.w_body")
        flight_controller.read_state("attitude_estimator.fro_P")

    def stop(self, data_dir):
        """
        Stops a run of the simulation and saves run data to disk.
        """
        with open(data_dir + "/simulation_data_main.txt", "w") as fp:
            json.dump(self.main_state_trajectory, fp)

        with open(data_dir + "/simulation_log.txt", "w") as fp:
            fp.write(self.log)
