/**
 * Settings page
 * Interface for modifying ventilation parameters
 */

import React, { useState } from 'react';
import { FaSave, FaUndo } from 'react-icons/fa';
import type { VentilatorSettings } from '../types';
import { mqttService } from '../services/mqtt.service';

const Settings: React.FC = () => {
  const [settings, setSettings] = useState<VentilatorSettings>({
    mode: 'VCV',
    fr: 15,
    volume: 500,
    peep: 5,
    ie_ratio: 0.5,
  });

  const [hasChanges, setHasChanges] = useState(false);
  const [saveStatus, setSaveStatus] = useState<string>('');

  const handleSave = async () => {
    try {
      if (!mqttService.isConnected()) {
        await mqttService.connect();
      }

      mqttService.publishSettings(settings);
      setHasChanges(false);
      setSaveStatus('Parametres envoyes a l ESP32 via MQTT.');
    } catch (error) {
      console.error('Unable to publish settings:', error);
      setSaveStatus('Impossible d envoyer les parametres. Verifiez le broker MQTT.');
    }
  };

  const handleReset = () => {
    setSettings({
      mode: 'VCV',
      fr: 15,
      volume: 500,
      peep: 5,
      ie_ratio: 0.5,
    });
    setHasChanges(false);
  };

  const handleChange = <K extends keyof VentilatorSettings>(
    field: K,
    value: VentilatorSettings[K],
  ) => {
    setSettings(prev => ({ ...prev, [field]: value }));
    setHasChanges(true);
    setSaveStatus('');
  };

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

  const formStyle: React.CSSProperties = {
    backgroundColor: '#16213e',
    borderRadius: '0.5rem',
    padding: '1.5rem',
    display: 'flex',
    flexDirection: 'column' as const,
    gap: '1.5rem',
  };

  const labelStyle: React.CSSProperties = {
    display: 'block',
    fontSize: '0.875rem',
    fontWeight: '500',
    color: '#d1d5db',
    marginBottom: '0.5rem',
  };

  const modeGridStyle: React.CSSProperties = {
    display: 'grid',
    gridTemplateColumns: 'repeat(3, 1fr)',
    gap: '0.75rem',
  };

  const modeButtonStyle: React.CSSProperties = {
    padding: '1rem',
    borderRadius: '0.5rem',
    border: '2px solid',
    cursor: 'pointer',
    transition: 'all 0.15s',
  };

  const sliderStyle: React.CSSProperties = {
    width: '100%',
    height: '0.5rem',
    backgroundColor: '#1a1a2e',
    borderRadius: '0.5rem',
    appearance: 'none',
    cursor: 'pointer',
  };

  const sliderRangeStyle: React.CSSProperties = {
    display: 'flex',
    justifyContent: 'space-between',
    fontSize: '0.75rem',
    color: '#6b7280',
    marginTop: '0.25rem',
  };

  const buttonContainerStyle: React.CSSProperties = {
    display: 'flex',
    justifyContent: 'flex-end',
    gap: '0.75rem',
  };

  const buttonStyle: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    padding: '0.75rem 1.5rem',
    borderRadius: '0.5rem',
    cursor: 'pointer',
    border: 'none',
    transition: 'all 0.15s',
  };

  const warningStyle: React.CSSProperties = {
    backgroundColor: 'rgba(255, 170, 0, 0.2)',
    border: '1px solid #ffaa00',
    borderRadius: '0.5rem',
    padding: '1rem',
  };

  return (
    <div style={{ ...containerStyle, ...spaceStyle }}>
      {/* Header */}
      <div>
        <h1 style={headerTitleStyle}>Paramètres</h1>
        <p style={headerSubtitleStyle}>Configurer les paramètres de ventilation</p>
      </div>

      {/* Settings Form */}
      <div style={formStyle}>
        {/* Mode Selection */}
        <div>
          <label style={labelStyle}>
            Mode de ventilation
          </label>
          <div style={modeGridStyle}>
            {(['VCV', 'PCV', 'VAC'] as const).map((mode) => (
              <button
                key={mode}
                onClick={() => handleChange('mode', mode)}
                style={{
                  ...modeButtonStyle,
                  borderColor: settings.mode === mode ? '#00d4ff' : '#4b5563',
                  backgroundColor: settings.mode === mode ? 'rgba(0, 212, 255, 0.2)' : '#1a1a2e',
                  color: settings.mode === mode ? '#ffffff' : '#9ca3af',
                }}
              >
                <div style={{ fontWeight: '600' }}>{mode}</div>
                <div style={{ fontSize: '0.75rem', marginTop: '0.25rem', opacity: 0.75 }}>
                  {mode === 'VCV' && 'Volume Contrôlé'}
                  {mode === 'PCV' && 'Pression Contrôlée'}
                  {mode === 'VAC' && 'Volume Assisté'}
                </div>
              </button>
            ))}
          </div>
        </div>

        {/* Respiratory Rate */}
        <div>
          <label style={labelStyle}>
            Fréquence respiratoire: {settings.fr} cycles/min
          </label>
          <input
            type="range"
            min="5"
            max="40"
            value={settings.fr}
            onChange={(e) => handleChange('fr', parseInt(e.target.value))}
            style={sliderStyle}
          />
          <div style={sliderRangeStyle}>
            <span>5</span>
            <span>40</span>
          </div>
        </div>

        {/* Tidal Volume */}
        <div>
          <label style={labelStyle}>
            Volume courant: {settings.volume} mL
          </label>
          <input
            type="range"
            min="200"
            max="800"
            step="10"
            value={settings.volume}
            onChange={(e) => handleChange('volume', parseInt(e.target.value))}
            style={sliderStyle}
          />
          <div style={sliderRangeStyle}>
            <span>200</span>
            <span>800</span>
          </div>
        </div>

        {/* PEEP */}
        <div>
          <label style={labelStyle}>
            PEEP: {settings.peep} cmH₂O
          </label>
          <input
            type="range"
            min="0"
            max="20"
            step="0.5"
            value={settings.peep}
            onChange={(e) => handleChange('peep', parseFloat(e.target.value))}
            style={sliderStyle}
          />
          <div style={sliderRangeStyle}>
            <span>0</span>
            <span>20</span>
          </div>
        </div>

        {/* I:E Ratio */}
        <div>
          <label style={labelStyle}>
            Rapport I:E: 1:{(1 / settings.ie_ratio).toFixed(1)}
          </label>
          <input
            type="range"
            min="0.2"
            max="1"
            step="0.1"
            value={settings.ie_ratio}
            onChange={(e) => handleChange('ie_ratio', parseFloat(e.target.value))}
            style={sliderStyle}
          />
          <div style={sliderRangeStyle}>
            <span>1:5</span>
            <span>1:1</span>
          </div>
        </div>
      </div>

      {/* Action Buttons */}
      <div style={buttonContainerStyle}>
        <button
          onClick={handleReset}
          disabled={!hasChanges}
          style={{
            ...buttonStyle,
            backgroundColor: '#16213e',
            border: '1px solid #4b5563',
            color: '#d1d5db',
            opacity: hasChanges ? 1 : 0.5,
            cursor: hasChanges ? 'pointer' : 'not-allowed',
          }}
        >
          <FaUndo />
          Réinitialiser
        </button>
        <button
          onClick={handleSave}
          disabled={!hasChanges}
          style={{
            ...buttonStyle,
            backgroundColor: '#00d4ff',
            color: '#ffffff',
            opacity: hasChanges ? 1 : 0.5,
            cursor: hasChanges ? 'pointer' : 'not-allowed',
          }}
        >
          <FaSave />
          Enregistrer les modifications
        </button>
      </div>

      {saveStatus && (
        <div style={{
          backgroundColor: '#16213e',
          border: '1px solid #0f3460',
          borderRadius: '0.5rem',
          color: '#d1d5db',
          padding: '0.75rem 1rem',
        }}>
          {saveStatus}
        </div>
      )}

      {/* Warning */}
      <div style={warningStyle}>
        <p style={{ color: '#ffaa00', fontSize: '0.875rem' }}>
          <strong>Avertissement:</strong> La modification des paramètres de ventilation affecte les soins aux patients.
          Assurez-vous d'une supervision médicale appropriée avant d'appliquer les modifications.
        </p>
      </div>
    </div>
  );
};

export default Settings;
