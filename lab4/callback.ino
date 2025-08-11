#include "painlessMesh.h"

//------------------------------------------------------------------
// MESH DETAILS - MUST BE THE SAME ON ALL NODES
//------------------------------------------------------------------
#define   MESH_PREFIX     "MyMeshNetwork" // Use a unique name for your mesh
#define   MESH_PASSWORD   "MySecurePassword" // Use a unique password
#define   MESH_PORT       5555

//------------------------------------------------------------------
// GLOBAL OBJECTS
//------------------------------------------------------------------
Scheduler userScheduler; // to control your own tasks
painlessMesh  mesh;

//------------------------------------------------------------------
// FORWARD DECLARATIONS
//------------------------------------------------------------------
void sendMessage(); 

// Create a task to send a message periodically
// TASK_SECOND * 5 means the task runs every 5 seconds
Task taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessage);

//------------------------------------------------------------------
// USER-DEFINED FUNCTIONS & TASKS
//------------------------------------------------------------------
void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  // sendBroadcast sends the message to every node on the mesh
  mesh.sendBroadcast(msg);
  
  Serial.printf("--> Sent broadcast: %s\n", msg.c_str());
  
  // Reschedule the task with a new random interval
  taskSendMessage.delay(random(5000, 15000)); 
}


//------------------------------------------------------------------
// MESH CALLBACKS (This is the important part for Task 1)
//------------------------------------------------------------------

// This function is called when your node receives a message
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("--> Received from %u: %s\n", from, msg.c_str());
}

// This function is called when a new node connects to your node directly
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> New Connection, nodeId = %u\n", nodeId);
}

// This function is called when the connections in the mesh change
void changedConnectionCallback() {
  Serial.printf("--> Changed Connections\n");
}

// This function is called when the node's clock is adjusted
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("--> Adjusted time. New time is %u. Offset = %d\n", mesh.getNodeTime(), offset);
}


//------------------------------------------------------------------
// SETUP & LOOP
//------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Set the types of debug messages you want to see
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  
  // Initialize the mesh
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  // Register the callback functions
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Add the sendMessage task to the scheduler
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  // Must call mesh.update() in your loop to keep the mesh running
  mesh.update();
}