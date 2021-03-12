/*
telemetry.js pulls data for every field initialized in './state-variables.js'
It then generates it into a form that can be understood by OpenMCT
Then it adds it to the hisorical and realtime servers through the history and listener fields.
*/

/**
 * This object, imported from './state-variables.js', holds all of the inital values of telemetry points for every subsystem/domain object
 */
const variables = require('./state-variables.js')
const request = require('request');

/**
 * The URL of the Elastic Search database
 */
var searchURl = 'http://localhost:5000/search-es';
/**
 * The index of the Elastic Search database
 */
var searchIndex = 'statefield_report_123456789012345';

/**
 *  constructor initializing and then calling for the generation and updating of all telemetry points from all subsytems and domains
 */
function Telemetry() {
    //This state function takes in initial values from the state-variables.js file
    this.initialState = variables;

    //all of the current values for the telemetry
    this.state = {};

    //creates an entry in state for every variable in './state-variables'
    Object.entries(this.initialState).forEach(function ([statesSubsystem,v]) {
      Object.entries(v).forEach(function ([k,v]) {
        let key = statesSubsystem + '.' + k;
        this.state[key] = v;
      }, this);
    }, this);

    //all of the historical telemetry data
    this.history = {};

    //the listeners for the real time telemetry
    this.listeners = [];

    //adds all the initial values to history
    Object.entries(this.state).forEach(function ([k,v]) {
      this.history[k] = [];
    }, this);

    //updates the states, generates the realtime listers/notifications and historical telemetry ever 1 second.
    setInterval(function () {
        this.updateState();
        this.generateTelemetry();
    }.bind(this), 1000);

    console.log("Now reading spacecraft telemetry")

};

/**
*   Updates the state fields with the new values in the Elastic Search database.<br>
*
*   This method has two cases:
* - if the state is an object, meaning it has substates (output1, output2, etc.)
*   then update the states object for each substate
* - if the state is a primitive type, int,bool, or char then update the state
*   for the state value directly
**/
Telemetry.prototype.updateState = async function () {
  Object.keys(this.state).forEach(async function (id) {

    //if the value for the key of the state entry is an object
    if(typeof(this.state[id]) == 'object'){
      
      Object.keys(this.state[id]).forEach(async function (subId){
        //send a request to Elastic Search for the field
        let res = await this.getValue(searchURl, searchIndex, id + '.' + subId);
        (this.state[id])[subId] = res;//update state
      },this)

    }
    //if the value for the key of the state entry is a primitive
    else{
      //send a request to Elastic Search for the field
      let res = await this.getValue(searchURl, searchIndex, id);
      this.state[id] = res;//update state
    }

  }, this);
};

/**
 * 
 * Requests the data from Elastic Search for field f
 * 
 * @param {*} myUrl the URL for the Elastic Search database
 * @param {*} i  the index of the Elastic Search database
 * @param {*} f the field that's value is being requested
 */
Telemetry.prototype.getValue = async function(myUrl, i, f){
  
  //properties of the search
  var propertiesObject = { index: i, field:f };

  let p = new Promise(function(resolve, reject){

    //requests URL based on properties
    request({url: myUrl, qs:propertiesObject}, function(err, response, body) {
      if(!err && response.statusCode == 200) { resolve(body);}
      else{ reject(err); }
    });
  });
  return await p;
  }



/**
 * Takes a measurement of all domain object's states, determines its type, stores in history, and notifies
 * listeners.
 *
 *This method has two cases:
 * - if the state is an object, meaning it has substates (output1, output2, etc.)
 *   then generate telemetry for each substate
 * - if the state is a primitive type, int,bool, or char then generate Telemetry
 *   for the state value directly
 */
Telemetry.prototype.generateTelemetry = function () {
    var timestamp = Date.now(), sent = 0;
    //make two cases one that updates objects and one that directly updates field
    
    Object.keys(this.state).forEach(function (id) {

      //if the value for the key of the state entry is an object
      if(typeof(this.state[id]) == 'object'){

        //generate telemetry point oject
        var telempoint = { timestamp: timestamp, id: id};
        for (const output in this.state[id]){
          telempoint[output] = this.state[id][output];
        }

        //notify the realtime server and push the datapoint to the history server
        this.notify(telempoint);
        this.history[id].push(telempoint);

      }
      //if the value for the key of the state entry is a primitive
      else{

        //generate telemetry point primitve state
        var telempoint = { timestamp: timestamp, value: this.state[id], id: id};

        //notify the realtime server and push the datapoint to the history server
        this.notify(telempoint);
        this.history[id].push(telempoint);
      }

    }, this);

};

/**
 * Notifies the realtime server of a new telemetry point
 * 
 * @param {*} point The telelmetry point to notify an update to to the realtime server for
 */
Telemetry.prototype.notify = function (point) {
    this.listeners.forEach(function (l) {
        l(point);
    });
};

/**
 * Adds and sets up the listener for realtime telemetry
 * 
 * @param {*} listener The lister being added
 */
Telemetry.prototype.listen = function (listener) {
    //adds the listener
    this.listeners.push(listener);
    return function () {
        this.listeners = this.listeners.filter(function (l) {
            return l !== listener;
        });
    }.bind(this);
};

//exports the telemetry function for use in './server.js'
module.exports = function () {
    return new Telemetry()
};
