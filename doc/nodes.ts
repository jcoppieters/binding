import {NodeInfo} from "../system/types";
import {UnitInfo} from "./unit";

export interface NodeConfig extends NodeInfo {
  serverAddress?: string;      // next 2 only for GUI and Gateway nodes.
  serverPort?: number;
  serverPassword?: string;
  units?: Array<UnitInfo>;
}