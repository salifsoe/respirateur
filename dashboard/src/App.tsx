/**
 * Main App component with React Router
 */

import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import Navigation from './layouts/Navigation';
import Dashboard from './pages/Dashboard';
import Alarms from './pages/Alarms';
import Settings from './pages/Settings';

function App() {
  return (
    <Router>
      <div style={{ minHeight: '100vh', backgroundColor: '#0a0a0f', color: '#ffffff' }}>
        <Navigation />
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/alarms" element={<Alarms />} />
          <Route path="/settings" element={<Settings />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;
