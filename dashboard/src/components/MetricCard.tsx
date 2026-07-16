/**
 * MetricCard component
 * Reusable card for displaying ventilator metrics
 */

import React from 'react';

interface MetricCardProps {
  title: string;
  value: string | number;
  unit?: string;
  icon?: React.ReactNode;
  status?: 'normal' | 'warning' | 'danger';
  className?: string;
}

const MetricCard: React.FC<MetricCardProps> = ({
  title,
  value,
  unit = '',
  icon,
  status = 'normal',
  className = '',
}) => {
  const getStatusColor = () => {
    switch (status) {
      case 'warning':
        return '#ffaa00';
      case 'danger':
        return '#ff0000';
      default:
        return 'transparent';
    }
  };

  const getStatusBg = () => {
    switch (status) {
      case 'warning':
        return 'rgba(255, 170, 0, 0.1)';
      case 'danger':
        return 'rgba(255, 0, 0, 0.1)';
      default:
        return '#16213e';
    }
  };

  const cardStyle = {
    padding: '1rem',
    borderRadius: '0.5rem',
    border: '2px solid',
    borderColor: getStatusColor(),
    backgroundColor: getStatusBg(),
  };

  const headerStyle = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    marginBottom: '0.5rem',
  };

  const titleStyle = {
    color: '#9ca3af',
    fontSize: '0.875rem',
    fontWeight: '500',
  };

  const valueContainerStyle = {
    display: 'flex',
    alignItems: 'baseline',
    gap: '0.25rem',
  };

  const valueStyle = {
    fontSize: '1.5rem',
    fontWeight: 'bold',
    color: '#ffffff',
  };

  const unitStyle = {
    fontSize: '0.875rem',
    color: '#00d4ff',
  };

  return (
    <div className={className} style={cardStyle}>
      <div style={headerStyle}>
        <span style={titleStyle}>{title}</span>
        {icon && <div style={{ color: '#00d4ff' }}>{icon}</div>}
      </div>
      <div style={valueContainerStyle}>
        <span style={valueStyle}>{value}</span>
        {unit && <span style={unitStyle}>{unit}</span>}
      </div>
    </div>
  );
};

export default MetricCard;
