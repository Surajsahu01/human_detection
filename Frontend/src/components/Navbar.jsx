import React, { useState } from "react";
import { NavLink } from "react-router-dom";
import { Menu, X } from "lucide-react"; // install: npm i lucide-react

function Navbar() {
  const [isOpen, setIsOpen] = useState(false);

  const linkClass =
    "block px-4 py-2 text-sm font-medium rounded-lg hover:bg-gray-100 transition";
  const activeClass = "bg-gray-200 text-blue-600";

  const toggleMenu = () => setIsOpen(!isOpen);

  return (
    <div className="p-2 sm:p-4 bg-gray-50 flex justify-center">
      <nav className="w-full max-w-7xl border-2 border-gray-300 rounded-2xl shadow-md bg-white px-6 py-3">
        <div className="flex items-center justify-between">
          {/* Logo / Title */}
          <h1 className="text-xl md:text-2xl font-semibold text-gray-700">
            👁 Human Presence Dashboard
          </h1>

          {/* Desktop Links */}
          <div className="hidden md:flex space-x-4">
            <NavLink
              to="/"
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Dashboard
            </NavLink>
            {/* <NavLink
              to="/logs"
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Logs
            </NavLink>
            <NavLink
              to="/settings"
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Settings
            </NavLink> */}
          </div>

          {/* Mobile Menu Button */}
          <button
            className="md:hidden text-gray-700 hover:text-blue-600 focus:outline-none"
            onClick={toggleMenu}
          >
            {isOpen ? <X size={24} /> : <Menu size={24} />}
          </button>
        </div>

        {/* Mobile Menu */}
        {isOpen && (
          <div className="md:hidden mt-3 border-t border-gray-200">
            <NavLink
              to="/"
              onClick={() => setIsOpen(false)}
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Dashboard
            </NavLink>
            {/* <NavLink
              to="/logs"
              onClick={() => setIsOpen(false)}
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Logs
            </NavLink>
            <NavLink
              to="/settings"
              onClick={() => setIsOpen(false)}
              className={({ isActive }) =>
                `${linkClass} ${isActive ? activeClass : ""}`
              }
            >
              Settings
            </NavLink> */}
          </div>
        )}
      </nav>
    </div>
  );
}

export default Navbar;
