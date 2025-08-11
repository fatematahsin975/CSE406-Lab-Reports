#include "painlessMesh.h"

//------------------------------------------------------------------
// MESH DETAILS - MUST BE THE SAME ON ALL NODES
//------------------------------------------------------------------
#define   MESH_PREFIX     "MyMeshNetwork" // Must match Task 1 code
#define   MESH_PASSWORD   "MySecurePassword" // Must match Task 1 code
#define   MESH_PORT       5555

//------------------------------------------------------------------
//      *** MODIFICATION FOR TASK 2 IS HERE ***
//------------------------------------------------------------------
// Replace 0 with the actual ID of the node you want to send a message to.
uint32_t targetNodeId = 0; 
//------------------------------------------------------------------

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage(); 
Task taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessage);

//------------------------------------------------------------------
//      *** MODIFICATION FOR TASK 2 IS HERE ***
//------------------------------------------------------------------
// This function is updated to use mesh.sendSingle()
void sendMessage() {
  // First, check if you have set a target ID.
  if (targetNodeId == 0) {
    Serial.println("--> ERROR: targetNodeId is not set. Please update the code.");
    return;
  }

  String msg = "Direct message from node ";
  msg += mesh.getNodeId();

  // Check if the target node is in our routing table
  if (mesh.isConnected(targetNodeId)) {
    // sendSingle sends a message to a specific node
    mesh.sendSingle(targetNodeId, msg);
    Serial.printf("--> Sent direct message to %u\n", targetNodeId);
  } else {
    Serial.printf("--> Target node %u not found in mesh. Cannot send message.\n", targetNodeId);
  }
  
  taskSendMessage.delay(random(5000, 15000)); 
}
//------------------------------------------------------------------


// Callbacks remain the same to monitor network status
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("--> Received from %u: %s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("--> Changed Connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("--> Adjusted time. New time is %u. Offset = %d\n", mesh.getNodeTime(), offset);
}


void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  mesh.update();
}
