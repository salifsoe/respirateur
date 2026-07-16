/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        background: '#0a0a0f',
        card: '#1a1a2e',
        cardLight: '#16213e',
        accent: '#0f3460',
        primary: '#00d4ff',
        success: '#00ff00',
        warning: '#ffaa00',
        danger: '#ff0000',
      },
    },
  },
  plugins: [],
  corePlugins: {
    preflight: true,
  },
}
