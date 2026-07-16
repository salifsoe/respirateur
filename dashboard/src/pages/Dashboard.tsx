/**
 * Dashboard page
 * Main dashboard with all metric cards and real-time graphs
 */

import React, { useState, useEffect } from 'react';
import { FaWifi, FaBatteryThreeQuarters, FaBatteryHalf, FaBatteryQuarter, FaBatteryEmpty } from 'react-icons/fa';
import MetricCard from '../components/MetricCard';
import RealTimeChart from '../components/RealTimeChart';
import { useVentilatorData } from '../hooks/useVentilatorData';
import type { GraphDataPoint } from '../types';

const Dashboard: React.FC = () => {
  const { data, connected, isMock, setIsMock } = useVentilatorData({ useMock: true });
  
  // Graph data buffers (100 points each)
  const [pressureData, setPressureData] = useState<GraphDataPoint[]>([]);
  const [flowData, setFlowData] = useState<GraphDataPoint[]>([]);
  const [volumeData, setVolumeData] = useState<GraphDataPoint[]>([]);

  // Update graph data when new data arrives
  useEffect(() => {
    const now = Date.now();
    
    setPressureData(prev => {
      const newPoint = { time: now, value: data.pressure };
      const updated = [...prev, newPoint];
      return updated.slice(-100);
    });

    setFlowData(prev => {
      const newPoint = { time: now, value: data.flow };
      const updated = [...prev, newPoint];
      return updated.slice(-100);
    });

    setVolumeData(prev => {
      const newPoint = { time: now, value: data.volume };
      const updated = [...prev, newPoint];
      return updated.slice(-100);
    });
  }, [data]);

  const getBatteryIcon = () => {
    if (data.battery > 75) return <FaBatteryThreeQuarters />;
    if (data.battery > 50) return <FaBatteryHalf />;
    if (data.battery > 25) return <FaBatteryQuarter />;
    return <FaBatteryEmpty />;
  };

  const getBatteryColor = () => {
    if (data.battery > 50) return '#00ff00';
    if (data.battery > 25) return '#ffaa00';
    return '#ff0000';
  };

  const containerStyle: React.CSSProperties = {
    padding: '1.5rem',
  };

  const spaceStyle: React.CSSProperties = {
    display: 'flex',
    flexDirection: 'column' as const,
    gap: '1.5rem',
  };

  const headerStyle: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
  };

  const headerTitleStyle: React.CSSProperties = {
    fontSize: '1.875rem',
    fontWeight: 'bold',
    color: '#ffffff',
  };

  const headerSubtitleStyle: React.CSSProperties = {
    color: '#9ca3af',
    marginTop: '0.25rem',
  };

  const statusBadgeStyle: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    backgroundColor: '#16213e',
    padding: '0.5rem 1rem',
    borderRadius: '0.5rem',
  };

  const buttonStyle: React.CSSProperties = {
    backgroundColor: '#0f3460',
    color: '#ffffff',
    padding: '0.5rem 1rem',
    borderRadius: '0.5rem',
    fontSize: '0.875rem',
    border: 'none',
    cursor: 'pointer',
  };

  const statusBarStyle: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    gap: '1rem',
    backgroundColor: '#16213e',
    padding: '1rem',
    borderRadius: '0.5rem',
  };

  const metricGridStyle: React.CSSProperties = {
    display: 'grid',
    gridTemplateColumns: 'repeat(2, 1fr)',
    gap: '1rem',
  };

  const graphGridStyle: React.CSSProperties = {
    display: 'grid',
    gridTemplateColumns: 'repeat(1, 1fr)',
    gap: '1rem',
  };

  return (
    <div style={{ ...containerStyle, ...spaceStyle }}>
      {/* Header */}
      <div style={headerStyle}>
        <div>
          <h1 style={headerTitleStyle}>MED-VENT Tableau de bord</h1>
          <p style={headerSubtitleStyle}>Système de surveillance de respirateur artificiel</p>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '1rem' }}>
          {/* Connection Status */}
          <div style={statusBadgeStyle}>
            <div style={{
              width: '0.75rem',
              height: '0.75rem',
              borderRadius: '50%',
              backgroundColor: connected ? '#00ff00' : '#ff0000',
            }} />
            <span style={{ fontSize: '0.875rem', color: '#d1d5db' }}>
              {isMock ? 'Mode Simulation' : connected ? 'Connecté' : 'Déconnecté'}
            </span>
          </div>
          
          {/* Toggle Mock/Real */}
          <button
            onClick={() => setIsMock(!isMock)}
            style={buttonStyle}
          >
            {isMock ? 'Utiliser MQTT Réel' : 'Utiliser MQTT Simulation'}
          </button>
        </div>
      </div>

      {/* Status Bar */}
      <div style={statusBarStyle}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          <FaWifi style={{ color: data.wifi ? '#00ff00' : '#6b7280' }} />
          <span style={{ fontSize: '0.875rem', color: '#d1d5db' }}>WiFi</span>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          <span style={{ color: getBatteryColor() }}>{getBatteryIcon()}</span>
          <span style={{ fontSize: '0.875rem', color: '#d1d5db' }}>{Math.round(data.battery)}%</span>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem', marginLeft: 'auto' }}>
          <div style={{
            width: '0.75rem',
            height: '0.75rem',
            borderRadius: '50%',
            backgroundColor: data.alarm ? '#ff0000' : '#00ff00',
            animation: data.alarm ? 'pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite' : 'none',
          }} />
          <span style={{ fontSize: '0.875rem', color: '#d1d5db' }}>
            {data.alarm ? 'ALARME' : 'Normal'}
          </span>
        </div>
      </div>

      {/* Metric Cards */}
      <div style={metricGridStyle}>
        <MetricCard
          title="Mode"
          value={data.mode}
          status={data.alarm ? 'danger' : 'normal'}
        />
        <MetricCard
          title="Pression"
          value={data.pressure.toFixed(1)}
          unit="cmH₂O"
          status={data.pressure > 30 ? 'danger' : data.pressure > 25 ? 'warning' : 'normal'}
        />
        <MetricCard
          title="Volume"
          value={Math.round(data.volume)}
          unit="mL"
          status={data.volume > 700 ? 'warning' : 'normal'}
        />
        <MetricCard
          title="Débit"
          value={data.flow.toFixed(1)}
          unit="L/min"
          status={Math.abs(data.flow) > 50 ? 'warning' : 'normal'}
        />
        <MetricCard
          title="FR"
          value={Math.round(data.fr)}
          unit="/min"
          status={data.fr > 30 ? 'warning' : 'normal'}
        />
        <MetricCard
          title="PEEP"
          value={data.peep.toFixed(1)}
          unit="cmH₂O"
          status={data.peep > 10 ? 'warning' : 'normal'}
        />
        <MetricCard
          title="I:E"
          value="1:2"
          status="normal"
        />
      </div>

      {/* Real-time Graphs */}
      <div style={graphGridStyle}>
        <RealTimeChart
          data={pressureData}
          color="#00d4ff"
          title="Pressure"
          unit="cmH₂O"
          yDomain={[0, 40]}
        />
        <RealTimeChart
          data={flowData}
          color="#00ff00"
          title="Flow"
          unit="L/min"
          yDomain={[-60, 60]}
        />
        <RealTimeChart
          data={volumeData}
          color="#ffaa00"
          title="Volume"
          unit="mL"
          yDomain={[0, 800]}
        />
      </div>
    </div>
  );
};

export default Dashboard;
