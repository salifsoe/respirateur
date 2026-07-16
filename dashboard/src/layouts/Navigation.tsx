/**
 * Navigation layout component
 * Provides navigation between pages
 */

import React from 'react';
import { NavLink } from 'react-router-dom';
import { FaHome, FaBell, FaCog } from 'react-icons/fa';

const Navigation: React.FC = () => {
  const navStyle = {
    backgroundColor: '#16213e',
    borderBottom: '1px solid #374151',
  };

  const containerStyle = {
    maxWidth: '1200px',
    margin: '0 auto',
    padding: '0 1.5rem',
  };

  const flexStyle = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    height: '4rem',
  };

  const logoStyle = {
    fontSize: '1.5rem',
    fontWeight: 'bold',
    color: '#00d4ff',
  };

  const linkStyle = {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    padding: '0.5rem 1rem',
    borderRadius: '0.5rem',
    transition: 'all 0.15s',
    textDecoration: 'none',
  };

  return (
    <nav style={navStyle}>
      <div style={containerStyle}>
        <div style={flexStyle}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
            <span style={logoStyle}>MED-VENT</span>
          </div>
          
          <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
            <NavLink
              to="/"
              style={({ isActive }) => ({
                ...linkStyle,
                backgroundColor: isActive ? '#00d4ff' : 'transparent',
                color: isActive ? '#ffffff' : '#9ca3af',
              })}
            >
              <FaHome />
              <span>Tableau de bord</span>
            </NavLink>
            
            <NavLink
              to="/alarms"
              style={({ isActive }) => ({
                ...linkStyle,
                backgroundColor: isActive ? '#00d4ff' : 'transparent',
                color: isActive ? '#ffffff' : '#9ca3af',
              })}
            >
              <FaBell />
              <span>Alarmes</span>
            </NavLink>
            
            <NavLink
              to="/settings"
              style={({ isActive }) => ({
                ...linkStyle,
                backgroundColor: isActive ? '#00d4ff' : 'transparent',
                color: isActive ? '#ffffff' : '#9ca3af',
              })}
            >
              <FaCog />
              <span>Paramètres</span>
            </NavLink>
          </div>
        </div>
      </div>
    </nav>
  );
};

export default Navigation;
