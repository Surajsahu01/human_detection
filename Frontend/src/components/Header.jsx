import React from 'react'

function Header() {
  return (
    <div>
    <header className="bg-white shadow-md p-4 flex justify-between items-center">
      <h1 className="text-2xl font-bold text-indigo-600">👁 Human Presence Dashboard</h1>
      <span className="text-sm text-gray-500">Real-time Monitoring</span>
    </header>
    </div>
  )
}

export default Header