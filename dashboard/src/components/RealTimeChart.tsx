/**
 * RealTimeChart component
 * Displays real-time data using Recharts
 */

import React from 'react';
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from 'recharts';

interface RealTimeChartProps {
  data: Array<{ time: number; value: number }>;
  color: string;
  title: string;
  unit: string;
  yDomain: [number, number];
}

const RealTimeChart: React.FC<RealTimeChartProps> = ({
  data,
  color,
  title,
  unit,
  yDomain,
}) => {
  const containerStyle = {
    backgroundColor: '#16213e',
    borderRadius: '0.5rem',
    padding: '1rem',
  };

  const titleStyle = {
    color: '#00d4ff',
    fontWeight: '500',
    marginBottom: '0.5rem',
  };

  return (
    <div style={containerStyle}>
      <h3 style={titleStyle}>{title}</h3>
      <ResponsiveContainer width="100%" height={150}>
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" stroke="#1a1a2e" />
          <XAxis 
            dataKey="time" 
            stroke="#888888"
            fontSize={12}
            tickFormatter={() => ''}
          />
          <YAxis 
            stroke="#888888"
            fontSize={12}
            domain={yDomain}
            tickFormatter={(value) => `${value}${unit}`}
          />
          <Tooltip
            contentStyle={{
              backgroundColor: '#1a1a2e',
              border: '1px solid #0f3460',
              borderRadius: '8px',
            }}
            labelFormatter={() => ''}
            formatter={(value) => {
              const numericValue = typeof value === 'number' ? value : Number(value ?? 0);
              return [`${numericValue.toFixed(1)}${unit}`, title];
            }}
          />
          <Line
            type="monotone"
            dataKey="value"
            stroke={color}
            strokeWidth={2}
            dot={false}
            isAnimationActive={false}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default RealTimeChart;
