/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        ladakh: {
          darkest: '#050811',
          bg: '#080d1a',
          surface: '#0d1527',
          card: '#111a33',
          border: 'rgba(0, 240, 255, 0.16)',
          cyan: '#00f0ff',
          cyanDim: 'rgba(0, 240, 255, 0.65)',
          blue: '#0ea5e9',
          green: '#10b981',
          greenDim: 'rgba(16, 185, 129, 0.2)',
          amber: '#f59e0b',
          amberDim: 'rgba(245, 158, 11, 0.2)',
          red: '#ef4444',
          redDim: 'rgba(239, 68, 68, 0.25)',
        }
      },
      fontFamily: {
        mono: ['ui-monospace', 'SFMono-Regular', 'Menlo', 'Monaco', 'Consolas', 'monospace'],
      },
      boxShadow: {
        'hud-glow': '0 0 15px rgba(0, 240, 255, 0.15)',
        'hud-glow-red': '0 0 20px rgba(239, 68, 68, 0.25)',
        'hud-glow-green': '0 0 20px rgba(16, 185, 129, 0.2)',
        'hud-glow-amber': '0 0 20px rgba(245, 158, 11, 0.2)',
      },
      animation: {
        'pulse-slow': 'pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite',
        'radar-sweep': 'spin 4s linear infinite',
      }
    },
  },
  plugins: [],
}
