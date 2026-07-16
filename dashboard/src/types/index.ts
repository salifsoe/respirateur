/**
 * Ventilator data types
 */

export type VentilationMode = 'VCV' | 'PCV' | 'VAC';

export type AlarmSeverity = 'info' | 'warning' | 'critical';

export interface VentilatorData {
  pressure: number;
  flow: number;
  volume: number;
  fr: number;
  peep: number;
  mode: VentilationMode;
  alarm: boolean;
  battery: number;
  wifi: boolean;
}

export interface VentilatorSettings {
  mode: VentilationMode;
  fr: number;
  volume: number;
  peep: number;
  ie_ratio: number;
}

export interface Alarm {
  id: string;
  message: string;
  time: string;
  severity: AlarmSeverity;
  active: boolean;
}

export interface HistoryEvent {
  id: string;
  date: string;
  time: string;
  event: string;
}

export interface GraphDataPoint {
  time: number;
  value: number;
}
