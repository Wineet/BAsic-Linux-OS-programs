***** Sensor Based Door Bell Embedded System ********

Note: Master Referred as Main Process

Motion Sensor >>
					motion sensor detects the motion in front of door 
					if motion is there it will trigger distance calculate Sensor.
					Motion sensor is in working state or failed. need to know to the Master.
					
Distance Sensor >>
					if Distance is in between the limits of threashold it will trigger to
					Ring the Door Bell. If Distance is out of Limits just print and ignore 
					further actions. 
					If Distance Sensor it self it is not working then let Master Know about it,
					it will take appropriate action.
Door Bell Ring	>>
					Door Bell need to ring twice it will take 2.5 seconds to complete 
					and let system know that Door bell ringing is done. If sucess variable will set as
					sucess.
					After Door Bell ring timer expiery need to check weather Door bell ring was
					sucessful or not. If it was not sucessful operation then inform. Master it will
					take appropriate action.
					
					
Assumption :
					
		All Input Need to take from Files Only.
		Motion Sensor Will take Input From file "Motion.txt" If it contains "Yes" then Motion is detected
		If "No" then Motion is not there. Default Value is "No".
		
		Distance value will read from "Distance.txt" File. On second Line Distance Sensor Working status
		will be there, If "Working" then Sensor is Fine if "Not Working" then sensor is Not Working.
		
		Door Bell Will read Door Bell status From "Bell.txt" file. In sucess case file will have "sucess"
		In Failure Case will have "unsucessful" in the File.
		
		
Note:   All Inputs are taken From File Due to Hw Limitation. But in future. File function call,
		Can be replaced with actual Sensor Function Call.
		
		Batch or shell script will be provided, which create all above Files with Default Values in it.
		