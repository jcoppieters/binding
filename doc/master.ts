import {AlertController, ToastController} from "@ionic/angular";
import {Directory, Filesystem} from '@capacitor/filesystem';

import {Node, Protocol, Rec, reqNodeAttributes, Unit, setSensorMethods} from "./protocol";
import {CommRecord, kVirtualNodeAddress, Message, NodeType, WriteError} from "./types";
import {TemperatureConfig} from "../models/temperature";
import {System} from "./system";
import {Logger} from "./logger";
import {Q} from "./Q";
import {doAlert, doToast} from '../core/ux';
import {MasterIP} from "./masterIP";
import {AudioConfig, BasicAudioSources, BasicAudioType} from "../models/audio";
import {CameraConfig} from "../models/camera";
import {RegisterConfig} from "../models/register";
import {MasterConfig} from "../models/master";
import {MasterMood} from "../models/mood";
import {Sanitizers} from "./sanitizers";
import {UnitInfo, UnitType} from "../models/unit";
import {NodeConfig} from "../models/nodes";
import {asNum, dateTimeString, hex, hex2} from "./helper_functions";
import {PictureKind} from "../models/pictures";
import {TemperatureControlMode} from "../models/temperature";

export class Master extends Logger {

  public system: System;
  public nodes: Array<Node>;
  public nrNodes: number;
  public schedule: number;
  public date: Date;
  public lastHeartbeat: Date;
  private onlyThisOne: number = 0;

  public audio: Array<AudioConfig> = [];
  public cameras: Array<CameraConfig> = [];
  public registers: Array<RegisterConfig> = [];
  public temperatures: Array<TemperatureConfig> = [];

  public isOpen: boolean;
  public isLoggedIn: boolean;
  private resolveLogin = null;
  private closeRequested: boolean;
  private buffer: string;

  version: number;
  protocol: number;

  private Q: Q;

  active: boolean;           // combination of config.active + period (config.start/end) is valid
  config: MasterConfig;
  moods: Array<MasterMood>;

  private socket: WebSocket; //  Socket;
  private heartbeatInterval: any = null; // Store heartbeat interval to prevent multiple timers

  constructor(system: System, config: MasterConfig, 
              private toastCtrl: ToastController, 
              private alertCtrl: AlertController,
              private ipMaster: MasterIP) {

    super("master");
    this.Q = new Q(system.logger);

    // save my eco system
    this.system = system;

    // save this masters config settings
    this.config = Sanitizers.masterConfig(config);
    // this.resetActive();

    // all nodes in this master
    this.nodes = [];
    this.nrNodes = 0;
    this.schedule = 0;
    this.date = null;

    // connection to an IP node / smartbox
    this.socket = null;
    this.isOpen = false;
    this.isLoggedIn = false;
    this.closeRequested = false;

    // incoming data
    this.buffer = "";

    // init to v1.00
    this.version = 100;
    this.protocol = 209;  
  }

  asMasterConfig(): MasterConfig {
    const node = this.findMasterNode();
    return {
      name: this.getName(), password: this.config.password,
      address: this.getAddress(),
      port: this.getPort(),
      physicalAddress: this.config.physicalAddress || node?.physicalAddress || -1,

      caddress: node?.serverAddress || this.config.caddress || this.getAddress(),
      cport: node?.serverPort || this.config.cport || this.getPort(),

      active: this.config.active,
      nodes: this.nodes.map(n => {
        return n.asNodeConfig(n.units.map(u => u.asUnitInfo()))
      }),
      end: this.config.end,
      start: this.config.start,

      // start_date: this.config.start_date,
      // end_date: this.config.end_date,
      // start_time: this.config.start_time,
      // end_time: this.config.end_time,

      locked: this.config.locked,
      backup: this.config.backup,
      
      socketserver: this.config.socketserver,
      socketport: this.config.socketport
    };
  }

  clearConfig() {
    this.audio = [];
    this.cameras = [];
    this.registers = [];
    this.temperatures = [];
  }

  getName() {
    return this.config.name || "master";
  }
  getSort() {
    return this.getName().toLowerCase();
  }

  log(msg: string) {
    // overwrite to add IP address
    super.log(this.getAddress() + ": " + msg);
  }

  getConfig(): MasterConfig {
    return this.config;
  }
  hasAddress(ip: string): boolean {
    return this.config.address === ip;
  }
  getAddress(): string {
    return this.config.address;
  }

  getPort(): number {
    return this.config.port;
  }
  hasPort(port: number): boolean {
    return this.config.port == port;
  }
  getURL(): string {
    if (this.system.config.usegateway) {
      // Use master-specific proxy settings, fallback to global settings
      const proxyServer = this.config.socketserver || this.system.config.socketserver;
      const proxyPort = this.config.socketport || this.system.config.socketport;
      
      // If we're still using the old proxy server, upgrade to new one automatically
      const isOldProxy = proxyServer === 'ws.duotecno.eu' || proxyPort === 9999;
      const actualProxyServer = isOldProxy ? 'masters.duotecno.eu' : proxyServer;
      const actualProxyPort = isOldProxy ? 5098 : proxyPort;
      
      // Old proxy (ws.duotecno.eu) required port-1 convention; new proxy uses port directly
      const masterPort = (actualProxyServer === 'masters.duotecno.eu') ? this.config.port : (this.config.port - 1);
      const url = actualProxyServer + ":" + actualProxyPort + "/" + this.config.address + ":" + masterPort;
      console.log(`[Master ${this.config.name}] getURL() -> ${url} (master proxy: ${this.config.socketserver || 'none'}:${this.config.socketport || 'none'}, global: ${this.system.config.socketserver}:${this.system.config.socketport}, using: ${actualProxyServer}:${actualProxyPort})`);
      return url;
    } else {
      return this.config.address + ":" + this.config.port;
    }
  }


  inMultiNode(): boolean {
    return (this.nodes.length > 1);
  }

  same(master: Master | string, port?: number): boolean {
    if (typeof master === "string") {
      if (typeof port === "undefined") { 
        // master is probably url ip:port
        const url = master.split(":");
        master = url[0];
        port = parseInt(url[1] || "5002");
      }
      return this.hasAddress(master) && this.hasPort(port);
      
    } else {
      return master && this.hasAddress(master.getAddress()) && this.hasPort(master.getPort());
    }
  }

  ////////////////////////////////////
  // Finding & looping nodes, units //
  ////////////////////////////////////
  forAllNodes(callback: (n: Node) => void) {
    if (this.active)
      this.nodes.forEach(node => {
        callback(node);
      });
  }

  forAllUnits(callback: (u: Unit) => void) {
    if (this.active)
      this.nodes.forEach(node => {
        node.units.forEach(unit => {
          callback(unit);
        });
      });
  }
  allActiveUnits(unitType?: UnitType | Array<UnitType>): Array<Unit> {
    if (!this.active)
      return [];

    else if (! unitType) 
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(
        n.units.filter(u => u.active)
      ), []);

