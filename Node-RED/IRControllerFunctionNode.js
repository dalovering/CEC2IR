/*
This function is intended to be used in a Node-RED function node, and is included in CEC2IR-flow.json
It receives msg from an cec-in Node 
The cec-in node is provided by the node-red-contrib-cec package https://flows.nodered.org/node/node-red-contrib-cec)
The flow then sends a msg where msg.payload contains the string hex representation of an IR code.

The CEC opcodes were identified using both manual testing with a debug node and https://www.cec-o-matic.com
These CEC opcodes work well for a Sony Bravia X900H TV
The IR codes work well for a Vizio SB3621n-E8 soundbar

The message is intended to be sent to a serial out node provided by node-red-node-serialport 
The serial out node should be connected to an appopriate IR blaster

*/


var opcode = msg.payload.opcode
var arg = msg.payload.args[0]
msg = {"payload":""}

if (opcode == 68){
    switch (arg){
        case 65: //If vol up key was pressed
            msg.payload = "0xFF827D";
            break;
        case 66: //If vol down key was pressed
            msg.payload = "0xFFA25D";
            break;
        case 67: //If mute key was pressed
            msg.payload = "0xFF12ED";
            break; 
    }
    return msg;
} else if (opcode == 125){// GIVE_SYSTEM_AUDIO_MODE_STATUS, occurs when tv turns on
        msg.payload = "0xFF827D";//turn ON soundbar
        node.send(msg);
        msg.payload = "0xFFA25D";
        return msg;
}

