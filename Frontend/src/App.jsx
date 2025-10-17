import { Route, Router, Routes } from "react-router-dom"
import Navbar from "./components/Navbar"
import Dashboard from "./pages/Dashboard"
// import Settings from "./pages/Settings"
// import Logs from "./pages/Logs"


function App() {


  return (
    
      <div className="min-h-screen bg-gray-50">
        <Navbar />
        <Routes>
          <Route path="/" element={<Dashboard />} />
          {/* <Route path="/logs" element={<Logs />} /> */}
          {/* <Route path="/settings" element={<Settings />} /> */}
        </Routes>
      </div>
    

  )
}

export default App