    else if (unitType instanceof Array)
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(
        n.units.filter(u => (u.active && (<Array<UnitType>>unitType).includes(u.type)))
      ), []); 

    else
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(
        n.units.filter(u => (u.active && (u.type == unitType)))
      ), []);    
  }
  nrActiveUnits(): number {
    return this.nodes.reduce((acc: number, n) => acc + n.units.filter(u => u.active).length, 0);
  }

  allUnits(unitType?: UnitType | Array<UnitType>): Array<Unit> {
    if (!this.active)
      return [];

    else if (! unitType) 
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(n.units), []);

    else if (unitType instanceof Array) 
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(
        n.units.filter(u => (<Array<UnitType>>unitType).includes(u.type)
      )), []);

    else
      return this.nodes.reduce((acc: Array<Unit>, n) => acc.concat(
        n.units.filter(u => (u.type == unitType)
      )), []);  
  }

  allNodes(nodeType?: NodeType): Array<Node> {
    return this.nodes.filter(n => (!nodeType || (n.type == nodeType)));
  }

  configNodes(all = false): Array<Node> {
    const nodes = this.nodes.filter(n => ((n.type == NodeType.kGatewayNode) || (n.type == NodeType.kGUINode)) && (all || n.hasSettings()));
    // don't return switch with lcd in them, they are also types as GUINode...
    // return nodes.filter(n => ((n.physicalAddress&0xffff0000) != 0x1C0000));
    return nodes;
  }

  findUnit(nodeAddress: number, unitAddress: number): Unit | null {
    const node = this.findNode(nodeAddress);
    if (node) {
      return node.units.find((u: Unit) => u?.unitAddress == unitAddress);
    }
    else {
      return null;
    }
  }

  findNode(nodeAddress: number): Node {
    return this.nodes.find(n => n?.nodeAddress === nodeAddress);
  }
  findNodeConfig(nodeAddress: number): NodeConfig {
    return this.config.nodes.find(nc => (nc.nodeAddress === nodeAddress));
  }
  findNodeConfigIndex(nodeAddress: number): number {
    return this.config.nodes.findIndex(nc => (nc.nodeAddress === nodeAddress));
  }
  findUnitConfig(nodeAddress: number, unitAddress: number): UnitInfo {
    return this.config.nodes.find(nc => nc.nodeAddress == nodeAddress)?.units.find(uc => uc.unitAddress == unitAddress);
  }
  findUnitConfigIndex(nodeAddress: number, unitAddress: number): number {
    return this.config.nodes.find(nc => nc.nodeAddress == nodeAddress)?.units.findIndex(uc => uc.unitAddress == unitAddress);
  }

  findMasterNode(): Node {
    const masterNode = this.findNode(0xfc);
    return ((masterNode?.type === NodeType.kGatewayNode) || 
            (masterNode?.type === NodeType.kGUINode)) ? masterNode : null;
  }
  
  resetActive() {
    // const now = new Date();
    // const start = this.config.start_date ? new Date(this.config.start_date) : null;
    // if (start) start.setTime(this.config.start_time);
    // const end = this.config.end_date ? new Date(this.config.end_date) : null;
    // if (end) end.setTime(this.config.end_time);
    // this.active = this.config.active && (!start || !end || (now > start && now < end));


    const nowStr = dateTimeString(new Date());
    // console.log("resetActive -> calc active: " + ((this.config.active) && (nowStr > this.config.start) && (nowStr < this.config.end)) +
    //          " := (" + this.config.active + " && " + this.config.start + " < " + nowStr + " < " + this.config.end + ")");
    this.active = (this.config.active) && (nowStr > this.config.start) && (nowStr < this.config.end);
    this.log(this.getName() + " -> enabled by time: " + ((nowStr > this.config.start) && (nowStr < this.config.end)));
  }

  /* ************* */
  /* Communication */
  /* ************* */
  async open(): Promise<boolean> {
    // prevent from opening non-active master or time limited
    this.resetActive();
    if (!this.active) return false;

    // CRITICAL FIX: Check if socket already exists and clean up before creating new one
    if (this.socket) {
      this.log("Socket already exists, cleaning up before creating new connection");
      
      // Clear existing heartbeat interval
      if (this.heartbeatInterval) {
        clearInterval(this.heartbeatInterval);
        this.heartbeatInterval = null;
        this.log("Cleared existing heartbeat interval");
      }
      
      // Close existing socket
      try {
        this.socket.close();
        this.log("Closed existing socket");
      } catch (e) {
        this.err("Error closing existing socket: " + e);
      }
      
      // Reset state
      this.socket = null;
      this.isOpen = false;
      this.isLoggedIn = false;
    }

    const url = this.getURL();
  
    return new Promise<boolean>((resolve, reject) => {
      try {
        ////////////////////////////////
        // try to open the connection //
        this.log("Opening connection to the Websocket Proxy");

        const wPath = "ws://" + url;

        this.socket = new WebSocket(wPath);

        if (!this.socket) {
          this.err("Could not create new web socket to " + wPath);
          doToast(this.toastCtrl, "Could not create new web socket to " + wPath, 2000);
            reject(false);
        }
 
        ///////////////////////
        // set data listener //
        this.socket.onmessage = (message) => {
          // messages need to be buffered until "]" is received
          this.handleData(message.data);
        };

        ///////////////////////////
        // set an error listener //
        this.socket.onerror = (err) => {
          this.err("Socket: " + JSON.stringify(err) + " on " + wPath);
          // doToast(this.toastCtrl, "Error opening Socket: " + wPath, 2000);
        };

        ///////////////////////////////////////////
        // set end: the server closed the socket //
        this.socket.onclose = () => {
          this.isOpen = false;
          this.isLoggedIn = false;

          // stop sending heartbeats - use class property instead of local variable
          if (this.heartbeatInterval) {
            clearInterval(this.heartbeatInterval);
            this.heartbeatInterval = null;
            this.log("Heartbeat stopped (socket closed)");
          }

          this.log("end -> socket got disconnected on " + wPath);

          if (!this.closeRequested) {
            // unexpected close
            this.err("Socket: closed unexpectedly on " + wPath);
            // this.toastCtrl, "Socket: closed unexpectedly", 2000);
          }
        };

        ///////////////////////////
        // socket actually opens //
        this.socket.onopen = () => {
          this.isOpen = true;

          // request a connection to the real socket
          this.log("connection open on " + wPath);

          // resolve our promise with the opened socket
          resolve(true);

          setTimeout(() => this.system.emitRefresh(), 5);

          // Clear any existing heartbeat before starting new one
          if (this.heartbeatInterval) {
            clearInterval(this.heartbeatInterval);
            this.log("Clearing existing heartbeat before starting new one");
          }

          // setup a heartbeat using class property for proper lifecycle management
          this.heartbeatInterval = setInterval(() => {
            this.send(Protocol.buildHeartbeat());
          }, 15 * 1000);
          
          this.log("Heartbeat started");
        };


      } catch (e) {
        console.log(e);
        this.err("Failed to open a connection to " + url + " -> " + e);
        doAlert(this.alertCtrl, "Failed to open a connection to " + url + " -> " + e);
        reject(false);
      }
    });
  }


  async close() {
    // Clear the heartbeat interval BEFORE closing the socket
    if (this.heartbeatInterval) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
      this.log("Heartbeat stopped (manual close)");
    }

    if (this.isOpen) {
      const message = Protocol.buildDisconnect();
      try {
        this.closeRequested = true;
        Protocol.write(this.socket, message);
        // await this.send(message);
        // server will close the socket, no need to call socket.close() manually

      } catch (err) {
        this.err("Disconnect failure: " + err);
      }
    }
  }

  async login(): Promise<boolean> {
    return new Promise((resolve, reject) => {
      try {
        const message = Protocol.buildLogin(this.config.password)
        Protocol.write(this.socket, message);
        // to be called when logged in
        this.resolveLogin = resolve;

      } catch (err) {
        this.resolveLogin = null;
        doToast(this.toastCtrl, "Login error: " + err, 2000);
        this.err("Login call failed: " + err);
        reject(false);
      }
    });
  }

  getSocket(): WebSocket {
    return this.socket;
  }

  async send(message: Message) {
    return this.Q.exec(async () => {
      if (! this.isOpen) {
        await this.open();
        if (! await this.login()) {
          return WriteError.writeFatal;
        }
      }
      return Protocol.write(this.socket, message);
    });
  }

  handleData(message: string) {
    // put the incoming data into a buffer and only use complete data
    this.buffer += message;

    while ((this.buffer.length > 0) && this.processData()) {
    }
  }

  processData(): boolean {
    // fetch next message and store the rest of unused data (if any)
    const next = Protocol.nextMessage(this.buffer);
    this.buffer = next.rest;

    if (!next.cmd) {
      return false;

    } else {
      /*
      if (next.cmd == Rec.Internal) {
        this.log("incoming msg=" + recName(next.cmd) +
        ", data=" + ((next.raw) ? next.raw.substr(0,50) : "--"));

      } else if (next.cmd != Rec.Sensor) { // don't log temperature
        this.log("incoming msg=" + recName(next.cmd) + ", status=" + next.isStatus +
        ", data=" + ((next.message) ? next.message.join(",") : "--"));
      }
      */

      this.Q.do();

      if (next.isStatus) {
        this.receiveStatus(next);

      // non-unit specific
      } else if (next.cmd === Rec.TimeDateStatus) {
        this.receiveDateTime(next.message);
    
      } else if (next.cmd === Rec.Info) {
        this.receiveInfo(next);

      } else if (next.cmd === Rec.ConnectStatus) {
        this.receiveLogin(next.message);

      } else if (next.cmd === Rec.ScheduleStatus) {
        this.receiveSchedule(next.message);

      } else if (next.cmd === Rec.HeartbeatStatus) {
        this.receiveHeartbeat(next.message);
      
      } else if (next.cmd === Rec.NodeMgtInfo) {
        this.receiveNodeMgtInfo(next.message);

      } else if (next.cmd === Rec.Register) {
        this.receiveRegister(next.message);

      } else if (next.cmd === Rec.Internal) {
        // this.receiveInternal(next.raw);
        this.log("Received an internal message !!");

      } else if (next.cmd === Rec.AutoStatus) {
        // this.log("Received an auto status (IR?): " + next.message);

      } else if ((next.cmd === Rec.AudioExtendedStatus) || (next.cmd === Rec.AudioStatus)) {
        this.receiveAudioStatus(next);
  
      } else if (next.cmd != Rec.ErrorMessage) {
        this.log("what to do with: " + next.message);

      }
      return true;
    }
  }


  ///////////////////
  // Login message //
  ///////////////////

  async receiveLogin(message: Message) {
    this.isLoggedIn = (message[2] === 1);
    this.log("Login message: " + message);

    if (this.resolveLogin) {
      if (!this.isLoggedIn)
        doToast(this.toastCtrl, "Login " + (this.isLoggedIn ? "OK" : "FAIL") + " for " + this.config.address);
      this.resolveLogin(this.isLoggedIn);
      this.resolveLogin = null;

      // if logged in, ask week schedule and protocol version
      if (this.isLoggedIn) {
        await this.send(Protocol.buildRequestSchedule());
        await this.send(Protocol.buildRequestNodeMgt(reqNodeAttributes.nodeProtocol));
      }

    } else {
      this.err("unexpected ConnectStatus ?");
    }
  }



  ///////////////////
  // Info messages //
  ///////////////////

  receiveNodeMgtInfo(message: Message) {
    if (message[1] === reqNodeAttributes.isMaster) {
      this.log("NodeMgtInfo - master " + this.getName() + " is set to master: " + message[2]);

    } else if (message[1] === reqNodeAttributes.nodeInfo) {
      // should be the same as the one from the DBInfo (64)
      this.log("NodeMgtInfo - node info for " + this.getName() + ": " + message);

    } else if (message[1] === reqNodeAttributes.masterSupported) {
      this.log("NodeMgtInfo - master " + this.getName() + " can be master: " + message[2]);

    } else if (message[1] === reqNodeAttributes.nodeVersion) {
      this.log("NodeMgtInfo - version " + this.getName() + ", version: " + message[2] + "." + message[3]);
      //this.version = message[2] * 100 + message[3];


    } else if (message[1] === reqNodeAttributes.nodeProtocol) {
      this.log("NodeMgtInfo - protocol " + this.getName() + ", protocol: " + message[2] + "." + message[3]);
      //this.protocol = message[2] * 100 + message[3];

    } else {
      this.err("dropped NodeMgtInfo info message = " + message[1] + ", message: " + message);
    }
  }

  receiveRegister(message: Message) {
    const register = Protocol.makeWord(message, 2);
    const value = Protocol.makeSigned(message, 4);

    if ((register < 0) && (register > 1024)) {
      this.err("Received register out of bound: " + register);

    } else {
      const reg = this.registers.find(r => ((r.register == register) && (this.same(r.master, r.port))));
      if (!reg) {
        this.log("receiveRegister(" + register + ") -> register not found");

      } else {
        const unit = this.findUnit(reg.nodeAddress, reg.unitAddress);
        if (unit) {
          unit.value = value;
          this.log("receiveRegister(" + register + ") = " + value);
          // inform the system that a change was detected
          this.system.emitUpdate();

        } else {
          this.log("receiveRegister(" + register + ") -> unit not found " + reg.nodeAddress + "/" + reg.unitAddress);
        }
      }
    }
  }

  receiveHeartbeat(next: Message) {
    this.lastHeartbeat = new Date();
  }

  receiveInfo(next: CommRecord) {
    if (next.message[1] === Rec.DBInfo) {
      this.receiveDBInfo(next.message);

    } else if (next.message[1] === Rec.NodeInfo) {
      this.receiveNodeInfo(next.message);

    } else if (next.message[1] === Rec.UnitInfo) {
      this.receiveUnitInfo(next.message);

    } else {
      this.err("What is this? info type = " + next.message[1] + ", message: " + next.message);
    }
    this.system.triggerRebuild();
  }

  receiveSchedule(message: Message) {
    this.schedule = message[2];
    this.log("received week schedule = " + this.schedule);
  }

  receiveDateTime(message: Message) {
    // 71,0,9,37,3,3,4,3,21,20 -> 09:37:03 Wednesday(3) 4 march 2120
    this.date = new Date( (message[8]-1) * 100 + message[9], message[7]-1, message[6], 
      message[2], message[3], message[4]);
    // this.info("Received date/time: " + this.date);
  }

  receiveDBInfo(message: Message) {
    const dbInfo = Protocol.makeDBInfo(message);
    this.nrNodes = dbInfo.nrNodes;
    for (let nodeInx = 0; nodeInx < this.nrNodes; nodeInx++) {
      this.fetchNode(nodeInx);
    }
  }

  receiveNodeInfo(message: Message) {
    
    const nodeInfo = Protocol.getNodeInfo(message);

    // merge with existing settings
    const conf = this.findNodeConfig(nodeInfo.nodeAddress);
    const nodeConfig = Sanitizers.nodeConfig({...conf, ...nodeInfo});

    // check valid physical address
    if (nodeConfig.nodeAddress == 0xfc) {
      if (nodeConfig.physicalAddress != this.config.physicalAddress) {
        doAlert(this.alertCtrl, { title: "Invalid Master", 
                                  message: this.config.physicalAddress + " is not a valid physical address for the master node: " + nodeConfig.name + " (" + nodeConfig.physicalAddress + ")"});
        // don't add the master node, make this master inactive, delete all nodes
        this.active = false;
        this.config.active = false;
        this.nodes = [];
        return;
      }
    }

    // add the node to the master, if not already there
    let node = this.findNode(nodeInfo.nodeAddress);
    if (!node) {
      node = new Node(this, nodeConfig);
      this.nodes.push(node);
    } else {
      Sanitizers.nodeConfig(nodeConfig, node);
    }
    if (nodeConfig.nodeAddress == 0xfc) {
      // console.log("Master node: " + nodeConfig.name + " - serverport: " + node.serverPort + " -> " + this.config.cport);
      node.serverAddress = node.serverAddress || this.config.caddress || '';
      node.serverPort = asNum(node.serverPort, asNum(this.config.cport, 0));
      node.serverPassword = node.serverPassword || this.config.password || '';
    }

    // add the node to the config
    const inx = this.findNodeConfigIndex(nodeInfo.nodeAddress);
    if (inx >= 0)  {
      this.config.nodes[inx] = nodeConfig;
    } else {
      this.config.nodes.push(nodeConfig);
    }


    //this.system.setActiveState(node);
    this.system.updateMasterConfig(this);

    if (this.onlyThisOne) {
      // fetching info for a single node
      if (node.nodeAddress == this.onlyThisOne)
        this.fetchAllUnits(node);

    } else if (node.nrUnits !== node.units.length) {
      // fetch all units, if not all units already there
      this.fetchAllUnits(node);

    } else {
      this.log("Skipping node: " + node.getDescription() + " - already has " + node.nrUnits + " units");
    }
  }

  receiveUnitInfo(message: Message) {
    const unitInfo = Protocol.getUnitInfo(message);
    //if ((unitInfo.nodeAddress == 3) && (unitAddress == 2)) debugger;

    const node = this.findNode(unitInfo.nodeAddress);
    if (!node) {
      this.err("Node not found: " + unitInfo.nodeAddress);

    } else {

      // update unit
      let unit = this.findUnit(unitInfo.nodeAddress, unitInfo.unitAddress);
      if (!unit) {
        unit = new Unit(node, unitInfo);
        node.units.push(unit);
      } else {
        Sanitizers.unitInfo(unitInfo, unit);
      }
      // this.system.setActiveState(unit);

//TODO: check why no new names??
//TODO !!
// console.log("unitInfo: " + node.name + "/" + unit.name + " = " + unitInfo.name + " (" + unit.type + ": " + unitInfo.unitAddress + ")");

      // update config
      const inxN = this.findNodeConfigIndex(unitInfo.nodeAddress);
      const inxU = this.findUnitConfigIndex(unitInfo.nodeAddress, unitInfo.nodeAddress);

      if (inxN >= 0) {
        if (inxU >= 0) {
          this.config.nodes[inxN].units[inxU] = unitInfo;
        } else {
          this.config.nodes[inxN].units.push(unitInfo);
        }
        this.system.updateMasterConfig(this);


      } else {
        this.err("Node config not found: " + unitInfo.nodeAddress);
      }
    }
  }


  async fetchAllUnits(node: Node) {
    for (let unitInx = 0; unitInx < node.nrUnits; unitInx++) {
      await this.fetchUnit(node, unitInx);
    }
  }

  async fetchOneNode(node: Node) {
    // block the system from getting all nodes/units
    this.log("fetchOneNode: requesting DB info for node " + hex2(node.nodeAddress));
    this.onlyThisOne = node.nodeAddress;
    await this.send(Protocol.buildDBInfo());
  }
  async fetchOnlyNodeInfo() {
    // block the system from getting all nodes/units
    this.log("fetchOnlyNodeInfo: requesting DB info for node " + hex2(this.onlyThisOne));
    this.onlyThisOne = -1;
    await this.send(Protocol.buildDBInfo());
  }

  async fetchDbInfo() {
    try {
      this.onlyThisOne = 0;
      this.log("fetchDbInfo: requesting DB info for all nodes + schedule");
      
      if (!this.isOpen) {
        const errorMsg = `Cannot fetch DB info: socket is not open for ${this.getName()}`;
        this.err(errorMsg);
        doToast(this.toastCtrl, errorMsg, 3000);
        return;
      }
      
      if (!this.isLoggedIn) {
        const errorMsg = `Cannot fetch DB info: not logged in to ${this.getName()}`;
        this.err(errorMsg);
        doToast(this.toastCtrl, errorMsg, 3000);
        return;
      }
      
      await this.send(Protocol.buildDBInfo());
      await this.send(Protocol.buildRequestSchedule());
      doToast(this.toastCtrl, `DB info request sent to ${this.getName()}`, 2000);

    } catch (err) {
      const errorMsg = `DB info request failed for ${this.getName()}: ${err}`;
      this.err(errorMsg);
      doToast(this.toastCtrl, errorMsg, 3000);
    }
  }

  async fetchNode(nodeInx: number) {
    const message = Protocol.buildNodeInfo(nodeInx);
    this.log("fetchNode: requesting node info for node index " + nodeInx);
    try {
      await this.send(message);

    } catch (err) {
      this.err("nodeInfo call failed -> " + err);
    }
  }

  async fetchUnit(node: Node, unitInx: number) {
    const message = Protocol.buildUnitInfo(node, unitInx);
    // this.log("fetchUnit: requesting unit info for node " + hex2(node.nodeAddress) + ", unit index " + unitInx);
    try {
      // unit with index "unitInx" in node "nodeAddress"
      await this.send(message);

    } catch (err) {
      this.err("unitInfo call failed -> " + err);
    }
  }


  makeNode(conf: NodeConfig): Node {
    //TODO: try to find the node in the master.config -> ### Why !!!
    let nodeConfig = this.findNodeConfig(conf.nodeAddress);

    // make new node with default values of with config found in stored info
    const name = (conf.nodeAddress == kVirtualNodeAddress) ? "Virtual Node" : ("Node-" + hex2(conf.nodeAddress));
    const node = new Node(this, 
      Sanitizers.makeNodeConfig({name, type: NodeType.kStandardNode, 
          nodeAddress: conf.nodeAddress, nrUnits: 0, ...nodeConfig})
    );


    this.nodes.push(node);
    this.log("new node: " + node.getName() + " (" + hex2(node.nodeAddress) + ")");
    return node;
  }

  async getDatabase(readDB: boolean = false) {
    this.nodes = [];

    if (readDB) {
      await this.fetchDbInfo();
      // upon reception of the DB info, 
      //   getNode info will be called, 
      //   which in it's turn will trigger getUnitInfo through fetchAllUnits
      
    } else {
      // loop over all nodes/units in the config with a matching ip address
      //  fill: this.nrNodes
      //  call: kind of receive-Node/Unit-Info
      this.log("building db from config file");
      this.config.nodes.forEach(nc => {
        const node = this.findNode(nc.nodeAddress) || this.makeNode(nc);

        nc.units.forEach(uc => {            
          let unit = this.findUnit(uc.nodeAddress, uc.unitAddress);
          if (!unit) {
            unit = new Unit(node, uc);
            node.units.push(unit);
            // this.log("new unit: " + unit.getName() + " (" + hex2(unit.nodeAddress) + "/" + hex2(unit.unitAddress) + "), active: " + unit.active + "/" + uc.active);
          } else {
            this.system.log("** ?? ** getDatabase.findUnit = found -> " + JSON.stringify(unit));
          }
          this.system.setActiveState(unit);
        });
      });
      
      this.system.writeConfig();
      this.system.log("Master " + this.getName() + " created with " + this.nodes.length + " nodes");

      // get my audio configs
      this.system.audio.forEach(r => {
        if (this.same(r.master, r.port))
          this.audio.push(r);
      });

      // get my camera configs
      this.system.cameras.forEach(c => {
        if (this.same(c.master, c.port))
          this.cameras.push(c);
      });

      // get my register configs
      this.system.registers.forEach(c => {
        if (this.same(c.master, c.port))
          this.registers.push(c);
      });

      // get my temperature configs
      this.system.temperatures.forEach(c => {
        if (this.same(c.masterAddress, c.masterPort))
          this.temperatures.push(c);
      });

      //this.system.log("master cameras: " + JSON.stringify(this.cameras, null, 2));
      //this.system.log("master audio: " + JSON.stringify(this.audio, null, 2));
      this.system.log("master registers: ", this.requestRegisters);

      this.system.emitRefresh(System.reasonRebuild);
    }
  }


  /* ****************************************************************************** */
  /* Status requests                                                                */
  /*   Database (all known nodes)                                                   */
  /*   Node (all known units in this node)                                          */
  /*   Unit (only dimmer, switch, input, temperature, motor and mood implemented)   */
  /* ****************************************************************************** */

  receiveStatus(next: CommRecord) {
    // called when an incoming message was received and classified as a status message

    // find node
    const nodeLogical = next.message[2];
    const node = this.nodes.find(node => node && (node.nodeAddress == nodeLogical));
    if (! node) {
      // this.err("status message " + next.cmd + " for unknown node = " + nodeLogical);
      return;
    }

    // find unit
    const unitLogical = next.message[3];
    const unit = node.units.find(unit => unit && (unit.unitAddress == unitLogical));
    if (!unit) {
      // this.err("status message " + next.cmd + " for unknown unit = " + unitLogical +
      //          " in node = " + nodeLogical);
      return;
    }

    // Parse the status into the unit
    if (Protocol.receiveStatus(next, unit)) {
      //this.log("-> received status: " + unit.getDisplayName() + " -> " + unit.getDispayState());
    }

    // inform the system that a change was detected
    this.system.emitUpdate();


    /*
    // inform the system if a change was detected or if macro command (69)
    if ((currValue != unit.value) || (currStatus != unit.status) || (next.cmd == 69))
      this.system.informStatusChange(this, unit);
    */
  }

  receiveAudioStatus(next: CommRecord) {
    const unit = this.findUnit(next.message[2], next.message[3]);
    if (unit) {
      Protocol.receiveAudioStatus(unit, next);

    } else {
      this.err("could not find unit (" + hex(next.message[2]) + ", " + hex(next.message[3]) + ")")
    }
  }

  async requestStatus() {
    this.log("requesting status of all units");
    for (let nodeInx = 0; nodeInx < this.nodes.length; nodeInx++) {
      const node = this.nodes[nodeInx];
      for (let unitInx = 0; unitInx < node.units.length; unitInx++) {
        await this.requestUnitStatus(node.units[unitInx]);
      }
      
    }
    this.log("requesting week schedule");
    await this.send(Protocol.buildRequestSchedule());
  }

  async requestNodeStatus(node: Node) {
    for (let unitInx = 0; unitInx < node.units.length; unitInx++) {
      if (node.units[unitInx].active) {
        await this.requestUnitStatus(node.units[unitInx]);
      }
    }
  }

  async requestUnitStatus(unit: Unit) {
    if ((unit.node.nodeAddress === kVirtualNodeAddress) && unit.isRegister()) {
      // console.log("request register value of " + unit.node.getName() + "-" + unit.getName() + ", nr: " + unit.register + " (protocol=" + this.protocol + ")");
      if (this.protocol >= 209) {
        if (typeof unit.register != "undefined") {
          await this.send(Protocol.buildRegister(unit.register));
        } else {
          this.err("Register unit without register nr...")
        }
      } else {
        this.err("FC_REGISTERMAP not available before protocol v2.09, master = v" + this.protocol);
      }

    } else if (unit.isExtendedTemperature()) {
      // console.log("get extended temp status of " + unit.node.getName() + "-" + unit.getName());
      // Ask any, we'll get all 4 methods back
      await this.send(Protocol.buildRequestSensStatus(unit.node, unit, setSensorMethods.swingangle));

    } else {
      //console.log("get unit status of " + unit.node.getName() + "-" + unit.getName());
      await this.send(Protocol.buildRequestUnitStatus(unit.node, unit));
      // results will be set by the data event listener
    }
  }
  async setUnitStatus(unit: Unit, value: number) {
    if (unit.isRegister()) {
      if (this.protocol >= 209) {
        await this.send(Protocol.buildRegister(unit.register, value));
        this.log("set register: " + unit.register + ", to: " + value);
      } else {
        this.err("FC_REGISTERMAP not available before protocol v2.09, master = v" + this.protocol);
      }

    } else {
      const params = Protocol.buildSetCmd(unit.node, unit, value);
      if (params.cmd) {
        this.log("set unit status of " + unit.node.getName() + "-" + unit.getName() + " to " + value);
        await this.send(params.message);
        if (unit.isDimmer() && (typeof value == "boolean") && (value)) {
          // dimmer can have its value changed while we were offline
          await this.requestUnitStatus(unit);
        }
      }
    }
  }


  async setPreset(unit: Unit, preset: number, temp: number) {
    await this.send(Protocol.buildSetPreset(unit.node, unit, preset, temp));
    this.log("set temp preset: " + preset + " of " + unit.node.getName() + "-" + unit.getName() + " to temp " + temp);
  }

  async selectPreset(unit: Unit, preset: number) {
    await this.send(Protocol.buildSelectPreset(unit.node, unit, preset));
    this.log("set temp preset of " + unit.node.getName() + "-" + unit.getName() + " to: " + preset);
  }

  async setSchedule() {
    await this.send(Protocol.buildSendSchedule(this.schedule));
    this.log("set schedule to week nr: " + this.schedule);
  }

  async setDateTime() {
    const now = new Date();
    await this.send(Protocol.buildSendDateTime(now));
    this.log("set datetime to: " + now);
  }

  async setTempOnOff(unit: Unit, on: boolean) {
    await this.send(Protocol.buildSensorOnOff(unit.node, unit, on));
    this.log("turn temp sensor of " + unit.node.getName() + "-" + unit.getName() + ": " + (on ? "on" : "off"));
  }

  async doIncDecPreset(unit: Unit, inc: boolean) {
    await this.send(Protocol.buildIncDecPreset(unit.node, unit, inc));
    this.log("set temp preset of " + unit.node.getName() + "-" + unit.getName() + ": " + (inc ? "up" : "down"));
  }

  async doHVAC(unit: Unit, what: TemperatureControlMode, value: number) {
    await this.send(Protocol.buildHVAC(unit.node, unit, what, value));
    this.log("set HAVC of " + unit.node.getName() + "-" + unit.getName() + ": " + what + " = " + value);
  }

  async setAudioSrcDest(unit: Unit, source: number, destination: number) {
    await this.send(Protocol.buildAudioSrcDest(unit.node, unit, source, destination));
    this.log("send source " + source + " to destination " + destination + " of " + unit.getName());
  }
  async sendAudioAction(unit: Unit, action: string, fnc?: string) {
    await this.send(Protocol.buildAudioAction(unit.node, unit, action, fnc));
    this.log("send action " + action + " to " + unit.getName());
  }

  //////////////////
  // Virtual Node //
  //////////////////
  addVirtualNode(): { node: Node, isNew: boolean } {
    let node = this.findNode(kVirtualNodeAddress);
    if (!node) {
      // add the node to master
      node = new Node(this, {
        name: "Virtual Node", type: NodeType.kNoNode,
        nrUnits: 0,
        nodeAddress: kVirtualNodeAddress,
        physicalAddress: 0
      });
      this.nodes.push(node);
      this.log("Added virtual node " + node.getNumber());
      return { node, isNew: true };
    }
    return { node, isNew: false };
  }
  
  makeVirtualUnits(forUnits: Array<CameraConfig | AudioConfig | RegisterConfig>, type?: UnitType) {
    let needsUpdate = false;
    forUnits.forEach(uc => {
      if (uc.nodeAddress == kVirtualNodeAddress) {
        let { node, isNew } = this.addVirtualNode();
        needsUpdate = isNew;
        
        if (!this.findUnit(kVirtualNodeAddress, uc.unitAddress)) {
          // add unit to node
          const unit = new Unit(node, {
            name: uc.name, type: type || uc.type, active: uc.active,
            nodeAddress: kVirtualNodeAddress, unitAddress: uc.unitAddress,
            masterAddress: this.config.address, masterPort: this.config.port
          });
          // unit.active = true;
          node.units.push(unit);
          node.nrUnits++;  // we never get a database info for this node/unit, so increment ourself
          this.log("Added virtual unit: " + unit.name + " = " + unit.getNumber());
          needsUpdate = true;
        }
      }
    });
    if (needsUpdate) this.system.updateMasterConfig(this);
  }

  removeVirtualUnit(vUnit: CameraConfig | AudioConfig | RegisterConfig) {
    let node = this.findNode(kVirtualNodeAddress);
    if (node) {
      let unit = this.findUnit(kVirtualNodeAddress, vUnit.unitAddress);
      if (unit) {
        node.units = node.units.filter(u => u !== unit);
        node.nrUnits--;
        this.system.updateMasterConfig(this);
      }
    }
  }


  ///////////
  // Audio //
  ///////////
  async requestAudio() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      const audio = await this.ipMaster.getAudio(node);

      // add in master address & port, you never know that the audio configs get consolidated
      this.audio = audio.map(conf => { 
        this.log("adding room: " + conf.name);

        return {
          id: conf.id, name: conf.name, active: true,
          nodeAddress: conf.nodeAddress ?? conf.nodeaddress, 
          unitAddress: conf.unitAddress ?? conf.unitaddress,
          master: this.getAddress(), port: this.getPort(),
          type: UnitType.kAV,
          destination: 0,
          destinations: conf.destinations.map(destination => { 
            // console.log("Adding audio: " + conf.name + " - " + destination.name +
            //             " (" + (conf.nodeAddress ?? conf.nodeaddress) + "/" + (conf.unitAddress ?? conf.unitaddress) + 
            //             ") @ " + this.getAddress() + ":" + this.getPort() + 
            //             ", max=" + destination.maxvolume);
            return {
              id: destination.id, // + (destination.location ?? 0) * 1000,
              nr: destination.destinationnr,
              nodeAddress: destination.nodeAddress ?? destination.nodeaddress, 
              unitAddress: destination.unitAddress ?? destination.unitaddress,
              master: this.getAddress(),
              port: this.getPort(),
              name: destination.name,
              maxvolume: destination.maxvolume,
              type: destination.type,
              basictype: BasicAudioType.Basic,
              volume: 0,  // current volume
              source: 0,  // current source
              sources: destination.sources.map(source => { return {
                id: source.id,
                nr: source.sourcenr,
                name: source.name
              }})
            }
          })
        }
      });

      doToast(this.toastCtrl, "Received " + this.audio.length + " audio config");
      this.system.addAudio(this);
      this.system.triggerRebuild(true);
    }
  }


  attachAudio() {
    this.makeVirtualUnits(this.audio, UnitType.kAV);
    
    // attach the audio settings to the units
    this.log("Attaching audio for master: " + this.getName() + ", audio units: " + JSON.stringify(this.audio, null, 2));
    this.forAllUnits(s => {
      if (s.isMedia()) {
        const audio = this.audio.find(ac => s.isUnit(ac.master, ac.port, ac.nodeAddress, ac.unitAddress ?? ac.id));
        if (audio) { 
          this.log("Audio unit " + s.name + " attached to room: " + audio.name);
          s.destinations = audio.destinations;
          s.destination = null;
          s.destinationId = 255;
          s.destinationNr = 255;
          s.active = audio.active;
        } else {
          this.log("Audio unit " + s.name + "(" + s.getNumber() + ") not found !!");
        }
      }
    }); 
    // fake audio stuff for basic audio units (if not already done)
    this.allActiveUnits(UnitType.kAudio).forEach(unit => {
      if (! unit.destinations) {
        unit.destinations = [];
      }
      if (! unit.destinations?.length) {
        unit.destinations.push({id: 0, nr: 0, type: 0,
          nodeAddress: unit.node.nodeAddress,
          unitAddress: unit.unitAddress,
          master: unit.node.master.getAddress(),
          port: unit.node.master.getPort(),
          name: "Audio",
          maxvolume: 100,
          volume: 0,  // current volume
          source: 0,  // current source
          basictype: BasicAudioType.Basic, // will be adjust by status messages
          sources: BasicAudioSources[BasicAudioType.Basic],  // source will be changed if basictype changes
          disc: 0, track: 0, frequency: "-", preset: 0});
      }
      unit.destinationId = 0;
      unit.destinationNr = 0;
      unit.destination = unit.destinations[0];
      this.log("*****  Basic audio (destination != null) = " + (unit.destination != null) + " of unit " + unit.name);
    });
  }



  //////////////
  // Camera's //
  //////////////
  async requestCameras() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      const cams = await this.ipMaster.getCameras(node);

      this.cameras = cams.map(c => { return {
        ...c,
        // cam_url: ((c.cam_url.indexOf("@") == -1) && (c.cam_userid)) ? c.cam_url.replace("://", "://"+c.cam_userid+":"+c.cam_pwd+"@") : c.cam_url,
        type: UnitType.kCamera, nodeAddress: 0xff, unitAddress: 0x100+c.id,
        master: this.getAddress(), port: this.getPort()
      }});

      this.system.addCameras(this);
      this.system.triggerRebuild(true);
    }
  }

  attachCameras() {
    this.log("Attaching cameras for master: " + this.getName())
    this.makeVirtualUnits(this.cameras, UnitType.kCamera);
    this.forAllUnits(u => {
      if (u.type === UnitType.kCamera) {
        const camera = this.cameras.find(camera => u.isUnit(camera.master, camera.port, camera.nodeAddress, camera.unitAddress));
        if (camera) { 
          this.system.log("Camera " + u.name + " with url: " + camera.cam_url);
          u.url = camera.cam_url;
          u.pwd = camera.cam_pwd;
          u.user = camera.cam_userid;
        } else {
          this.system.log("Camera " + u.name + " not found !!");
        }
      }
    });
  }

  ///////////////
  // Registers //
  ///////////////
  async requestRegisters() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      const regs = await this.ipMaster.getRegisters(node);

      this.registers = regs.map(c => { return {
        ...c,
        type: UnitType.kRegister, nodeAddress: 0xff, unitAddress: 0x0200+c.register,
        master: this.getAddress(), port: this.getPort()
      }});

      // console.log("Master.requestRegisters -> Registers of " + this.getName(), this.registers);
      this.system.addRegisters(this);
      this.system.triggerRebuild(true);
    }
  }


  attachRegisters() {
    this.log("Attaching registers for master: " + this.getName())
    this.makeVirtualUnits(this.registers, UnitType.kRegister);

    this.forAllUnits(u => {
      if (u.type === UnitType.kRegister) {
        const register = this.registers.find(register => u.isUnit(register.master, register.port, register.nodeAddress, register.unitAddress));
        if (register) { 
          this.system.log("Master.attachRegisters -> update register " + u.name + " with nr: " + register.register);
          u.register = register.register;
          u.icon = register.icon;
          u.divider = register.divider || 1;
          u.multiplier = register.multiplier || 1;
          u.decimals = register.decimals;
          u.measure = register.measure;
          this.requestUnitStatus(u);
        } else {
          this.system.log("Master.attachRegisters -> register " + u.name + " not found !!");
        }
      }
    });
  }

  ///////////
  // rooms //
  ///////////
  async requestRooms(dontFavorites: boolean = false, dontMoods: boolean = false) {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      doToast(this.toastCtrl, "Requesting rooms - " + this.getName());
      const roomConfig = await this.ipMaster.getRooms(node, this.getAddress(), this.getPort())
      this.system.setRooms(this, roomConfig, dontFavorites, dontMoods);
    }
  }

  async requestDevices() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      doToast(this.toastCtrl, "Requesting devices - " + this.getName());
      this.system.setDevices(this, await this.ipMaster.getDevices(node, this.getAddress(), this.getPort()));
      this.system.writeDevices();
      // this.system.updateMasterConfig(master);       
    }
  }

  async requestBackgroundImages() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");
      return;
    }

    const toast = await this.toastCtrl.create({
      message: 'Getting room background images', position: 'top'
    });
    toast.present();

    try {
      // Fetch rooms directly from LCD
      const lcdRooms = await this.ipMaster.getRooms(node, this.getAddress(), this.getPort());
      
      console.log(`Fetching ${lcdRooms.length} background images from LCD rooms`);

      const prefix = this.system.getPrefix();
      let successCount = 0;
      
      // Loop over LCD rooms and fetch their background images
      // Note: room[0] (favorites) doesn't exist in LCD, so it won't be touched
      // Manually added rooms also won't be in lcdRooms, so they're safe too
      for (let i = 0; i < lcdRooms.length; i++) {
        const lcdRoom = lcdRooms[i];
        toast.message = `Reading background ${i + 1}/${lcdRooms.length}`;
        
        try {
          // Fetch image using the picture ID (which equals roomnr on LCD)
          const image = await this.ipMaster.readImage(node, lcdRoom.picture, PictureKind.kBackground, "width=600", "jpg");
          
          if (image) {
            // Extract extension from the image name
            const extension = image.name.split('.').pop() || 'jpg';
            
            // Actually, we need to save as picture_${room.id} because loadRoomPictures uses room.id!
            // But the CONTENT should be from lcdRoom.picture (which we're already fetching correctly)
            const fileName = `picture_${lcdRoom.id}.${extension}`;
            const base64Data = image.url.split(',')[1] || image.url;
            
            await Filesystem.writeFile({
              path: prefix + fileName,
              data: base64Data,
              directory: Directory.Data
            });
            
            // Find the corresponding room in this.system.rooms by id and update extension
            const systemRoom = this.system.rooms.find(r => r.id === lcdRoom.id);
            if (systemRoom) {
              systemRoom.extension = extension;
              systemRoom.pictureUrl = image.url;
              console.log(`  ✓ Room ${lcdRoom.id} "${lcdRoom.name}": saved ${fileName} (from LCD picture ${lcdRoom.picture})`);
              successCount++;
            } else {
              console.warn(`  ⚠ Room ${lcdRoom.id} "${lcdRoom.name}": saved ${fileName} but room not found in system.rooms`);
            }
          }
        } catch (err) {
          console.error(`  ✗ Room ${lcdRoom.id} "${lcdRoom.name}": failed - ${err.message}`);
        }
      }
      
      console.log(`Successfully saved ${successCount}/${lcdRooms.length} room background images`);
      
      // Save updated room config
      if (successCount > 0) {
        this.system.writeRooms();
      }
    } catch(e) {
      console.error("Error fetching background images:", e);
    }
    
    toast.dismiss();
  }

  async requestMoodTiles() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");
      return;
    }

    const toast = await this.toastCtrl.create({
      message: 'Getting mood tile images', position: 'top'
    });
    toast.present();

    try {
      // Get all tiles (not just active ones - user might want to download icons before configuring rooms)
      const activeTiles = this.system.tiles;
      
      if (activeTiles.length === 0) {
        toast.dismiss();
        doToast(this.toastCtrl, 'No mood tiles found. Please load "Home Tiles" first to get the tile definitions from the master.', 1500);
        return;
      }
      
      console.log(`Fetching ${activeTiles.length} mood tile icons (ON + OFF) - Total tiles: ${this.system.tiles.length}`);

      const prefix = this.system.getPrefix();
      
      // Ensure mood directory exists before writing files
      await this.system.pictureUtils.assertDirs(prefix);
      
      let successCount = 0;
      const total = activeTiles.length * 2; // ON + OFF for each tile
      let count = 0;

      for (const tile of activeTiles) {
        // Fetch and save OFF icon
        count++;
        toast.message = `Reading mood icon ${count}/${total}`;
        
        try {
          const imageOff = await this.ipMaster.readImage(node, tile.id, PictureKind.kMoodOff, "", "png");
          if (imageOff) {
            const extension = imageOff.name.split('.').pop() || 'png';
            const fileName = `mood/off_${tile.id}.${extension}`;
            const base64Data = imageOff.url.split(',')[1] || imageOff.url;
            
            await Filesystem.writeFile({
              path: prefix + fileName,
              data: base64Data,
              directory: Directory.Data
            });
            
            tile.extensionOff = extension;
            tile.iconOffUrl = imageOff.url;
            console.log(`  ✓ Tile ${tile.id} "${tile.name}": saved ${fileName} (OFF)`);
            successCount++;
          }
        } catch (err) {
          console.error(`  ✗ Tile ${tile.id} "${tile.name}": failed to save OFF icon - ${err.message}`);
        }

        // Fetch and save ON icon
        count++;
        toast.message = `Reading mood icon ${count}/${total}`;
        
        try {
          const imageOn = await this.ipMaster.readImage(node, tile.id, PictureKind.kMoodOn, "", "png");
          if (imageOn) {
            const extension = imageOn.name.split('.').pop() || 'png';
            const fileName = `mood/on_${tile.id}.${extension}`;
            const base64Data = imageOn.url.split(',')[1] || imageOn.url;
            
            await Filesystem.writeFile({
              path: prefix + fileName,
              data: base64Data,
              directory: Directory.Data
            });
            
            tile.extensionOn = extension;
            tile.iconOnUrl = imageOn.url;
            console.log(`  ✓ Tile ${tile.id} "${tile.name}": saved ${fileName} (ON)`);
            successCount++;
          }
        } catch (err) {
          console.error(`  ✗ Tile ${tile.id} "${tile.name}": failed to save ON icon - ${err.message}`);
        }
      }
      
      console.log(`Successfully saved ${successCount}/${total} mood tile icons`);
      
      // Save updated tile config
      if (successCount > 0) {
        this.system.writeTiles();
      }
    } catch(e) {
      console.error("Error fetching mood tile images:", e);
    }
    
    toast.dismiss();
  }

  async requestHomeTiles() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      doToast(this.toastCtrl, "Requesting home tiles - " + this.getName());
      this.system.setTiles(this, await this.ipMaster.getHomeTiles(node, this.getAddress(), this.getPort()));
   }
  }

  /////////////////////////
  // Temperatures / HVAC //
  /////////////////////////
  async requestTemperatures() {
    const node = this.findMasterNode();
    if (!node) {
      doToast(this.toastCtrl, "Couldn't find a master node");

    } else {
      doToast(this.toastCtrl, "Requesting temperature - " + this.getName());
      this.system.setTemperatures(this, await this.ipMaster.getTemperatures(node, this.getAddress(), this.getPort()));
   }
  }
  
  attachTemperatures() {
    this.log("Attaching temperatures for master: " + this.getName())
    this.temperatures.forEach(t => {
      const unit = this.findUnit(t.nodeAddress, t.unitAddress);
      if (unit) {
        //fill in the workingmode/fanspeed etc from the TemperatureTypes from the TemperatureConfig's
        unit.workingmode = t.workingmode;
        unit.fanspeed = t.fanspeed;
        unit.swingangle = t.swingangle;
        unit.swingmode = t.swingmode;

        unit.workingmodes = t.workingmodes;
        unit.fanspeeds = t.fanspeeds;
        unit.swingangles = t.swingangles;
        unit.swingmodes = t.swingmodes;

        // console.log("added hvac to unit: " + unit.name, unit);
      }
    });
  }

  ///////////////
  // downloads //
  ///////////////

  // Bij factory-reset uit node(fc)
  // 1. lezen van nodes.json -> IP addressen van alle LCD's (moods en beelden) en Gateways (moods)
  // 2. lezen van scrmap_0-5.json -> units die we aanvinken in de master als "used" + welke we onder toestellen zetten
  // 3. lezen van backgrounds via graphics API en lokaal stockeren
  // 4. lezen van rooms.json -> rooms aanmaken, photo's linken
  // 5. lezen van cameras.json
  // 6. lezen van registers.json
  // 7. lezen van audio.json
  // 8. lezen van hvac en hvactypes.json

  // tijdens normale werking uit gateways en lcd
  // - lezen van moods uit gateway / edit / schrijven
  // - lezen van beelden uit gateway of lcd / delete /edit / crop /schrijven

  // todo: onder toestellen 6 tabs maken (scrmap0-5)
  // mapping config / array aanmaken.

}
