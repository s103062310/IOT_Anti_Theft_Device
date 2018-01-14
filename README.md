# IOT_Anti_Theft_Device


## Hardware


## Network


## Phone APP

### Function Description
+ Trace Lost Thing: decide whether the target is stolen or not, and deal with the decision.
+ call police: when confirm the message that the target was stolen, pop out the choice box to choose whether sending message to police or not.
+ view statistical data website: be able to view all stolen record and other statistical data from official website.

### Prototype
-----------------------------
> log in page
>> main meun
>>> start monitor(confirm box: function)  
>>> trace target  
>>> view website  
>>> setting  
-----------------------------

#### log in page
+ Connect to **Server**.
+ Need account box and password box.
+ Need log in button and create account button.
+ Create page: other screen
	>+ Connect to **Server**.
	>+ Need account box and password box.  
	>+ Need create button.  
	>+ Pop out dialog to show create result.  
	>>+ If sucess, confirm and go back to log in page.   
	>>+ If fail, confirm and stay at create page.  

#### start monitor
+ First, pop out function confirm box, push **"yes"** to enter the page.
+ When receive moving message from **server**,
	>+ If the target is nearby, target is not stolen.  
	>+ If the target is not nearby, target is stolen.  
+ If target was stolen, display warning message and vibrate.
	>+ **Send message to police**: send message then go to trace.  
	>+ **Start trace**: directly go to trace.  
	>+ **Deny**: target is not stolen, back to monitor page.  

#### trace target
+ Choose one target to enter the page.
+ Display timestamp, user location.
+ Display timestamp, target location.
+ Display radar map, distance.
+ Connect to **Server**

#### view website
+ Open the browser and go to the official website page.

#### setting
+ Show information:
	>+ User Name   
	>+ Modify Password   
	>+ Add Target   
	>+ Remove Target   
	>+ **Show ALL Target**   
	>+ (Alarm Sound)   
	>+ (other information)   
+ Modify Password: other screen
	>+ Need to connect to **Server**.     
	>+ Need original password box and new password box.   
	>+ Need modify button.   
	>+ Pop out dialog to show result.    
	>>+ If sucess, back to setting screen.    
	>>+ If fail, stay at modify password screen.   
+ Add Target: other screen 
	>+ Need target name box and target ID box.   
	>+ Need add button.   
	>+ Need connect to **Server** to establish connection with sensor.   
+ Add Target: pick list 
	>+ Need a pick list to show all target.   
	>+ After choosing, remove the target and notify **Server**.   
+ (Alarm Sound: toggle button)
	>+ Need to connect to **Server**.    
	>+ Need to notify sensor to turn off sound through server.   


## Database

Use MySQL establish database.

### Account Table
|User Name|Password|
|:--------|:-------|
|Cindy    |062310  |
|...      |...     |

### Target List Table
+ Except MAC and user, all initialize to 0.    

|Target MAC|User |Time Stamp|Latitude|Longitude|
|:---------|:----|:---------|:-------|:--------|
|dli5s6    |Cindy|2018/01/13|24.5    |120.1    |
|...       |...  |...       |...     |...      |

### Event Table
|Latitude|Longitude|Region|Time Stamp|
|:-------|:--------|:-----|:---------|
|24.5    |121      |Delta |2018/01/13|
|...     |...      |...   |...       |

### Place Table
+ All place initialize to 0.   

|Region |Number of Events|
|:------|:---------------|
|Delta  |20              |
|library|50              |
|...    |...             |

### Time Table
+ Hour: 0~23   

|Hour|Number of Events|
|:---|:---------------|
|0   |6               |
|1   |12              |
|... |...             |


## Server

### Function Description
+ Organize the data that sensors collect.
+ Provide API for phone APP and Website.

### Organize Data
+ Decide GPS latitude and longitude belong to which region in NTHU.
+ When new event occurs, update the statistical data.
	>+ place table: convert GPS data to region, and add 1 case at corresponding region.
	>+ time table: add 1 time at corresponding hour

### What APP Needs?
+ login page
	>1. look up account   
	>>+ input: user name   
	>>+ output: No user **or** user password   
	>2. create new account: add new element to Account Table   
	>>+ input: user name **and** password   
	>>+ output: nothing   
+ monitor/trace page
	>1. trace target   
	>>+ input: target MAC   
	>>+ output: timestamp **and** GPS data(latitude&longitude)   
	>>  (if not found MAC, return all zero)   
	>2. add event: add new element to Event Table   
	>>+ input: timestamp **and** GPS data(latitude&longitude)   
	>>+ output: nothing   
+ setting page
	>1. modify password: update Account Table   
	>>+ input: user name **and** new password   
	>>+ output: nothing   
	>2. add target: add new element to Target List Table   
	>>+ nput: user name **and** target MAC   
	>>+ output: nothing   
	>3. remove target: delete element from Target List Table   
	>>+ input: target MAC   
	>>+ output: nothing   


## Website