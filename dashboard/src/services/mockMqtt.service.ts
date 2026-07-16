/**
 * Mock MQTT Service
 * Simulates MQTT data for testing without ESP32
 */

import type { VentilatorData } from '../types';

class MockMQTTService {
  private intervalId: NodeJS.Timeout | null = null;
  private dataCallback: ((data: VentilatorData) => void) | null = null;
  private connected: boolean = false;

  /**
   * Connect to mock MQTT broker
   */
  connect(): Promise<void> {
    return new Promise((resolve) => {
      setTimeout(() => {
        this.connected = true;
        console.log('Mock MQTT Connected');
        resolve();
      }, 500);
    });
  }

  /**
   * Disconnect from mock MQTT broker
   */
  disconnect(): void {
    if (this.intervalId) {
      clearInterval(this.intervalId);
      this.intervalId = null;
    }
    this.connected = false;
    console.log('Mock MQTT Disconnected');
  }

  /**
   * Subscribe to mock ventilator data
   */
  subscribeToData(callback: (data: VentilatorData) => void): void {
    this.dataCallback = callback;
    
    if (this.intervalId) {
      clearInterval(this.intervalId);
    }

    // Generate mock data every 100ms
    this.intervalId = setInterval(() => {
      const data = this.generateMockData();
      if (this.dataCallback) {
        this.dataCallback(data);
      }
    }, 100);

    console.log('Mock MQTT: Subscribed to ventilator/data');
  }

  /**
   * Generate mock ventilator data
   */
  private generateMockData(): VentilatorData {
    const time = Date.now();
    const cycle = (time % 4000) / 4000; // 4-second breathing cycle

    // Simulate pressure waveform
    const pressure = 5 + 20 * Math.sin(cycle * Math.PI * 2) * (cycle < 0.5 ? 1 : 0.3);
    
    // Simulate flow waveform
    const flow = 60 * Math.sin(cycle * Math.PI * 2) * (cycle < 0.3 ? 1 : -0.5);
    
    // Simulate volume (integral of flow)
    const volume = 500 * Math.sin(cycle * Math.PI) * (cycle < 0.5 ? 1 : 0);

    return {
      pressure: Math.max(0, pressure),
      flow,
      volume: Math.max(0, volume),
      fr: 15,
      peep: 5,
      mode: 'VCV',
      alarm: pressure > 30,
      battery: 85 + Math.random() * 15,
      wifi: true,
    };
  }

  /**
   * Mock publish settings (no-op)
   */
  publishSettings(): void {
    console.log('Mock MQTT: Settings published (no-op)');
  }

  /**
   * Check if connected
   */
  isConnected(): boolean {
    return this.connected;
  }
}

// Export singleton instance
export const mockMqttService = new MockMQTTService();
