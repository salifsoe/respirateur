/**
 * MQTT Service
 * Handles MQTT connection, subscription, and publishing
 */

import mqtt, { type IClientOptions, type MqttClient } from 'mqtt';
import type { VentilatorData, VentilatorSettings } from '../types';

const DEFAULT_MQTT_BROKER_URL = 'wss://broker.hivemq.com:8884/mqtt';
const MQTT_USERNAME = '';
const MQTT_PASSWORD = '';
const DATA_TOPIC = 'respirateur-medvent/data';
const CONFIG_TOPIC = 'respirateur-medvent/config';

class MQTTService {
  private client: MqttClient | null = null;
  private brokerUrl: string = import.meta.env.VITE_MQTT_BROKER_URL ?? DEFAULT_MQTT_BROKER_URL;
  private options: IClientOptions = {
    clientId: 'ventilator-dashboard-' + Math.random().toString(16).substr(2, 8),
    clean: true,
    connectTimeout: 4000,
    reconnectPeriod: 1000,
  };
  private dataCallback: ((data: VentilatorData) => void) | null = null;

  /**
   * Connect to MQTT broker
   */
  connect(brokerUrl?: string): Promise<void> {
    return new Promise((resolve, reject) => {
      if (brokerUrl) {
        this.brokerUrl = brokerUrl;
      }

      this.options.username = import.meta.env.VITE_MQTT_USERNAME || MQTT_USERNAME || undefined;
      this.options.password = import.meta.env.VITE_MQTT_PASSWORD || MQTT_PASSWORD || undefined;
      this.client = mqtt.connect(this.brokerUrl, this.options);

      this.client.on('connect', () => {
        console.log('MQTT Connected');
        resolve();
      });

      this.client.on('error', (err: Error) => {
        console.error('MQTT Error:', err);
        reject(err);
      });

      this.client.on('message', (topic: string, message: Buffer) => {
        if (topic === DATA_TOPIC && this.dataCallback) {
          try {
            const data: VentilatorData = JSON.parse(message.toString());
            this.dataCallback(data);
          } catch (error) {
            console.error('Error parsing MQTT message:', error);
          }
        }
      });

      this.client.on('offline', () => {
        console.log('MQTT Client offline');
      });

      this.client.on('reconnect', () => {
        console.log('MQTT Reconnecting');
      });
    });
  }

  /**
   * Disconnect from MQTT broker
   */
  disconnect(): void {
    if (this.client) {
      this.client.end();
      this.client = null;
    }
  }

  /**
   * Subscribe to ventilator data topic
   */
  subscribeToData(callback: (data: VentilatorData) => void): void {
    this.dataCallback = callback;
    if (this.client) {
      this.client.subscribe(DATA_TOPIC, (err: Error | null) => {
        if (err) {
          console.error('Subscription error:', err);
        } else {
          console.log(`Subscribed to ${DATA_TOPIC}`);
        }
      });
    }
  }

  /**
   * Publish ventilator settings
   */
  publishSettings(settings: VentilatorSettings): void {
    if (this.client) {
      this.client.publish(
        CONFIG_TOPIC,
        JSON.stringify(settings),
        { qos: 1 },
        (err?: Error) => {
          if (err) {
            console.error('Publish error:', err);
          } else {
            console.log('Settings published');
          }
        }
      );
    }
  }

  /**
   * Check if connected
   */
  isConnected(): boolean {
    return this.client !== null && this.client.connected;
  }
}

// Export singleton instance
export const mqttService = new MQTTService();
