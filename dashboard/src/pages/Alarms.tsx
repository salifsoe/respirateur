/**
 * Alarms page
 * Displays alarm history with filters and search
 */

import React, { useState } from 'react';
import { FaExclamationTriangle, FaSearch, FaFilter } from 'react-icons/fa';
import type { Alarm, AlarmSeverity } from '../types';

const Alarms: React.FC = () => {
  const [alarms] = useState<Alarm[]>([
    {
      id: '1',
      message: 'High pressure detected',
      time: '09:45',
      severity: 'critical',
      active: true,
    },
    {
      id: '2',
      message: 'Low battery warning',
      time: '09:30',
      severity: 'warning',
      active: false,
    },
    {
      id: '3',
      message: 'WiFi connection lost',
      time: '08:15',
      severity: 'warning',
      active: false,
    },
    {
      id: '4',
      message: 'System started',
      time: '08:00',
      severity: 'info',
      active: false,
    },
  ]);

  const [filter, setFilter] = useState<AlarmSeverity | 'all'>('all');
  const [searchTerm, setSearchTerm] = useState('');

  const getSeverityColor = (severity: AlarmSeverity) => {
    switch (severity) {
      case 'critical':
        return { bg: 'rgba(255, 0, 0, 0.2)', border: '#ff0000', text: '#ff0000' };
      case 'warning':
        return { bg: 'rgba(255, 170, 0, 0.2)', border: '#ffaa00', text: '#ffaa00' };
      case 'info':
        return { bg: 'rgba(0, 255, 0, 0.2)', border: '#00ff00', text: '#00ff00' };
      default:
        return { bg: '#16213e', border: '#4b5563', text: '#9ca3af' };
    }
  };

  const getSeverityBadge = (severity: AlarmSeverity) => {
    switch (severity) {
      case 'critical':
        return { bg: '#ff0000', text: '#ffffff' };
      case 'warning':
        return { bg: '#ffaa00', text: '#000000' };
      case 'info':
        return { bg: '#00ff00', text: '#000000' };
      default:
        return { bg: '#6b7280', text: '#ffffff' };
    }
  };

  const filteredAlarms = alarms.filter((alarm) => {
    const matchesFilter = filter === 'all' || alarm.severity === filter;
    const matchesSearch = alarm.message.toLowerCase().includes(searchTerm.toLowerCase());
    return matchesFilter && matchesSearch;
  });

  const containerStyle: React.CSSProperties = {
    padding: '1.5rem',
  };

  const spaceStyle: React.CSSProperties = {
    display: 'flex',
    flexDirection: 'column' as const,
    gap: '1.5rem',
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

  const filterBarStyle: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    gap: '1rem',
    backgroundColor: '#16213e',
    padding: '1rem',
    borderRadius: '0.5rem',
  };

  const searchContainerStyle: React.CSSProperties = {
    position: 'relative',
    flex: 1,
  };

  const searchIconStyle: React.CSSProperties = {
    position: 'absolute',
    left: '0.75rem',
    top: '50%',
    transform: 'translateY(-50%)',
    color: '#9ca3af',
  };

  const inputStyle: React.CSSProperties = {
    width: '100%',
    backgroundColor: '#1a1a2e',
    border: '1px solid #4b5563',
    borderRadius: '0.5rem',
    paddingLeft: '2.5rem',
    paddingRight: '1rem',
    paddingTop: '0.5rem',
    paddingBottom: '0.5rem',
    color: '#ffffff',
  };

  const selectStyle: React.CSSProperties = {
    backgroundColor: '#1a1a2e',
    border: '1px solid #4b5563',
    borderRadius: '0.5rem',
    paddingLeft: '1rem',
    paddingRight: '1rem',
    paddingTop: '0.5rem',
    paddingBottom: '0.5rem',
    color: '#ffffff',
  };

  const alarmItemStyle: React.CSSProperties = {
    padding: '1rem',
    borderRadius: '0.5rem',
    border: '2px solid',
  };

  const emptyStateStyle: React.CSSProperties = {
    textAlign: 'center',
    paddingTop: '3rem',
    paddingBottom: '3rem',
    color: '#9ca3af',
  };

  return (
    <div style={{ ...containerStyle, ...spaceStyle }}>
      {/* Header */}
      <div>
        <h1 style={headerTitleStyle}>Alarmes</h1>
        <p style={headerSubtitleStyle}>Historique et statut des alarmes</p>
      </div>

      {/* Filters */}
      <div style={filterBarStyle}>
        {/* Search */}
        <div style={searchContainerStyle}>
          <FaSearch style={searchIconStyle} />
          <input
            type="text"
            placeholder="Rechercher des alarmes..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            style={inputStyle}
          />
        </div>

        {/* Filter */}
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          <FaFilter style={{ color: '#9ca3af' }} />
          <select
            value={filter}
            onChange={(e) => setFilter(e.target.value as AlarmSeverity | 'all')}
            style={selectStyle}
          >
            <option value="all">Toutes les gravités</option>
            <option value="critical">Critique</option>
            <option value="warning">Avertissement</option>
            <option value="info">Info</option>
          </select>
        </div>
      </div>

      {/* Alarms List */}
      <div style={{ display: 'flex', flexDirection: 'column' as const, gap: '0.75rem' }}>
        {filteredAlarms.length === 0 ? (
          <div style={emptyStateStyle}>
            Aucune alarme trouvée
          </div>
        ) : (
          filteredAlarms.map((alarm) => {
            const colors = getSeverityColor(alarm.severity);
            const badgeColors = getSeverityBadge(alarm.severity);
            
            return (
              <div
                key={alarm.id}
                style={{
                  ...alarmItemStyle,
                  backgroundColor: colors.bg,
                  borderColor: colors.border,
                  animation: alarm.active ? 'pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite' : 'none',
                }}
              >
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                  <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
                    <FaExclamationTriangle style={{ fontSize: '1.25rem', color: colors.text }} />
                    <div>
                      <h3 style={{ fontWeight: '600', color: colors.text }}>{alarm.message}</h3>
                      <p style={{ fontSize: '0.875rem', opacity: 0.75, color: colors.text }}>{alarm.time}</p>
                    </div>
                  </div>
                  <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
                    <span style={{
                      padding: '0.25rem 0.75rem',
                      borderRadius: '9999px',
                      fontSize: '0.75rem',
                      fontWeight: '600',
                      backgroundColor: badgeColors.bg,
                      color: badgeColors.text,
                    }}>
                      {alarm.severity.toUpperCase()}
                    </span>
                    {alarm.active && (
                      <span style={{
                        padding: '0.25rem 0.75rem',
                        borderRadius: '9999px',
                        fontSize: '0.75rem',
                        fontWeight: '600',
                        backgroundColor: 'rgba(255, 255, 255, 0.2)',
                        color: '#ffffff',
                      }}>
                        ACTIF
                      </span>
                    )}
                  </div>
                </div>
              </div>
            );
          })
        )}
      </div>
    </div>
  );
};

export default Alarms;
