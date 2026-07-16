/**
 * Custom hook for managing ventilator data
 */

import { useState, useEffect } from 'react';
import type { VentilatorData } from '../types';
import { mqttService } from '../services/mqtt.service';
import { mockMqttService } from '../services/mockMqtt.service';

interface UseVentilatorDataProps {
  useMock?: boolean;
}

export const useVentilatorData = ({ useMock = true }: UseVentilatorDataProps = {}) => {
  const [data, setData] = useState<VentilatorData>({
    pressure: 0,
    flow: 0,
    volume: 0,
    fr: 15,
    peep: 5,
    mode: 'VCV',
    alarm: false,
    battery: 100,
    wifi: false,
  });
  const [connected, setConnected] = useState(false);
  const [isMock, setIsMock] = useState(useMock);

  useEffect(() => {
    const service = isMock ? mockMqttService : mqttService;

    const connectAndSubscribe = async () => {
      try {
        await service.connect();
        setConnected(true);
        
        service.subscribeToData((newData) => {
          setData(newData);
        });
      } catch (error) {
        console.error('Connection error:', error);
        setConnected(false);
      }
    };

    connectAndSubscribe();

    return () => {
      service.disconnect();
      setConnected(false);
    };
  }, [isMock]);

  return { data, connected, isMock, setIsMock };
};
