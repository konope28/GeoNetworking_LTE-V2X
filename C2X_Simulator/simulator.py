import os, sys, csv, traci

stop_time = 500;

#open file for data
veh_data_file = open('veh_data_file.csv', 'w', newline='')
csv_writer = csv.writer(veh_data_file, delimiter=',')
csv_writer.writerow([stop_time])

#directory of traci libraries
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
    
#path to sumo-gui.exe and .sumocfg file
sumoBinary = "D:/programs/SUMO/bin/sumo-gui.exe"
sumoCmd = [sumoBinary, "-c", "D:/programs/SUMO/Simulator/sim.sumocfg"]

#simulation start
traci.start(sumoCmd) 
step = 0
while step < stop_time:
    traci.simulationStep()
    for veh_id in traci.vehicle.getIDList():
        position = traci.vehicle.getPosition(veh_id)
        #position[]
        speed = traci.vehicle.getSpeed(veh_id)
        angle = traci.vehicle.getAngle(veh_id)
        #store data into file
        csv_writer.writerow([step, veh_id, position[0], position[1], speed, angle])
    step += 1
    
#close
veh_data_file.close()
traci.close()
